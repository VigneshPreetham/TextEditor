#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <fstream>
#include <cctype>
#include <iostream>

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

//initialize the head ptr
StudentSpellCheck::StudentSpellCheck()
{
    head = new Node(' ', false);
    
}

//delete the trie
StudentSpellCheck::~StudentSpellCheck() {
    deleteTrie(head);
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
    deleteTrie(head);
    head = new Node(' ', false);
    std::ifstream infile(dictionaryFile);
    if(!infile)
    {
        return false;
    }
    std::string s;
    while(getline(infile, s))
    {
        std::string temp;
        int lengthOfLine = s.length();
        
        //strips and checks for valid chars
        for(int i = 0; i < lengthOfLine; i++)
        {
            if(checkValidChar(tolower(s[i])))
            {
                temp.push_back(tolower(s[i]));
            }
        }
        
        
        int newLength = temp.length();
        Node* tempPtr = head;
        //inserts chars into tree
        for(int j = 0; j < newLength; j++)
        {
            if(j != newLength - 1)
            {
                insertIntoTree(tempPtr,temp[j], false);
            }else{
                insertIntoTree(tempPtr,temp[j], true);
            }
            
        }
        
    }
    return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
    
    std::string test = word;
    //converts string to all lowercase
    for(int i = 0; i < test.length(); i++)
    {
        test[i] = tolower(test[i]);
    }
    //if found return true
    if(search(test))
    {
        return true;
    }else{
        int numSuggestions = 0;
        char possibleChars[27] = {'a','b','c','d','e','f','g','h','i', 'j', 'k', 'l', 'm', 'n', 'o', 'p','q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\''};
        suggestions.clear();
        int testLength = test.length();
        //goes through and checks all possible combos excluding current character
        for(int i = 0; i < testLength; i++)
        {
            if(numSuggestions < max_suggestions)
            {
                char existing = tolower(test[i]);
                for(int j = 0; j < 27; j++)
                {
                    if(existing != possibleChars[j])
                    {
                        test[i] = possibleChars[j];
                        if(search(test))
                        {
                            suggestions.push_back(test);
                            numSuggestions++;
                        }
                    }
                    
                }
                test[i] = existing;
            }else{
                break;
            }
        }
        return false;
    }
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
    int lengthText = line.length();
    int wordStart = -1;
    int wordEnd = -1;
    //increments each the end by one until bad character and marks the start
    for(int i = 0; i < lengthText; i++)
    {
        if(isalpha(line[i]) || line[i] == '\'')
        {
            if(wordStart == -1)
            {
                wordStart = i;
            }
            wordEnd = i;
        }else{
            if(wordStart != -1 && wordEnd != -1)
            {
                bool isSpelledCorrectly = search(line.substr(wordStart, wordEnd - wordStart + 1));
                if(!isSpelledCorrectly)
                {
                    Position word;
                    word.start = wordStart;
                    word.end = wordEnd;
                    problems.push_back(word);

                }
                
            }
            wordStart = -1;
            wordEnd = -1;
        }
    }
    //in case of a loop exit before adding the word
    if(wordStart != -1 && wordEnd != -1)
    {
        bool isSpelledCorrectly = search(line.substr(wordStart, wordEnd - wordStart + 1));
        if(!isSpelledCorrectly)
        {
            Position word;
            word.start = wordStart;
            word.end = wordEnd;
            problems.push_back(word);

        }
        
    }
}

int StudentSpellCheck::charToIndex(char someChar)
{
    int index = -1;
    someChar = tolower(someChar);
    bool notFoundYet = false;
    switch(someChar)
    {
        case 'a':
            index = 0;
            break;
        case '\'':
            index = 26;
            break;
        default:
            notFoundYet = true;
            break;
    }
    
    if(notFoundYet)
    {
        index = someChar - 'a';
    }
    return index;
    
}
void StudentSpellCheck::insertIntoTree(Node*& head,char someChar, bool isEnd)
{
    //inserts particular char into tree
    int indexSearch = charToIndex(someChar);
    while(head != nullptr)
    {
        if(head -> arr[indexSearch] == nullptr)
        {
            head -> arr[indexSearch] = new Node(someChar, isEnd);
            head = head -> arr[indexSearch];
            break;
        }else{
            //if the same character is already there progress to next character in word to add to trie
            bool isSameChar = false;
            if((head -> arr[indexSearch] -> letter) == someChar)
            {
                isSameChar = true;
            }
            head = head -> arr[indexSearch];
            if(isSameChar)
            {
                break;
            }
        }
    }
}

bool StudentSpellCheck::checkValidChar(char someChar)
{
    //checks if the character is a valid character
    if(isalpha(someChar) || someChar == '\'')
    {
        return true;
    }
    return false;
}

void StudentSpellCheck::deleteTrie(Node* head)
{
    //deletes trie recursively
    if(head != nullptr)
    {
        for(int i = 0; i < 27; i++)
        {
            deleteTrie(head -> arr[i]);
        }
        delete head;
    }
}

bool StudentSpellCheck::search(std::string text)
{
    //searches through and tries to find the word (helper for spellcheck)
    int textLength = text.length();
    Node* temp = head;
    for(int i = 0; i < textLength; i++)
    {
        int indexChar = charToIndex(text[i]);
        if(temp != nullptr)
        {
            if(temp -> arr[indexChar] == nullptr)
            {
                return false;
            }else{
                if(i == textLength - 1)
                {
                    if(temp -> arr[indexChar] -> isWord == false)
                    {
                        return false;
                    }
                }
                temp = temp -> arr[indexChar];
            }
        }
    }
    return true;
    
}
