#include <iostream>
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

using namespace std;

int main(int argc, char* argv[])
{
//181ed5489bfc64cc0f241f385f1d24f3241cb155
if(argc == 2 && strlen(argv[1]) == 40)
{
    size_t FragmentSize = 3;
    string WeaknessMarker("#_Weakness_Threat_#");
    cout <<"#GIT COMMANDS EMULATION..." << endl << endl;
    //string S1("git rev-list --min-parents=0 HEAD");

    string S_SHA1(argv[1]);
/*
    vector<string> Vector_SHA1;
    cout << Vector_SHA1.size() << endl;

    exec_git_getsha1(S_SHA1, &Vector_SHA1);
    cout << Vector_SHA1.size() << endl;
*/
    vector<Commit_Level> Commit_Levels;
    Commit_Level Level0;
    Level0.level = 0;
    Level0.SHA1_of_commits.push_back(S_SHA1);
    Commit_Levels.push_back(Level0);
    if(Fill_Commit_Levels(&Commit_Levels) == 1)
    {
        return 1;
    }

    for(size_t i = 0; i < Commit_Levels.size(); i++)
    {
        if( i < 10 ) cout << "COMMIT LEVEL :  " << Commit_Levels[i].level << " : ";
        else cout << "COMMIT LEVEL : " << Commit_Levels[i].level << " : ";
        for(size_t j = 0; j < Commit_Levels[i].SHA1_of_commits.size(); ++j)
            cout << Commit_Levels[i].SHA1_of_commits[j] << "   ";
        cout << endl;
    }
    cout << endl;

    vector<string> Vector_of_Paths;
    list_dir_contents(&Vector_of_Paths);

    vector<Cluster> Clusters;
    if(initialize_clusters(&Vector_of_Paths, &Clusters, S_SHA1, FragmentSize, WeaknessMarker) == 1)
    {
        return 1;
    }

    cout << "CLUSTERS ENUMERATION: " << endl;
    for(size_t i = 0; i < Clusters.size(); ++i)
    {
        cout << "Cluster #" << i << endl;
        for(size_t j = 0; j < Clusters[i].commits[0].files.size(); ++j)
        {
            for(size_t k = 0; k < Clusters[i].commits[0].files[j].exemplars.size(); ++k)
            {
                cout << "LINE : " << Clusters[i].commits[0].files[j].exemplars[k].line << " ; IN FILE : " << Clusters[i].commits[0].files[j].FilePath << endl;
            }
        }
        cout << endl;
    }

    cout <<"#PROCESS IS OVER..." << endl;

}
else
    cout << "EXACTLY ONE PARAMETER IS NEEDED - SHA1 OF STARTING COMMIT!" << endl;

    return 0;
}
