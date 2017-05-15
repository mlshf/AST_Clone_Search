#ifndef LEXICAL_H_INCLUDED
#define LEXICAL_H_INCLUDED

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

//function finds out what token str contains and if it's an ID
int What_Keyword(string str, string* output, vector<string>* id_and_num)
//then changes it to ID - for parameter and stores information about encountered IDs and constants real "names"
{

    vector<string> keywords {"auto","break","case","char","const","continue","default",
                        "do","double","else","enum","extern","float","for","goto",
                        "if","int","long","register","return","short","signed",
                        "sizeof","static","struct","switch","typedef","union",
                        "unsigned","void","volatile","while", "bool"};

    for(size_t i = 0; i < keywords.size(); ++i)//checking if str contains a keyword
    {
        if(str.compare(keywords[i]) == 0)
        {

            *output += str + " ";

            return 0;
        }
    }

            if(isdigit(str[0]))//checking if str contains a number
            {
                for(size_t j = 1; j < str.size(); ++j)
                {
                    if(!isdigit(str[j]))
                    {
                        return 1;
                    }
                }

                *output += str + " ";//for constant

            }
            else//checking if str contains an identifier
            {
                if(!isalpha(str[0]) && str[0] != '_')
                {
                    return 1;
                }

                for(size_t j = 1; j < str.size(); ++j)
                {
                    if( (isdigit(str[j]) || isalpha(str[j]) || str[j] == '_') == 0 )
                    {
                        return 1;
                    }
                }

                *output += "ID";//for identifier
                id_and_num->push_back(str);

            }

    return 0;
}

//this function parameterizes given in_str and stores it into output, id_and_num is used in What_Keyword function
int Parametrization(string in_str, string* output, vector<string>* id_and_num, char showflag, ofstream& logfile)
{
    char ch;
    string  separators = " \t\n\v\f\r,;(){}[]#\"'" , operators = "!%^&*-+=~|.<>/?:";//vector serparators contains all symbols that can separate tokens from each other
    //vector operators contains all operators of C language
	size_t i = 0;
	string str_temp;
	while(i < in_str.size())
	{
        ch = in_str[i];//with variable ch i read the given line in_str
        ++i;
        str_temp += ch;


        if(operators.find(ch) != string::npos)//if ch is an operator
        {
            if(str_temp.size() != 0)
            {
                str_temp.erase(str_temp.end() - 1);
                if(str_temp.size() != 0)
                {
                    if(What_Keyword(str_temp, output, id_and_num) == 1)
                    {
                        if(showflag == 1)
                            logfile << str_temp << " is not a C lexeme..." << endl;
                        return 1;
                    }
                    str_temp.clear();
                }
            }

            if( ch == '/' && i < in_str.size() && in_str[i] == '/' )
                break;

            string temp(1, ch);
            *output += temp;

        }
        else
        {

            if(separators.find(ch) != string::npos)//if ch is a separator
            {
                if(str_temp.size() != 0)
                    str_temp.erase(str_temp.end() - 1);

                if(ch == '#')//special case - include, define, ifndef and etc.
                {
                    str_temp += ch;
                    while(i < in_str.size() && ch != '>' && ch != '"' && in_str[i] != '\n')
                    {
                        ch = in_str[i];
                        str_temp += ch;
                        i++;
                    }
                    if(ch == '"')
                    {
                        ch = in_str[i];
                        str_temp += ch;
                        i++;
                        while(i < in_str.size() && ch != '"')
                        {
                            ch = in_str[i];
                            str_temp += ch;
                            i++;
                        }
                    }

                    *output += str_temp;

                    str_temp.clear();
                }
                else
                {
                    if(ch == '"')//special case - constant char string
                    {
                        str_temp += ch;
                        do
                            {if(i < in_str.size())
                            {

                                ch = in_str[i];
                                i++;
                                str_temp += ch;
                            }
                            else
                                i = in_str.size() + 1;


                        }while(i <= in_str.size() && ch != '"');

                        *output += str_temp;

                        str_temp.clear();
                    }
                    else
                    {
                        if(ch == '\'')//special case - constant char string
                        {
                            str_temp += ch;
                            do
                            {
                                if(i < in_str.size())
                                {
                                    ch = in_str[i];
                                    i++;
                                    str_temp += ch;
                                }
                                else
                                    i = in_str.size() + 1;

                            }while(i <= in_str.size() && ch != '\'');

                            *output += str_temp;

                            str_temp.clear();
                        }
                        else
                        {

                            if(str_temp.size() != 0)
                            {
                                while(i < in_str.size() && isspace(ch))//ignoring space symbols
                                {
                                    ch = in_str[i];
                                    ++i;
                                }

                                if(ch == '(')//found a function call or definition
                                {
                                    if( output->size() > 0 && operators.find( (*output)[output->size() - 1] ) == string::npos )
                                    {
                                        *output += "CBP4_PARAMETERIZED_FUNCNAME_POSSIBLE_FUNCDEF";
                                    }
                                    else
                                        if( output->size() > 0 && (*output)[output->size() - 1] == '*' )
                                        {
                                            *output += "CBP4_PARAMETERIZED_FUNCNAME_POSSIBLE_FUNCDEF";
                                        }
                                        else
                                        {
                                            *output += str_temp;
                                        }
                                }
                                else
                                {
                                    if(ch == '{')//found a structure or union or compound operator
                                    {

                                        *output += str_temp;

                                    }
                                    else
                                    if(str_temp.size() != 0)
                                    {
                                        if(What_Keyword(str_temp, output, id_and_num) == 1)
                                        {
                                            if(showflag == 1)
                                                logfile << str_temp << " is not a C lexeme..." << endl;
                                            return 1;
                                        }
                                    }
                                }

                                str_temp.clear();

                                if( separators.find(ch) == string::npos )//as we have read a little more we have to find out where we are now
                                {
                                    if(operators.find(ch) != string::npos)
                                    {
                                        if(str_temp.size() != 0)
                                        {
                                            str_temp.erase(str_temp.end() - 1);
                                            if(str_temp.size() != 0)
                                            {
                                                if(What_Keyword(str_temp, output, id_and_num) == 1)
                                                {
                                                    if(showflag == 1)
                                                        logfile << str_temp << " is not a C lexeme..." << endl;
                                                    return 1;
                                                }
                                                str_temp.clear();
                                            }
                                        }

                                        string temp(1, ch);
                                        *output += temp;

                                    }
                                    else
                                    {
                                        str_temp += ch;
                                    }
                                }
                                else
                                {
                                    string temp(1, ch);
                                    *output += temp;
                                }
                            }
                            else
                            {
                                if(!isspace(ch))
                                {
                                    string temp(1, ch);
                                    *output += temp;
                                }
                            }

                        }

                    }
                }
            }
        }
    }

    //this is used for checking last lexeme - while loop will not find variable or constant if it's last token in the string
    if(separators.find( in_str[ in_str.size() - 1 ] ) == string::npos && operators.find( in_str[ in_str.size() - 1 ] ) == string::npos)
    {
        if(str_temp.size() != 0)
        {
            if(What_Keyword(str_temp, output, id_and_num) == 1)
            {
                if(showflag == 1)
                    logfile << str_temp << " is not a C lexeme..." << endl;
                return 1;
            }
            str_temp.clear();
        }
    }

    return 0;
}

#endif // LEXICAL_H_INCLUDED
