//Byron Simmons, Jaylan Hunter
//COMP 360 Project 1 February 25th, 2024

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <map>
#include <vector>
#include <regex>

using namespace std;

char singleChar[15] = {'-', '+', '*', '/', '=', '[', ']', '{', '}', '(', ')', ';', ' ', '<', '>'};
char listOfNumbers[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
std::vector<string> localVars ;
string keywords[3] = {"int", "void", "while"};
bool previouslyKeyword = false;

vector<std::pair<string, string>> vectorToken;
int globalIndex;
string errorMessage = "";

bool checkChar(char input) 
{
    for (char c : singleChar) 
    {
        if (input == c) {return true;}
    }
    return false;
}

bool isNumber(char input) 
{
    for (char c : listOfNumbers)
    {
        if (input == c) { return true; }
    }
    return false;
}

bool checkLocalVar(string variableName)
{
    for (string name : localVars)
    {
        if (variableName == name) {return true;}
    }
    return false;
}

string parseWord(string input, int i)
{
    string result = "";
    if (!isNumber(input[i])) 
    {
        while (!checkChar(input[i]))
        {
            result += input[i];
            i++;
        }
    }
    return result;
}

string parseKeywords(string input)
{
    if (checkLocalVar(input)) 
    {
        previouslyKeyword = false;
        return "IDENTIFIER";
    }

    if (previouslyKeyword) 
    {
        previouslyKeyword = false;
        localVars.push_back(input);
        return "IDENTIFIER";
    }

    if (input == "void") 
    {
        previouslyKeyword = true;
        return "KEYWORD";
    }
    if (input == "int") 
    {
        previouslyKeyword = true;
        return "KEYWORD";
    }
    if (input == "while")
    {
        previouslyKeyword = false;
        return "KEYWORD";
    }
    else {return "NULL";}
}

bool expected(char input, char match)
{
    if (input == match)
    {
        globalIndex += 1;
        return true;
    }
    else {return false;}
}

bool checkKeyword();
bool checkIdentifier();
bool checkDeclares();
bool checkLoop();
bool checkAssignment();
bool checkConst();

//Placeholder Functions for Syntax/EBNF Analyisis

vector<std::pair<string, string>> parseTokens(string input)
{
    vector<std::pair<string, string>> resultVec;
    string s; 
    for (int i = 0; i < input.length(); i++)
    {
        s = "";

        if (isNumber(input[i])) 
        {
            int subIndex = i+1;
            s += input[i];
            while (isNumber(input[subIndex]))
            {
                s += input[subIndex];
                subIndex += 1;
            }
            resultVec.push_back(std::make_pair(s, "DIGIT"));
            i = subIndex-1;
        }
        else
        {
            if (checkChar(input[i])) // found things like +, -. Meaning that this is a something that can be identified with a single char.
            {
                switch (input[i])
                {
                    case '+': case '-': case '*': case '/': case '=': case '<': case '>':
                    s += input[i];
                    resultVec.push_back(std::make_pair(s, "OPERATOR"));
                    //cout << "OPERATOR FOUND" << endl;
                    break;

                    case '(': case '[': case '{':
                    s += input[i];
                    resultVec.push_back(std::make_pair(s, "L_BRACE"));
                    //cout << "L_BRACE FOUND" << endl;
                    break;

                    case ')': case ']': case '}':
                    s += input[i];
                    resultVec.push_back(std::make_pair(s, "R_BRACE"));
                    //cout << "R_BRACE FOUND" << endl;
                    break;

                    case ';':
                    s += input[i];
                    resultVec.push_back(std::make_pair(s, "SEMICOLON"));
                    break;
                    //cout << "SEMICOLON FOUND" << endl;
                    
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    s += input[i];
                    resultVec.push_back(std::make_pair(s, "DIGIT"));
                    break;
                }
            }
            else // find keywords, identifiers and anything other single chars
            {
                s = parseWord(input, i);
                i += s.length()-1;
                resultVec.push_back(std::make_pair(s, parseKeywords(s)));
            }
        }
    }        
    return resultVec;
}

void checkProgram() 
{
    if (checkKeyword() && checkIdentifier() && expected(vectorToken[globalIndex].first[0], '(') && expected(vectorToken[globalIndex].first[0] ,')'))
    {
        if (expected(vectorToken[globalIndex].first[0], '{') && checkDeclares() && checkLoop() && checkAssignment() && expected(vectorToken[globalIndex].first[0], '}'))
        {
            cout << "The test program is generated by the EBNF Grammar for Simple Function" << endl;
        }
        else { cout << "The test program cannot be generated by the EBNF Grammar for Simple Function. The first syntax error is: (" << errorMessage << ")" << endl; }
    }
    else { cout << "The test program cannot be generated by the EBNF Grammar for Simple Function. The first syntax error is: (" << errorMessage << ")" << endl; }
} 

bool checkDeclares() 
{
    if (checkKeyword() && checkIdentifier() && expected(vectorToken[globalIndex].first[0], '=') && checkConst() && expected(vectorToken[globalIndex].first[0], ';'))
    {
        return true;
    }
    else 
    {
        errorMessage = "ERROR: Delcare Error at command index: " + globalIndex;
        return false; 
    }
}

bool checkLoop()
{
    if (checkKeyword() && expected(vectorToken[globalIndex].first[0], '(') && checkIdentifier() && expected(vectorToken[globalIndex].first[0], '>') && expected(vectorToken[globalIndex].first[0], '=') && checkConst() && expected(vectorToken[globalIndex].first[0], ')')) 
    {
        return true;
    }
    else 
    {
        errorMessage = "ERROR: Loop Error at command index: " + globalIndex;
        return false; 
    }
}

bool checkAssignment() 
{
    if (checkIdentifier() && expected(vectorToken[globalIndex].first[0], '=') && checkIdentifier() && expected(vectorToken[globalIndex].first[0], '-' ) && checkConst() && expected(vectorToken[globalIndex].first[0], ';')) 
    {
        return true;
    }
    else 
    {
        errorMessage = "ERROR: Assignment Error at command index: " + globalIndex;
        return false;
    }
}

bool checkKeyword()
{
    if (vectorToken[globalIndex].first == "int" || vectorToken[globalIndex].first == "while" || vectorToken[globalIndex].first == "void")
    { 
        globalIndex += 1;
        return true;
    }
    else 
    {
        errorMessage = "ERROR: Current Token -> " + vectorToken[globalIndex].first + " <- is NOT a keyword.";
        return false;
    }
}

bool checkConst()
{
    if (vectorToken[globalIndex].first == "21" || vectorToken[globalIndex].first == "1" || vectorToken[globalIndex].first == "10")
    { 
        globalIndex += 1;
        return true;
    }
    else 
    {
        errorMessage = "ERROR: Current Token -> " + vectorToken[globalIndex].first + " <- is NOT a valid constant.";
        return false;
    }
}

bool checkIdentifier()
{
    if (isalpha(vectorToken[globalIndex].first[0]))
    {
        globalIndex += 1;
        return true;
    }
    else 
    {
        errorMessage = "ERROR: Current Token -> " + vectorToken[globalIndex].first + " <- is NOT a valid identifier.";
        return false;
    }
}

int main () 
{
    cout << "Welcome to COMP360 Project 1" << endl;
    cout << "Please type '1' to designate TEST PROGRAM 1. Please type '2' to designate TEST PROGRAM 2" << endl;

    string userInput;
    cin >> userInput;

    while (userInput != "1" && userInput != "2")
    {
        cout << "ERROR: Invalid output. Please input a '1' or '2'." << endl;
        cin >> userInput;
    }

    cout << "----------------------------------------------------------------" << endl;
    cout << "<-> Parsing Pairs {Lexemes : Tokens} <->" << endl << endl;
    
    std::ifstream file;
    if (userInput == "1") {file.open("Test Program 1.txt");}
    if (userInput == "2") {file.open("Test Program 2.txt");}

    string newLine;
    string input;

    while (file) {std::getline(file, newLine); input += newLine;}

    vectorToken = parseTokens(input); //Lexical Analyzer

    for (int i = 0; i < vectorToken.size(); i++)
    {
        cout << vectorToken[i].first << " : " << vectorToken[i].second << endl;
    }

     cout << "----------------------------------------------------------------" << endl;
    cout << "<-> Syntax Analysis {EBNF Grammar} <->" << endl << endl;
    checkProgram(); // Recursive Descent Parser + EBNF Analyzer
}