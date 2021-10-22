#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();
    ~StudentUndo();

private:
    struct undoDetail
    {
        Undo::Action action;
        char operationChar;
        int currentRow;
        int currentCol;
        undoDetail(Undo::Action myAction, char myChar, int myRow, int myCol)
        {
            action = myAction;
            operationChar = myChar;
            currentRow = myRow;
            currentCol = myCol;
        } //keeps track of any actions, row, col, and char that were involved with the undo
    };
    std::stack<undoDetail*> undoStack;
    void reverse(std::string& myString); //reverses string
};

#endif // STUDENTUNDO_H_
