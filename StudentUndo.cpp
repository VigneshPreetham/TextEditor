#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
    //after student text editor submits, reverses the action and then feeds into get
    switch(action)
    {
        case Undo::Action::INSERT:
            undoStack.push(new undoDetail(Undo::Action::DELETE, ch, row, col - 1));
            break;
        case Undo::Action::DELETE:
            undoStack.push(new undoDetail(Undo::Action::INSERT, ch, row, col));
            break;
        case Undo::Action::JOIN:
            undoStack.push(new undoDetail(Undo::Action::SPLIT, ch, row, col));
            break;
        case Undo::Action::SPLIT:
            undoStack.push(new undoDetail(Undo::Action::JOIN, ch, row, col));
            break;
            
    }
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
    
    if(!undoStack.empty())
    {
        text = "";
        count = 0;
        bool isdelete = false;
        bool isbackspace = false;
        row = undoStack.top() -> currentRow;
        col = undoStack.top() -> currentCol;
        int backCol = col;
        int tempCol = col;
        Undo::Action undoAct = undoStack.top() -> action;
        //check for the next action for batching
        if(!undoStack.empty())
        {
            if(undoAct == Undo::Action::INSERT)
            {
                text += (undoStack.top() -> operationChar);
            }
            delete undoStack.top();
            undoStack.pop();
            if(!undoStack.empty())
            {
                if(undoStack.top() -> currentCol == col + 1 && undoStack.top() -> action == undoAct && undoStack.top() -> action == Undo::Action::INSERT)
                {
                    isbackspace = true;
                }
            }
        }
        if(undoAct == Undo::Action::DELETE)
        {
            isdelete = true;
            count++;
        }
        
        //this is the loop which checks for batching if it happens
        while(!undoStack.empty() && undoStack.top() -> action == undoAct && undoStack.top() -> currentRow == row && ((undoStack.top() -> currentCol == tempCol - 1) || (undoStack.top() -> currentCol == tempCol) || (isbackspace && (undoStack.top() -> currentCol == tempCol + 1))))
        {

            //if not delete action (basically not undoing a insert), append to string
            if(!(undoAct == Undo::Action::DELETE))
            {
                text += (undoStack.top() -> operationChar);
            }
            
            //pop current stack item
            if(!undoStack.empty())
            {
                row = undoStack.top() -> currentRow;
                col = undoStack.top() -> currentCol;
                delete undoStack.top();
                undoStack.pop();
            }
            //for backspace, you need to compare previous item with next
            if(isbackspace)
            {
                tempCol++;
            }

            if(undoAct == Undo::Action::DELETE)
            {
                tempCol -= 1;
                count++;
            }
        }
        //reversing the text string
        
        if(undoAct == Undo::Action::INSERT && !isbackspace)
        {
            reverse(text);
        }
        if(isbackspace)
        {
            col = backCol;
        }
        if(undoAct != Undo::Action::DELETE)
        {
            count = 1;
        }
        
        
        return undoAct;
    }else{
        return Undo::Action::ERROR;
    }
    
}

void StudentUndo::clear() {
    //clears the stack popping anything thats there
    while(!undoStack.empty())
    {
        delete undoStack.top();
        undoStack.pop();
    }
}

StudentUndo::~StudentUndo()
{
    //clear out the stack
    while(!undoStack.empty())
    {
        delete undoStack.top();
        undoStack.pop();
    }
}
void StudentUndo::reverse(std::string& myString)
{
    //reversing a string for backspace
    int length = myString.length();
    for(int i = 0; i < length/2; i++)
    {
        char temp = myString.at(i);
        myString.at(i) = myString.at(length - i - 1);
        myString.at(length - i - 1) = temp;
    }
}
