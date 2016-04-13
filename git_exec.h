#ifndef GIT_EXEC_H_INCLUDED
#define GIT_EXEC_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>

using namespace std;

int exec_git_command(string S)//It just executes command and prints the result
{
    FILE* in;
    char buff[512];

    cout << S << endl << endl;

    if(!(in = popen(S.c_str(), "r")))
    {
        return 1;
    }

    while(fgets(buff, sizeof(buff), in) != NULL)
    {
        cout << buff;
    }

    cout << endl;

    pclose(in);

    return 0;
}

int exec_git_getsha1(string S, vector<string>* VS)//It executes command and reads the result. Then prints it.
//Accepts string S, that contains only SHA1 of
{
    FILE* in;

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
        return 1;
    }

    while(!feof(in))
    {

        char str[64];
        fscanf(in, "%[^\n]\n", str);

        string S_temp(str);
        VS->push_back(S_temp);
        cout << S_temp << endl;

    }

    cout << endl;

    pclose(in);

    return 0;
}

#endif // GIT EXEC_H_INCLUDED
