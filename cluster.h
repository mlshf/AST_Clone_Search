#ifndef CLUSTER_H_INCLUDED
#define CLUSTER_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>

using namespace std;

//exemplar of weakness in a file
struct Exemplar {
    long long line;//line of file that contains weakness
    vector<string> fragment;//fragment that contains weakness
};

//a set of weaknesses in one file described by FilePath
struct Weakness {
    string FilePath;//path to described file
    string FileState;//deleted, changed, unchanged, ... ?
    vector<Exemplar> exemplars;//vector of exemplars of weaknesses in this one file
};

//a set of files that contain weaknesses in one commit described by its SHA1
struct Commit {
    string SHA1;//current commit's SHA1
    vector<Weakness> weaknesses;//vector of files' descriptions
};

//a set of structures representing changes per one commit for a specific type of weaknesses
struct Cluster {
    //commits[0] - starting commit - contains info about first appearance of type of weakness,
    //change history of which is described by this Cluster
    //commits[0].weaknesses[0] - original weakness from staring commit
    vector<Commit> commits;
};

#endif // CLUSTER_H_INCLUDED
