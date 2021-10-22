#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck();
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
    
    struct Node
    {
        char letter;
        bool isWord;
        Node* arr[27];
        //bool for tracking end of word
        //letter stored inside
        //array for the next char that follows
        Node(char myLetter, bool isItWord)
        {
            letter = myLetter;
            isWord = isItWord;
            for(int i = 0; i < 27; i++)
            {
                arr[i] = nullptr;
            }
        }
    };
    Node* head; //root to the rest of the trie
    int charToIndex(char someChar); //converts char to index position its at in my trie
    void insertIntoTree(Node*& head,char someChar,bool isEnd); //inserts char into trie
    bool checkValidChar(char someChar); //checks if its a letter or apostrophe
    void deleteTrie(Node* head); //deletes the trie
    bool search(std::string text); //searches if word is in dictionary
    
};

#endif  // STUDENTSPELLCHECK_H_
