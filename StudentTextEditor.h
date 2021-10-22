#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include <string>
#include <list>

class Undo;

class StudentTextEditor : public TextEditor {
public:

	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:
    std::list<std::string> lineList;
    std::list<std::string>::iterator cursor;
    struct Coord
    {
        int currRow;
        int currCol;
    };
    Coord* currentPos;
    void moveCursor(std::list<std::string>::iterator& mover, int startRow, int endRow);
    bool undoIt;
};

#endif // STUDENTTEXTEDITOR_H_
