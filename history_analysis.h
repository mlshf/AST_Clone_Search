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
#include "lexical.h"

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
int Fill_Commit_Levels(vector<Commit_Level>* Commit_Levels, vector<string>* Start_SHA1)
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

    //now we need to exclude commits that are listed in file argv[1] from Commit_Levels, because we consider them already processed
    for(size_t i = 0; i < Commit_Levels->size(); ++i)
    {
        size_t j = 0;
        while( j < (*Commit_Levels)[i].SHA1_of_commits.size() )
        {
            int Not_Initial = 1;
            for(size_t k = 1; k < Start_SHA1->size() && Not_Initial == 1; ++k)
            {
                if( (*Start_SHA1)[k].compare( (*Commit_Levels)[i].SHA1_of_commits[j] ) == 0 )
                    Not_Initial = 0;
            }

            if(Not_Initial == 0)
            {
                (*Commit_Levels)[i].SHA1_of_commits.erase( (*Commit_Levels)[i].SHA1_of_commits.begin() + j );
            }
            else
                ++j;
        }
    }

    return 0;
}

void Find_Indices_of_Clusters(string S_compared, vector<Cluster>* Clusters, vector<size_t>* Need_to_Compare, string path_to_fake_libc)//returns number of cluster that contains similar original string or -1 otherwise
{
    vector<string> first;
    first.push_back(S_compared);

    for(size_t i = 0; i < Clusters->size(); ++i)
    {
        int j = (*Clusters)[i].commits[0].files[0].exemplars[0].fragment.size() / 2;//because size is always 2 * FragmentSize + 1, middle string index is j
        /*if(S_compared.compare( (*Clusters)[i].commits[0].files[0].exemplars[0].fragment[j] ) == 0)
            Need_to_Compare->push_back(i);*/
        vector<string> second;
        second.push_back( (*Clusters)[i].commits[0].files[0].exemplars[0].fragment[j] );

        Exemplar exemplar1;
        exemplar1.fragment = first;

        Exemplar exemplar2;
        exemplar2.fragment = second;

        if(Exemplars_Are_Equal(exemplar1, exemplar2, path_to_fake_libc) == 1)
            Need_to_Compare->push_back(i);
    }
    return;
}

int Analyze_History(vector<Commit_Level>* Commit_Levels, vector<Cluster>* Clusters, size_t FragmentSize, string path_to_fake_libc)
{
    for(size_t i = 1; i < Commit_Levels->size(); ++i)
    {
        for(size_t j = 0; j < (*Commit_Levels)[i].SHA1_of_commits.size(); ++j)
        {
            string git_command("git checkout ");
            git_command += (*Commit_Levels)[i].SHA1_of_commits[j];
            if(exec_git_command(git_command) == 1)//checking out to a commit from commit_levels
                return 1;
            vector<string> Paths;//stores paths to all files of current commit with allowed extension
            list_dir_contents(&Paths);

            for(size_t k = 0; k < Paths.size(); ++k)
            {
                ifstream in_file(Paths[k].c_str(), ios_base::in);
                if(!in_file.is_open())
                {
                    std::cout << "FILE : " << Paths[k] << " COULD NOT BE OPENED. ABORTING..." << std::endl;
                    return 1;
                }

                vector<string> previous;//contains strings before commentary in number of FragmentSize
                //and if needed commentary is found then contains previous.size() + more

                long long line = 0;//stores current number of line that was read

                while(!in_file.eof())
                {
                    std::string S_temp;//S_temp stores line that has been read
                    std::getline(in_file, S_temp);
                    line++;//number of current line

                    Delete_Extra_Spaces(&S_temp);

                    //if we've read not end of file, not empty string and not a commentary
                    if(!in_file.eof() && Is_String_Not_Empty(S_temp) == 1 && S_temp[0] != '/' && S_temp[0] != '#')
                    {
                        //we check if last line in prev does not end with ; or { or } - meaning it's an incomplete line
                        if(previous.size() > 0)
                        {
                            string last = previous[previous.size() - 1];//last string from previous
                            //incomplete line can be found for example when defining a function in some of C coding styles
                            if(last[last.size() - 1] != ';' && last[last.size() - 1] != '}' && last[last.size() - 1] != '{')
                            {
                                S_temp = previous[previous.size() - 1] + " " + S_temp;//new S_temp is last + S_temp
                                previous.pop_back();//because last element was incomplete - we will add it again - full this time
                            }
                        }

                        Delete_Extra_Spaces(&S_temp);

                        std::string S_temp_alt;
                        std::vector<string> temp_unused;

                        if(Parametrization(S_temp, &S_temp_alt, &temp_unused) == 1)//if we encountered a function declaration then
                        {//we'll check if we encountered a function declaration and skip it if so
                            cout << "String number " << line << " contains NOT C lexeme." << endl;
                            cout << S_temp << endl << S_temp_alt << endl;
                            return 1;
                        }

                        if( S_temp_alt.find("FUNCDEF(") != std::string::npos || S_temp_alt.find("FUNCDEF (") != std::string::npos )
                        {
                            previous.clear();//at this point we have some strings in PREVIOUS and we encountered a string that contains
                        }
                        else
                        {
                            vector<size_t> Need_to_Compare;//will contain indices of clusters, that have marked string similar to current string
                            if( (S_temp.size() >= 2 && S_temp[0] == '/' && S_temp[1] == '/') || (S_temp.size() >= 1 && S_temp[0] == '#' ) )
                                S_temp = " ";
                            else
                            {
                                Find_Indices_of_Clusters(S_temp, Clusters, &Need_to_Compare, path_to_fake_libc);
                            }

                            //if current line is in defect lines we need to form a fragment
                            if(!in_file.eof() && Need_to_Compare.size() > 0 )
                            {
                                Exemplar Exmplr;
                                Exmplr.line = line;

                                int j = 0, prev_size = previous.size();//prev_size - size of previous[] before adding string with weakness
                                //j counts number of lines that have been added to previous[]
                                previous.push_back(S_temp);

                                while(j < prev_size && !in_file.eof())
                                {
                                    std::string S_temp2;
                                    std::getline(in_file, S_temp2);
                                    line++;

                                    Delete_Extra_Spaces(&S_temp2);

                                    //if we did not encounter end of file, string is not empty and it is not a commentary
                                    if(!in_file.eof() && Is_String_Not_Empty(S_temp2) == 1 && S_temp2[0] != '/' && S_temp[0] != '#')
                                    {
                                        //we check if last line in prev does not end with ; or { or } - meaning it's an incomplete line
                                        if(previous.size() > 0)
                                        {
                                            string last = previous[previous.size() - 1];//last string from previous
                                            //incomplete line can be found for example when defining a function in some of C coding styles
                                            if(last[last.size() - 1] != ';' && last[last.size() - 1] != '}' && last[last.size() - 1] != '{')
                                            {
                                                S_temp2 = previous[previous.size() - 1] + " " + S_temp2;//new S_temp is last + S_temp
                                                previous.pop_back();//because last element was incomplete - we will add it again - full this time
                                            }
                                        }

                                        Delete_Extra_Spaces(&S_temp2);

                                        std::string S_temp2_alt;
                                        std::vector<string> temp2_unused;

                                        if(Parametrization(S_temp2, &S_temp2_alt, &temp2_unused) == 1)//if we encountered a function declaration then
                                        {
                                            cout << "String number " << line << " contains NOT C lexeme." << endl;
                                            cout << S_temp << endl << S_temp_alt << endl;
                                            return 1;
                                        }

                                        if( S_temp2_alt.find("FUNCDEF(") != std::string::npos || S_temp2_alt.find("FUNCDEF (") != std::string::npos )
                                        {
                                            //j is current number of added lines beyond prev_size + 1. if it's 0 it means that we havent added any yet
                                            //we have to leave only 2 * j + 1 lines in previous
                                            break;//while cycle is exited
                                        }
                                        else
                                        {
                                            previous.push_back(S_temp2);
                                            if(S_temp2[S_temp2.size() - 1] == ';' || S_temp2[S_temp2.size() - 1] == '}' || S_temp2[S_temp2.size() - 1] == '{')
                                            {
                                                j++;//we added line to previous[]
                                            }
                                        }
                                    }
                                    else//here we'll check that file doesn't end with incomplete line
                                    {
                                        if(previous.size() > 0)
                                        {
                                            string last = previous[previous.size() - 1];//last string from previous
                                            //incomplete line can be found for example when defining a function in some of C coding styles
                                            if(last[last.size() - 1] != ';' && last[last.size() - 1] != '}' && in_file.eof())//if file ends with incomplete line it's an error
                                            {
                                                cout << "Unexpected end of file " << Paths[i] << endl;
                                                return 1;
                                            }
                                        }
                                    }
                                }

                                if(j < prev_size)//at this moment j shows how many lines we were able to add
                                {
                                    previous.erase(previous.begin(), previous.begin() + prev_size - j);
                                }

                                //AT THIS POINT I HAVE A FRAGMENT OF SIZE 2*FragmentSize + 1 or less THAT CONTAINS WEAKNESS
                                Exmplr.fragment = previous;

                                if(previous.size() > FragmentSize)
                                    previous.erase(previous.begin(), previous.begin() + previous.size() - FragmentSize);

                                //now we have fragment of code ready
                                if(Exmplr.fragment.size() != 0)//because empty weaknesses are useless
                                {
                                    size_t ix = 0;
                                    int Found_Equal = 0;
                                    while( ix < Need_to_Compare.size() && Found_Equal != 1 )
                                    {
                                        Found_Equal = Exemplars_Are_Equal( (*Clusters)[ Need_to_Compare[ix] ].commits[0].files[0].exemplars[0], Exmplr, path_to_fake_libc );
                                        if(Found_Equal != 1) ++ix;//so that at the end we will have either ix = clusters.size() => no equal exemplars were found
                                        //or ix value will be index of cluster, that contains equal exemplar
                                    }

                                    if(ix != Need_to_Compare.size())
                                        ix = Need_to_Compare[ix];
                                    //now ix contains not index of element of Need_to_Compare that contains index of needed cluster, but index of needed cluster

                                    if(Found_Equal == 1 && ix != Need_to_Compare.size() )//we have found equal exemplar
                                    {
                                        int Commit_Exists = 0;//checking if clusters->[ix] contains commit with SHA1 = (*Commit_Levels)[i].SHA1_of_commits[j]
                                        int index_of_last_commit = (*Clusters)[ix].commits.size();//contains index of last commit in (*Clusters)[ix]
                                        if(index_of_last_commit > 0)
                                            index_of_last_commit--;

                                        if( (*Clusters)[ix].commits[ index_of_last_commit ].SHA1.compare( (*Commit_Levels)[i].SHA1_of_commits[j] ) == 0 )
                                            Commit_Exists = 1;

                                        if(Commit_Exists == 0)//we have to add commit with SHA1 = (*Commit_Levels)[i].SHA1_of_commits[j]
                                        {
                                            Commit Cmmt;
                                            Cmmt.SHA1 = (*Commit_Levels)[i].SHA1_of_commits[j];
                                            (*Clusters)[ix].commits.push_back(Cmmt);
                                            index_of_last_commit++;
                                        }

                                        //now we have to check, if current Path is among FilePaths in FileDescriptions
                                        size_t jx = 0;
                                        int Found_Path = 0;//shows if there's already file description of current file in last commit description

                                        while( jx < (*Clusters)[ix].commits[ index_of_last_commit ].files.size() && Found_Path != 1)//stop if searched through all files[] or if found Path
                                        {
                                            if( (*Clusters)[ix].commits[ index_of_last_commit ].files[jx].FilePath.compare( Paths[k] ) == 0)//if there is a FilePath that is equal to current Path
                                                Found_Path = 1;

                                            if(Found_Path != 1) jx++;//so that at the end jx will be either size() => no equal paths were found
                                            //or it'll be index of file, that contains another weakness of current type
                                        }

                                        if(Found_Path == 1)
                                        {
                                            (*Clusters)[ix].commits[ index_of_last_commit ].files[jx].exemplars.push_back(Exmplr);//just adding exemplar to the file that already has similar weaknesses
                                        }
                                        else//no files that already contain weakness of current type were found
                                        {
                                            FileDescripton FlDscrptn;
                                            FlDscrptn.FilePath = Paths[k];
                                            FlDscrptn.FileState = "Clone";
                                            FlDscrptn.exemplars.push_back(Exmplr);
                                            (*Clusters)[ix].commits[ index_of_last_commit ].files.push_back(FlDscrptn);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(!in_file.eof() && Is_String_Not_Empty(S_temp) == 1)
                                {
                                    if(previous.size() < FragmentSize)
                                    {
                                        previous.push_back(S_temp);
                                    }
                                    else
                                        if (previous.size() != 0)
                                        {
                                            previous.erase(previous.begin());
                                            previous.push_back(S_temp);
                                        }
                                }
                            }
                        }
                    }
                    else//here we'll check that file doesn't end with incomplete line
                    {
                        if(previous.size() > 0)
                        {
                            string last = previous[previous.size() - 1];//last string from previous
                            //incomplete line can be found for example when defining a function in some of C coding styles
                            if(last[last.size() - 1] != ';' && last[last.size() - 1] != '}' && in_file.eof())//if file ends with incomplete line it's an error
                            {
                                cout << "Unexpected end of file " << Paths[i] << endl;
                                return 1;
                            }
                        }
                    }
                }

                in_file.close();
            }
        }
    }

    return 0;
}

#endif // HISTORY_ANALYSIS_H_INCLUDED
