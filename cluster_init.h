#ifndef CLUSTER_INIT_H_INCLUDED
#define CLUSTER_INIT_H_INCLUDED

#include "cluster.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>

using namespace boost::filesystem;
struct recursive_directory_range
{
    typedef recursive_directory_iterator iterator;
    recursive_directory_range(path p) : p_(p) {}

    iterator begin() { return recursive_directory_iterator(p_); }
    iterator end() { return recursive_directory_iterator(); }

    path p_;
};

int string_found_C_extension(std::string path)
{
    bool Found = (path.find(".h", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".H", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".hh", path.size() - 3) != std::string::npos);
    Found = Found || (path.find(".hpp", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".h++", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".hxx", path.size() - 4) != std::string::npos);

    Found = Found || (path.find(".c", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".C", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".cc", path.size() - 3) != std::string::npos);
    Found = Found || (path.find(".cpp", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".c++", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".cxx", path.size() - 4) != std::string::npos);

    Found = Found || (path.find(".i", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".I", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".ii", path.size() - 3) != std::string::npos);
    Found = Found || (path.find(".ipp", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".i++", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".inl", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".ixx", path.size() - 4) != std::string::npos);

    Found = Found || (path.find(".t", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".T", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".tt", path.size() - 3) != std::string::npos);
    Found = Found || (path.find(".tpp", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".t++", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".tpl", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".txx", path.size() - 4) != std::string::npos);
    return Found;
}

int list_dir_contents(std::vector<std::string>* Paths)
{
    for (directory_entry it : recursive_directory_range("."))
    {
        std::string path = it.path().string();
        if(path.find(".git") == std::string::npos && (string_found_C_extension(path)))
        {
            std::cout << path << std::endl;
            Paths->push_back(path);
        }
    }
  /*std::copy(
  boost::filesystem::recursive_directory_iterator("./"),
  boost::filesystem::recursive_directory_iterator(),
  std::ostream_iterator<boost::filesystem::directory_entry>(std::cout, "\n"));*/
return 0;
}

int initialize_clusters(vector<string>* Paths, vector<Cluster>* clusters, string SHA1, size_t FragmentSize, string WeaknessMarker)
{

    Commit Cmmt;
    Cmmt.SHA1 = SHA1;

    for(size_t i = 0; i < Paths->size(); ++i)
    {
        FILE* in_file;
        in_file = fopen((*Paths)[i].c_str(), "r");
        if(in_file == NULL) return 1;

        std::vector<std::string> previous;//contains strings before commentary in number of FragmentSize
        //and if needed commentary is found then contains previous.size() + more

        long long line = 1;

        while(!feof(in_file))
        {

            char str[256];
            fscanf(in_file, "%[^\n]\n", str);

            std::string S_temp(str);

            if(S_temp[0] == '/' && S_temp[1] == '/' && S_temp.find(WeaknessMarker.c_str()) != std::string::npos)
            {
                std::cout << (*Paths)[i] << " $$$ " << S_temp << std::endl;
                size_t j = 0, k = previous.size();
                while(j <= k)
                {
                    std::cout << j << std::endl;
                    if(!feof(in_file))
                    {
                        char str_t[256];
                        fscanf(in_file, "%[^\n]\n", str_t);

                        string S_temp_2(str_t);
                        previous.push_back(S_temp_2);
                    }
                    j++;
                }

                previous.clear();

            }
            else
            {
                if(previous.size() < FragmentSize)
                {
                    previous.push_back(S_temp);
                }
                else
                {
                    previous.erase(previous.begin());
                    previous.push_back(S_temp);
                }
            }

        ++line;

        }

        fclose(in_file);

    }

    return 0;
}
#endif // CLUSTER_INIT_H_INCLUDED
