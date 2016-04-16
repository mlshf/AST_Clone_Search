#ifndef HISTORY_ANALYSIS_H_INCLUDED
#define HISTORY_ANALYSIS_H_INCLUDED

#include "cluster.h"
#include "cluster_init.h"
#include "git_exec.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>
#include <cctype>
#include <cmath>

using namespace std;

struct Commit_Level
{
    long long level;
    vector<string> SHA1_of_commits;
};

int Are_There_Equal_Strings(vector<string>* VS, string S)
{
    int Found_Equal = 0;
    for(size_t i = 0; i < VS->size() && Found_Equal != 1; ++i)
    {
        if(S.compare((*VS)[i]) == 0)
            Found_Equal = 1;
    }
    return Found_Equal;
}

void Analyze_History(vector<Commit_Level>* Commit_Levels)
{
    size_t i = 1;
    int unchecked_commits = (int)(*Commit_Levels)[i - 1].SHA1_of_commits.size();

    while(unchecked_commits > 0)
    {
        Commit_Level Level_i;
        Level_i.level = i;
        //cout << "Previous level size is: " << (*Commit_Levels)[i - 1].SHA1_of_commits.size() << endl;

        for(size_t j = 0; j < (*Commit_Levels)[i - 1].SHA1_of_commits.size(); ++j)
        {
            vector<string> Vector_of_SHA1;
            exec_git_getsha1((*Commit_Levels)[i - 1].SHA1_of_commits[j], &Vector_of_SHA1);

            for(size_t t = 0; t < Vector_of_SHA1.size(); ++t)
                if(Are_There_Equal_Strings(&(Level_i.SHA1_of_commits), Vector_of_SHA1[t]) != 1)
                    Level_i.SHA1_of_commits.push_back(Vector_of_SHA1[t]);

            --unchecked_commits;
        }

        Commit_Levels->push_back(Level_i);
        ++i;
        unchecked_commits = (int)Level_i.SHA1_of_commits.size();
        //cout << "Current level size is: " << unchecked_commits << endl << endl;

    }

    return;
}

#endif // HISTORY_ANALYSIS_H_INCLUDED
