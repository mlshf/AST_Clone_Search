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
#include "output.h"

using namespace std;

//reads init versions identificators from file
int Get_Start_SHA1(const char* filename, vector<string>* Start_SHA1)
{
    ifstream in_file(filename, ios_base::in);
    if(!in_file.is_open())
    {
        std::cout << "FILE : " << filename << " COULD NOT BE OPENED. ABORTING..." << std::endl;
        return 1;
    }

    while(!in_file.eof())
    {
        string S_temp;
        getline(in_file, S_temp);
        int it_is_SHA1 = 1;
        for(size_t i = 0; i < S_temp.size() && it_is_SHA1 == 1 && i < 40; ++i)//SHA_1 is 40 symbols long
        {
            if( (isdigit(S_temp[i]) || isalpha(S_temp[i])) == 0 )
                it_is_SHA1 = 0;
        }
        if(it_is_SHA1 == 1 && S_temp.size() == 40)
            Start_SHA1->push_back(S_temp);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    //181ed5489bfc64cc0f241f385f1d24f3241cb155 - example of SHA1 - used just for debugging
    if(argc == 4)//there must be strictly 4 command line arguments
    {
        stringstream S_FragmentSize(argv[2]);//turn FragmentSize char[] -> size_t
        size_t FragmentSize;
        S_FragmentSize >> FragmentSize;

        if(FragmentSize < 0)//fragment size should be >= 0
        {
            cout << "Second parameter (<SIZE>) should be an integer value greater than 0, or equal." << endl;
            cout << endl;
            exec_git_command("git checkout master");
            return 1;
        }

        cout <<"#GIT COMMANDS EMULATION..." << endl << endl;
        //string S1("git rev-list --min-parents=0 HEAD");

        vector<string> Start_SHA1;//reading init versions identificators (SHA-1) from file in argv[1]

        if(Get_Start_SHA1(argv[1], &Start_SHA1) == 1)
        {
            exec_git_command("git checkout master");
            return 1;
        }

        //print out SHA1 of all versions in argv[1]
        cout << "SHA1 hashes of commits used for initializing are:" << endl;
        for(size_t i = 0; i < Start_SHA1.size(); ++i)
            cout << Start_SHA1[i] << endl;
        cout << endl;

        //exit if no SHA1 found
        if(Start_SHA1.size() == 0)//should be at least one version in file - one elemtn in vector
        {
            cout << "No hashes in " << argv[1] << "Aborting..." << endl;
            return 1;
        }

        //first element of vector is initial versions from which we'll start
        string S_SHA1(Start_SHA1[0]);
        cout << "Start commit's SHA1 hash is: " << S_SHA1 << endl;

        //vector that contains versions to be processed is created and filled
        vector<Commit_Level> Commit_Levels;
        Commit_Level Level0;
        Level0.level = 0;
        Level0.SHA1_of_commits.push_back(S_SHA1);
        Commit_Levels.push_back(Level0);

        if(Fill_Commit_Levels(&Commit_Levels, &Start_SHA1) == 1)
        {
            exec_git_command("git checkout master");
            return 1;
        }

        //
        for(size_t i = 0; i < Commit_Levels.size(); i++)
        {
            if( i < 10 )//выравнивание т.к. появляются двузначные числа
                cout << "COMMIT LEVEL :  " << Commit_Levels[i].level << " : ";
            else
                cout << "COMMIT LEVEL : " << Commit_Levels[i].level << " : ";

            for(size_t j = 0; j < Commit_Levels[i].SHA1_of_commits.size(); ++j)
                cout << Commit_Levels[i].SHA1_of_commits[j] << "   ";

            cout << endl;
        }
        cout << endl;

        //vector of Clusters of clones of code is created
        vector<Cluster> Clusters;
        //and initializaed
        for(size_t i = 0; i < Start_SHA1.size(); ++i)
        {

            string git_command("git checkout ");//so that we use starting commit to initialize clusters
            git_command += Start_SHA1[i];
            if(exec_git_command(git_command) == 1)
                return 1;
            //this vector contains paths to all .c files in current version
            vector<string> Vector_of_Paths;
            list_dir_contents(&Vector_of_Paths);

            if(initialize_clusters(&Vector_of_Paths, &Clusters, Start_SHA1[i], FragmentSize) == 1)
            {
                exec_git_command("git checkout master");
                return 1;
            }

            Vector_of_Paths.clear();//we no longer need the resources
        }

        //this time we have vector of Clusters and we add new elements if needed
        if(Analyze_History(&Commit_Levels, &Clusters, FragmentSize) == 1)
        {
            exec_git_command("git checkout master");
            return 1;
        }

        //producing output
        string BaseName(argv[3]);
        if( Output_Of_Result(&Clusters, BaseName) == 1 )//if we could not create all output files then we dump clusters in the console
        {
            cout << "Something went wrong with creating result with GraphViz..." << endl;
            cout << "RESULT: " << endl << endl;
            for(size_t i = 0; i < Clusters.size(); ++i)
            {
                cout << "CLUSTER #" << i << " :" << endl << "-----------------------------------------------------------" << endl;
                for(size_t j = 0; j < Clusters[i].commits.size(); ++j)
                {
                    cout << "COMMIT #" << j << " ; " << "SHA1 : " << Clusters[i].commits[j].SHA1 << " :" << endl;
                    for(size_t k = 0; k < Clusters[i].commits[j].files.size(); ++k)
                    {
                        cout << "FILE : " << Clusters[i].commits[j].files[k].FilePath << " ; STATE = " << Clusters[i].commits[j].files[k].FileState;
                        cout <<" ; NUMBER OF EXEMPLARS = " << Clusters[i].commits[j].files[k].exemplars.size() << " ; LINES & SIZES : ";
                        for(size_t n = 0; n < Clusters[i].commits[j].files[k].exemplars.size(); ++n)
                        {
                            cout << Clusters[i].commits[j].files[k].exemplars[n].line << " " << Clusters[i].commits[j].files[k].exemplars[n].fragment.size();
                            if(n != Clusters[i].commits[j].files[k].exemplars.size() - 1) cout << " , ";
                        }
                        cout << endl;

                    }
                    cout << endl;
                }
                cout << "=============================================================================" << endl;
            }

            return 1;
        }

        cout <<"#PROCESS IS OVER..." << endl;

    }
    else
    {
        cout << "Exactly four parameters are needed - file with SHA1 hashes of starting commits, fragment size and path to output file that describes cluster without extension." << endl;
        cout << "For example: ../Course_Realization/bin/Debug/Course_Realization '../sha1.txt' 3 '../output'" << endl;;
    }

    cout << endl;
    exec_git_command("git checkout master");

    return 0;
}
