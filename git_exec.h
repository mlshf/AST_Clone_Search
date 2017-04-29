#ifndef GIT_EXEC_H_INCLUDED
#define GIT_EXEC_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>
#include <stddef.h>

using namespace std;

int exec_git_command(string S)//It just executes command and prints the result
{
    FILE* in;
    //char buff[512];

    cout << S << endl << endl;

    if(!(in = popen(S.c_str(), "r")))
    {
        cout << "COMMAND COULD NOT BE EXECUTED. ABORTING..." << endl;
        return 1;
    }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, in)) != -1)
    {
        cout << line;
    }

    cout << endl;

    pclose(in);
    if (line)
        free(line);

    return 0;
}

int exec_command(string command, vector<string>* result)
{
    FILE* in;
    //char buff[512];

    cout << command << endl << endl;

    if(!(in = popen(command.c_str(), "r")))
    {
        cout << "COMMAND COULD NOT BE EXECUTED. ABORTING..." << endl;
        return 1;
    }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, in)) != -1)
    {
        string str_line(line);
        result->push_back(str_line);
    }

    cout << endl;

    pclose(in);
    if (line)
        free(line);

    return 0;
}

int Is_Char_String_Not_Empty(char S_temp[])//1 - does contain non-space characters, 0 otherwise
{
    int String_Not_Empty = 0;
    for(size_t t = 0; t < strlen(S_temp) && String_Not_Empty != 1; ++t)
    {
        char c = S_temp[t];
        if(isspace(c) == 0 && c != EOF && (isalpha(c) || isdigit(c)))
            String_Not_Empty = 1;
    }
    return String_Not_Empty;
}

int exec_git_getsha1(string S, vector<string>* VS)//It executes command and reads the result. Then prints it.
//Accepts string S - SHA1 of the commit, whose descendant commits we look for
//VS contains the list of descendant commits of commit with SHA1 ~ S
{
    FILE* in;

    //git rev-list --all --parents | grep "^.\{40\}.*<PARENT_COMMIT_SHA1>.*" | awk '{print $1}'
    string S_exec("git rev-list --all --parents | grep ");
    S_exec += '"';
    S_exec += "^.";
    S_exec += "\\";
    S_exec += "{40";
    S_exec += "\\";
    S_exec += "}.*";
    S_exec += S;
    S_exec += ".*";
    S_exec += '"';
    S_exec += " | awk '{print $1}'";
    cout << S_exec << endl << endl;

    if(!(in = popen(S_exec.c_str(), "r")))
    {
        cout << "COMMAND COULD NOT BE EXECUTED. ABORTING..." << endl;
        return 1;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, in)) != -1)
    {
        string S_temp(line);
        if(S_temp.size() > 40)
            S_temp.erase(40);
        cout << S_temp << endl;
        VS->push_back(S_temp);

    }

    cout << endl;
    if (line)
        free(line);

    pclose(in);

    return 0;
}

#endif // GIT EXEC_H_INCLUDED
