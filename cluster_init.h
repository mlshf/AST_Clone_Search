#ifndef CLUSTER_INIT_H_INCLUDED
#define CLUSTER_INIT_H_INCLUDED

#include "cluster.h"
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
    std::cout << std::endl;
  /*std::copy(
  boost::filesystem::recursive_directory_iterator("./"),
  boost::filesystem::recursive_directory_iterator(),
  std::ostream_iterator<boost::filesystem::directory_entry>(std::cout, "\n"));*/
return 0;
}

int Is_String_Not_Empty(std::string S_temp)//1 - does contain non-space characters, 0 otherwise
{
    int String_Not_Empty = 0;
    for(size_t t = 0; t < S_temp.size() && String_Not_Empty != 1; ++t)
    {
        char c = S_temp[t];
        if(isspace(c) == 0 && c != EOF)
            String_Not_Empty = 1;
    }
    return String_Not_Empty;
}

void Delete_Extra_Spaces(std::string* Str)
{
    int beginning_flag = 1, whitespace_row = 0;
    size_t i = 0;
    while(i < Str->size())
    {
        if(beginning_flag == 1)
        {
            if((*Str)[i] == ' ')
            {
                Str->erase(Str->begin());
                i = 0;
            }
            else
            {
                beginning_flag = 0;
                i++;
            }
        }
        else
        {
            if((*Str)[i] == ' ')
            {
                if(whitespace_row == 1)
                {
                    Str->erase(Str->begin() + i);
                }
                else//it's first whitespace in a row
                {
                    whitespace_row = 1;
                    i++;
                }
            }
            else
            {
                whitespace_row = 0;
                i++;
            }
        }
    }
    if((*Str)[Str->size() - 1] == ' ')
        Str->erase(Str->begin() + Str->size() - 1);
}

int initialize_clusters(vector<string>* Paths, vector<Cluster>* clusters, string SHA1, size_t FragmentSize, string WeaknessMarker)
{

    Commit Cmmt;
    Cmmt.SHA1 = SHA1;

    for(size_t i = 0; i < Paths->size(); ++i)
    {/*
        FILE* in_file;
        in_file = fopen((*Paths)[i].c_str(), "r");
        if(in_file == NULL) return 1;*/
        ifstream in_file((*Paths)[i].c_str(), ios_base::in);
        if(!in_file.is_open()) return 1;

        std::vector<std::string> previous;//contains strings before commentary in number of FragmentSize
        //and if needed commentary is found then contains previous.size() + more

        long long line = 1;

        while(/*!feof(in_file)*/!in_file.eof())
        {/*
            char str[256];
            fscanf(in_file, "%[^\n]\n", str);

            std::string S_temp(str);*/

            std::string S_temp;
            std::getline(in_file, S_temp);
            Delete_Extra_Spaces(&S_temp);

            if(!in_file.eof() && S_temp.size() > 2 && S_temp[0] == '/' && S_temp[1] == '/' && S_temp.find(WeaknessMarker.c_str()) != std::string::npos)
            {
                std::cout << (*Paths)[i] << " $$$ " << S_temp << std::endl;
                int j = 0, prev_size = previous.size()/*, k = 0*/;//prev_size - size of previous[] before adding string with weakness
                //k counts lines that cone AFTER line with weakness that are added to the previous[]
                while(j <= prev_size && !in_file.eof())
                {
                    /*
                        char str_t[256];
                        fscanf(in_file, "%[^\n]\n", str_t);

                        string S_temp_2(str_t);*/
                        std::string S_temp_2;
                        std::getline(in_file, S_temp_2);
                        Delete_Extra_Spaces(&S_temp_2);

                        if(!in_file.eof() && Is_String_Not_Empty(S_temp_2) == 1 && S_temp_2[0] != '/' && S_temp_2[1] != '/')
                        {
                            previous.push_back(S_temp_2);
                            //if(j != 0)//because k counts lines after line with weakness
                                //k++;
                            j++;
                        }
                    //j++;
                }

                if(j > 0)
                    j--;//1because j shows the number of line that would have been read if cycle hadn't stopped
                if(j < prev_size)
                {
                    previous.erase(previous.begin(), previous.begin() + prev_size - j);
                }

                //AT THIS POINT I HAVE A FRAGMENT OF SIZE 2*FragmentSize + 1 or less THAT CONTAINS WEAKNESS
                for(size_t j = 0; j < previous.size(); ++j)
                    std::cout << previous[j] << " " << previous.size() << std::endl;
                std::cout << std::endl;

                if(previous.size() > FragmentSize)
                    previous.erase(previous.begin(), previous.begin() + previous.size() - FragmentSize);

            }
            else
            if(!in_file.eof() && Is_String_Not_Empty(S_temp) == 1)
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

        //fclose(in_file);
        in_file.close();

        previous.clear();

    }

    return 0;
}
#endif // CLUSTER_INIT_H_INCLUDED
