#ifndef OUTPUT_H_INCLUDED
#define OUTPUT_H_INCLUDED

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>
#include "git_exec.h"//exec_git_command & exec_git_getsha1
#include "cluster_init.h"//initializes clusters
#include "cluster.h"
#include "history_analysis.h"
#include <cctype>

int Output_Of_Result(vector<Cluster>* Clusters, string BaseName, char showflag, ofstream& logfile)
{
    long long counter = 0;
    for(size_t i = 0; i < Clusters->size(); ++i)
    {
        stringstream converter;
        converter << i + 1;

        string Name = BaseName + "_" + converter.str() + ".gv";
        ofstream output_file(Name.c_str(), ios_base::trunc);
        if(!output_file.is_open())
        {
            if(showflag == 1)
                logfile << "Could not create/open file " << Name << endl;
            return 1;
        }

        //cout << "1_" << i << endl;

        output_file << "digraph Cluster" << endl << "{" << endl;

            output_file << "subgraph Commits" << endl << "{" << endl;

                output_file << "node [shape=\"record\", fontsize=\"20\"];" << endl << "label = \"Commits\";" << endl;

                for(size_t j = 0; j < (*Clusters)[i].commits.size(); ++j)
                {
                    converter.clear();
                    converter.str(string());
                    converter << j + 1;

                    string Commit_j = "\"Commit " + converter.str() + "\" [label = \"{Commit ";
                    converter.clear();
                    converter.str(string());
                    converter << j + 1;

                    Commit_j += converter.str() + " | SHA1 : \\n ";
                    Commit_j += (*Clusters)[i].commits[j].SHA1;

                    //cout << "OK ?" << i << endl;

                    if(j == 0)
                    {
                        Commit_j += " | Original : \\n ";
                        /*for(size_t k = 0; k < (*Clusters)[i].commits[0].files[0].exemplars[0].fragment.size(); ++k)
                        {
                            Commit_j += (*Clusters)[i].commits[0].files[0].exemplars[0].fragment[k];
                            if(k < (*Clusters)[i].commits[0].files[0].exemplars[0].fragment.size() - 1)
                                Commit_j += " \\n ";
                        }*/
                        Commit_j += "File : ";
                        converter.clear();
                        converter.str(string());
                        converter << (*Clusters)[i].commits[j].files[0].FilePath;

                        Commit_j += converter.str() += " \\n Line : ";
                        converter.clear();
                        converter.str(string());
                        converter << (*Clusters)[i].commits[j].files[0].exemplars[0].line;

                        Commit_j += converter.str() + " \\n Size : ";
                        converter.clear();
                        converter.str(string());
                        converter << (*Clusters)[i].commits[j].files[0].exemplars[0].fragment.size();

                        Commit_j += converter.str();

                    }

                    //cout << "OK !" << i << endl;

                    Commit_j += "}\"];";

                    output_file << Commit_j << endl;

                }

            output_file << "}" << endl;

            for(size_t j = 0; j < (*Clusters)[i].commits.size(); ++j)
            {
                for(size_t k = 0; k < (*Clusters)[i].commits[j].files.size(); ++k)
                {
                    converter.clear();
                    converter.str(string());
                    converter << j + 1;

                    string File_k = "\"Commit " + converter.str() + " File ";
                    converter.clear();
                    converter.str(string());
                    converter << k + 1;

                    File_k += converter.str() + "\" [shape = \"record\", label = \"{" + (*Clusters)[i].commits[j].files[k].FilePath;
                    File_k += " | " + (*Clusters)[i].commits[j].files[k].FileState;

                    for(size_t n = 0; n < (*Clusters)[i].commits[j].files[k].exemplars.size(); ++n)
                    {
                        File_k += " | Line : ";
                        converter.clear();
                        converter.str(string());
                        converter << (*Clusters)[i].commits[j].files[k].exemplars[n].line;

                        File_k += converter.str() + " \\n Size : ";
                        converter.clear();
                        converter.str(string());
                        converter << (*Clusters)[i].commits[j].files[k].exemplars[n].fragment.size();

                        File_k += converter.str();

                        ++counter;
                    }

                    File_k += "}\"];";
                    output_file << File_k << endl;

                }
            }

            output_file << endl;

            for(size_t j = 0; j < (*Clusters)[i].commits.size(); ++j)
            {
                converter.clear();
                converter.str(string());
                converter << j + 1;

                string Structure = "\"Commit " + converter.str() + "\"";
                for(size_t k = 0; k < (*Clusters)[i].commits[j].files.size(); ++k)
                {
                    converter.clear();
                    converter.str(string());
                    converter << j + 1;

                    Structure += " -> \"Commit " + converter.str() + " File ";
                    converter.clear();
                    converter.str(string());
                    converter << k + 1;

                    Structure += converter.str() + "\"";
                }

                Structure += ";";
                output_file << Structure << endl;
            }

        output_file << "}" << endl;

        output_file.close();

        /*string Command = "dot -Tpng ";//example1.txt -o example1.png
        converter.clear();
        converter.str(string());
        converter << i + 1;

        Command += BaseName + "_" + converter.str() + ".gv -o " + BaseName + "_" + converter.str() + ".png";
        if(exec_git_command(Command, showflag, logfile) == 1)
            return 1;*/

    }

    cout << endl << counter << endl << endl;

    return 0;
}


#endif // OUTPUT_H_INCLUDED
