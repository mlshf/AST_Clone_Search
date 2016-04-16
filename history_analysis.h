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

//creates vector of levels of commits
//level 0 - starting commit
//level i+1 - level of commits-descendants of commits from level i
int Fill_Commit_Levels(vector<Commit_Level>* Commit_Levels)
{
    size_t i = 1;//number of current level; 1 at the beginning of the process
    int unchecked_commits = (int)(*Commit_Levels)[i - 1].SHA1_of_commits.size();//as we work we have two sets of commits
    //first one is that we use to form the second one. unchecked commits is its size at the beginning of a step. it decreases as we find descendants of one of
    //this set's commits and add them to the second set
    //second one is the one that is being formed using the first one. it contains all the descendant-commits of all the commits from the first set
    //on the i+1 step of the algorithm the second set of i step becomes the first set for i+1 step

    while(unchecked_commits > 0)
    {
        Commit_Level Level_i;
        Level_i.level = i;
        //cout << "Previous level size is: " << (*Commit_Levels)[i - 1].SHA1_of_commits.size() << endl;

        for(size_t j = 0; j < (*Commit_Levels)[i - 1].SHA1_of_commits.size(); ++j)
        {
            vector<string> Vector_of_SHA1;
            if(exec_git_getsha1((*Commit_Levels)[i - 1].SHA1_of_commits[j], &Vector_of_SHA1) == 1)
            {
                cout << "COULD NOT FILL COMMIT LEVELS..." << endl;
                return 1;
            }

            for(size_t t = 0; t < Vector_of_SHA1.size(); ++t)
                if(Are_There_Equal_Strings(&(Level_i.SHA1_of_commits), Vector_of_SHA1[t]) != 1)//SHA1 is added to level_i's vector of commits only if it is not already there
                    Level_i.SHA1_of_commits.push_back(Vector_of_SHA1[t]);

            --unchecked_commits;//as we found the descendants for one of the commits from the parent commit set we decrease the number of unprocessed parent commits
        }

        Commit_Levels->push_back(Level_i);
        ++i;
        unchecked_commits = (int)Level_i.SHA1_of_commits.size();
        //cout << "Current level size is: " << unchecked_commits << endl << endl;

    }

    return 0;
}

#endif // HISTORY_ANALYSIS_H_INCLUDED
