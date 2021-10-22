#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
     lineList.push_back("");
     cursor = lineList.begin();
     currentPos = new Coord;
     currentPos -> currRow = 0;
     currentPos -> currCol = 0;
     undoIt = true;
}

StudentTextEditor::~StudentTextEditor()
{
    delete currentPos;
}

bool StudentTextEditor::load(std::string file) {
    reset();
    getUndo() -> clear();
    std::ifstream myFile(file);
    
    if( !myFile )
    {
        return false;
    }else{
        std::string s;
        while(getline(myFile, s))
        {
            //stripping off the carriage return if string is not empty and has a carriage return and pushing into my list
            if(!(s.empty()) && s.back() == '\r')
            {
                s.pop_back();
            }
            lineList.push_back(s);
        }
        //setting the iterator to the starting row of the text
        cursor = lineList.begin();
        
        return true;
    }
}

bool StudentTextEditor::save(std::string file) {
    
    std::ofstream outfile(file);
    if(!outfile) //sees if the file can be saved
    {
        return false;
    }else{
        std::list<std::string>::iterator p = lineList.begin(); //sets temp iterator to list's beginning
        for(; p != lineList.end(); p++)
        {
            outfile<<(*p) + '\n'; //adds the string with a newline to the end
        }
        return true;
    }
}


void StudentTextEditor::reset() {
    
    //clearing the list of its contents
    lineList.clear();
    
    //setting the cursor's coordinate to (0,0)
    currentPos -> currRow = 0;
    currentPos -> currCol = 0;
}

void StudentTextEditor::move(Dir dir) {
    //cannot be just past the end (p signifies the last line)
    std::list<std::string>::iterator p = lineList.end();
    p--;
	switch(dir)
    {
        case Dir::UP:
            if(cursor != lineList.begin())
            {
                int prevLength = cursor -> length();
                cursor--;
                (currentPos -> currRow) -= 1;
                //if next line is shorter than current line
                if((currentPos -> currCol) > (cursor -> length()))
                {
                    (currentPos -> currCol) = (cursor -> length());
                }
                
            }
            break;
        case Dir::DOWN:
            if(cursor != p)
            {
                cursor++;
                (currentPos -> currRow) += 1;
                //if next line is shorter than current line
                if((currentPos -> currCol) > (cursor -> length()))
                {
                    (currentPos -> currCol) = (cursor -> length());
                }
            }
            break;
        case Dir::LEFT:
            if((currentPos -> currCol) == 0) //case where cursor is at the beginning of a line
            {
                if(cursor != lineList.begin()) //do nothing if at the beginning of text
                {
                    cursor--;
                    (currentPos -> currRow) -= 1;
                    currentPos -> currCol = cursor -> length();
                }
            }else{
                (currentPos -> currCol) -= 1;
            }
            break;
        case Dir::RIGHT:
            if(currentPos -> currCol == cursor -> length())
            {
                //addresses the case of being at the end of a line
                //but not the end of the whole text
                if(cursor != p)
                {
                    cursor++;
                    currentPos -> currRow += 1;
                    currentPos -> currCol = 0;
                }
            }else{
                (currentPos -> currCol) += 1;
            }
            break;
        case Dir::HOME:
            (currentPos -> currCol) = 0;
            break;
        case Dir::END:
            (currentPos -> currCol)= cursor -> length();
            break;
    }
}

void StudentTextEditor::del() {
    //in the case that the cursor is just past the character at the end of the line
    if((currentPos -> currCol) == cursor -> length())
    {
        std::list<std::string>::iterator lastLine = lineList.end();
        lastLine--;
        //this situation does not apply if the cursor is on the last line
        if(cursor != lastLine)
        {
            std::list<std::string>::iterator temp = cursor;
            temp++;
            std::string append = *temp;
            //take the current line out from the list
            lineList.erase(temp);
            //add the portion of the line to the end of the current line
            (*cursor) += append;
            
            if(undoIt)
            {
                getUndo() -> submit(Undo::Action::JOIN, currentPos -> currRow, currentPos -> currCol);
            }
        }
        
        
    }else{
        //the normal case
        char deletedChar = cursor -> at(currentPos -> currCol);
        cursor -> erase((currentPos -> currCol), 1);
        if(undoIt)
        {
            getUndo() -> submit(Undo::Action::DELETE,currentPos -> currRow, currentPos -> currCol, deletedChar);
        }
    }
    
    
}

void StudentTextEditor::backspace() {
    //in the case that the current cursor is in between the line
    if(currentPos -> currCol > 0)
    {
        //erase the previous character, shift cursor one to the left
        char deletedChar = cursor -> at((currentPos -> currCol) - 1);
        cursor -> erase((currentPos -> currCol) - 1, 1);
        currentPos -> currCol -= 1;
        if(undoIt)
        {
            getUndo() -> submit(Undo::Action::DELETE, currentPos -> currRow, currentPos -> currCol, deletedChar);
        }
    }
    if(currentPos -> currCol == 0)
    {
        if((cursor != lineList.begin()) && (currentPos -> currRow != 0))
        {
            std::list<std::string>::iterator temp = cursor;
            temp--;
            //keep track of how long the line is to shift the cursor's column position
            int tempCol = temp -> length();
            std::string append =  *cursor;
            //delete the line that is being merged
            cursor = lineList.erase(cursor);
            cursor--;
            *cursor += append;
            //adjust the position of the cursor
            currentPos -> currRow -= 1;
            currentPos -> currCol = tempCol;
            
            if(undoIt)
            {
                getUndo() -> submit(Undo::Action::JOIN, currentPos -> currRow, currentPos -> currCol);
            }
        }
    }
}

void StudentTextEditor::insert(char ch) {
    
    //old, inefficient version
    /**
    if(ch == '\t')
    {
        for(int j = 0; j < 4; j++)
        {
            (*cursor) += ' ';
            for(int i = (cursor -> length() - 1); i > (currentPos -> currCol); i--)
            {
                swap(cursor -> at(i), cursor -> at(i-1));
            }
            (currentPos -> currCol) += 1;
        }
        
    }else{
        std::string blah = *cursor;
        int cRow = currentPos -> currRow;
        int cCol = currentPos -> currCol;
        (*cursor) += ch;
        for(int i = (cursor -> length() - 1); i > (currentPos -> currCol); i--)
        {
            swap(cursor -> at(i), cursor -> at(i-1));
        }
        (currentPos -> currCol) += 1;
    }
    */
    //add four consecutive spaces and move the cursor four spots to the right if tab
    if(ch == '\t')
    {
        cursor -> insert(currentPos -> currCol, 4, ' ');
        if(undoIt)
        {
            for(int i = 0; i < 4; i++)
            {
                currentPos -> currCol += 1;
                getUndo() -> submit(Undo::Action::INSERT, (currentPos -> currRow), (currentPos -> currCol), ' ');
            }
        }else{
            (currentPos -> currCol) += 4;
        }
        
    }else{
        //else add the desired character and move one space to the right
        cursor -> insert(currentPos -> currCol, 1, ch);
        currentPos -> currCol += 1;
        if(undoIt)
        {
            getUndo() -> submit(Undo::Action::INSERT, currentPos -> currRow, currentPos -> currCol, ch);
        }
        
    }
    
    
}

void StudentTextEditor::enter() {
    
    std::list<std::string>::iterator p = lineList.end();
    p--;
    //in the case that editing position is just past the last line
    //push back a new empty line in the text
    if(cursor == p && (currentPos -> currCol) == cursor -> length())
    {
        if(undoIt)
        {
            getUndo() -> submit(Undo::Action::SPLIT, currentPos -> currRow, currentPos -> currCol);
        }
        
        currentPos -> currRow += 1;
        currentPos -> currCol = 0;
        lineList.push_back("");
        cursor++;
        
    }else{
        //otherwise insert a new line and move and move cursor back to the desired position
        //after doing so
        if(undoIt)
        {
            getUndo() -> submit(Undo::Action::SPLIT, currentPos -> currRow, currentPos -> currCol);
        }
        std::string part1 = cursor -> substr(0, (currentPos -> currCol));
        std::string part2 = cursor -> substr(currentPos -> currCol);
        *cursor = part1;
        currentPos -> currRow += 1;
        currentPos -> currCol = 0;
        cursor++;
        //inserting the new line with substring portion of past line
        lineList.insert(cursor,part2);
        cursor--;
    }
    
}

void StudentTextEditor::getPos(int& row, int& col) const {
    row = currentPos -> currRow;
    col = currentPos -> currCol;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
    
    if(startRow < 0 || numRows < 0 || startRow > lineList.size())
    {
        return -1;
    }else{
        int numLines = 0;
        
        //clear the contents of the vector lines
        lines.clear();
        
        //create a copy of the current row iterator
        std::list<std::string>::const_iterator p = cursor;
        
        //simple inefficient version that works
        /**
        for(; p != lineList.end(); p++)
        {
            if(numLines == startRow)
            {
                break;
            }
            numLines++;
        }
        numLines = 0;
        for(; (p != lineList.end()) && (numLines < numRows); p++)
        {
            lines.push_back(*p);
            numLines++;
        }
        return numLines;*/
        
        
        
        //a better version
        
        int tempRow = currentPos -> currRow;
        
        if(tempRow < startRow) //case where the current row is less than the start row
        {
            for(; p != lineList.end() && (tempRow != startRow); p++)
            {
                if(tempRow == startRow)
                {
                    break;
                }
                tempRow++;
            }
        }else if(tempRow > startRow){ //case where the current row is greater than the start row
            for(; p != lineList.begin() && (tempRow != startRow); p--)
            {
                if(tempRow == startRow)
                {
                    break;
                }
                tempRow--;
            }
        }
        
        //pushing back elements into the vector
        for(; (p != lineList.end()) && (numLines < numRows); p++)
        {
            lines.push_back(*p);
            numLines++;
        }
        
        return numLines;
        
        
    }

}

void StudentTextEditor::undo() {
    //obtaining the information needed for the undo operation
    int row = 0;
    int col = 0;
    int numChars = 0;
    std::string text;
    Undo::Action undoAct = getUndo() -> get(row, col, numChars, text);
    //actually executing the undo operation
    switch(undoAct)
    {
        case Undo::Action::DELETE:
            moveCursor(cursor, currentPos -> currRow, row);
            currentPos -> currRow = row;
            currentPos -> currCol = col;
            undoIt = false; //sets it to false, to not store in stack
            for(int i = 0; i < numChars; i++)
            {
                del();
            }
            undoIt = true; //sets it to true, for other actions to store in stack
            break;
        case Undo::Action::INSERT:
            moveCursor(cursor, currentPos -> currRow, row);
            currentPos -> currRow = row;
            currentPos -> currCol = col;
            undoIt = false;
            cursor -> insert(currentPos -> currCol, text);
            undoIt = true;
            break;
        case Undo::Action::SPLIT:
            moveCursor(cursor, currentPos -> currRow, row);
            currentPos -> currRow = row;
            currentPos -> currCol = col;
            undoIt = false;
            enter();
            cursor--;
            currentPos -> currCol = cursor -> length(); //re-inserting at the end of above row
            currentPos -> currRow -= 1;
            undoIt = true;
            break;
        case Undo::Action::JOIN:
            moveCursor(cursor, currentPos -> currRow, row);
            currentPos -> currRow = row;
            currentPos -> currCol = col;
            undoIt = false;
            del();
            undoIt = true;
            break;
    }
}

void StudentTextEditor::moveCursor(std::list<std::string>::iterator& mover, int startRow, int endRow)
{
    if((startRow) < endRow) //case where the current row is less than the start row
    {
        for(; mover != lineList.end() && (startRow != endRow); mover++)
        {
            if(startRow == endRow)
            {
                break;
            }
            startRow += 1;
        }
    }else if(startRow > endRow){ //case where the current row is greater than the start row
        for(; mover != lineList.begin() && (startRow != endRow); mover--)
        {
            if(startRow == endRow)
            {
                break;
            }
            startRow -= 1;
        }
    }
}

