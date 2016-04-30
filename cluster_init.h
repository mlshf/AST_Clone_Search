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
#include <cmath>
#include "git_exec.h"

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
    /*Found = Found || (path.find(".H", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".hh", path.size() - 3) != std::string::npos);*/
    Found = Found || (path.find(".hpp", path.size() - 4) != std::string::npos);
   /* Found = Found || (path.find(".h++", path.size() - 4) != std::string::npos);
    Found = Found || (path.find(".hxx", path.size() - 4) != std::string::npos);*/

    Found = Found || (path.find(".c", path.size() - 2) != std::string::npos);
   /* Found = Found || (path.find(".C", path.size() - 2) != std::string::npos);
    Found = Found || (path.find(".cc", path.size() - 3) != std::string::npos);*/
    Found = Found || (path.find(".cpp", path.size() - 4) != std::string::npos);
    /*Found = Found || (path.find(".c++", path.size() - 4) != std::string::npos);
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
    Found = Found || (path.find(".txx", path.size() - 4) != std::string::npos);*/

    return Found;
}

int list_dir_contents(std::vector<std::string>* Paths)
{
    std::cout << "LISTING FILES WITH ACCEPTABLE EXTENSIONS : " << std::endl;
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
    int beginning_flag = 1, whitespace_row = 0;//beginning_flag means that we are at the beginning of the string
    //it is needed for erasing all the whitespaces in the beginning, before the first !isspace() symbol
    //whitespace_row is a flag, that contains information whether we encountered a space and all the next space symbols may be erased
    //until we find !isspace() symbol
    size_t i = 0;//position in the string. it is increased by 1 only if Str[i] wasn't erased
    while(i < Str->size())
    {
        if(beginning_flag == 1)
        {
            if((*Str)[i] == ' ')
            {
                Str->erase(Str->begin());
                i = 0;//as we erase all the spaces at the beginning of the string we remain at position Str[0]
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
                    whitespace_row = 1;//if whitespace row was not == 1, then we only make its value equal 1, as we don't delete the first space in the row
                    i++;
                }
            }
            else
            {
                whitespace_row = 0;//if encountered symbol isn't space symbol, whitespace_row is set 0
                i++;
            }
        }
    }

    if((*Str)[Str->size() - 1] == ' ')//КОСТЫЛЬ, чтобы удалить последний пробел
        Str->erase(Str->begin() + Str->size() - 1);

    return;
}

int Exemplars_Are_Equal(Exemplar Original, Exemplar Compared)
{
    int offset = ((int)Original.fragment.size() - (int)Compared.fragment.size()) / 2;
    Exemplar small = Compared, big = Original;

    if(offset < 0)
    {
        small = Original;
        big = Compared;
    }

    int are_equal = 1;
    offset = abs(offset);
    size_t i = 0;
    for(; i < small.fragment.size() && are_equal == 1; ++i)
    {
        //std::cout << small.fragment[i] << " " << big.fragment[i + offset] << endl;
        if( small.fragment[i].compare(big.fragment[offset + i]) != 0 )
        {
            are_equal = 0;
        }
    }

    return are_equal;
}

int initialize_clusters(vector<string>* Paths, vector<Cluster>* clusters, string SHA1, size_t FragmentSize, string WeaknessMarker)
{
    for(size_t i = 0; i < Paths->size(); ++i)
    {/*
        FILE* in_file;
        in_file = fopen((*Paths)[i].c_str(), "r");
        if(in_file == NULL) return 1;*/
        ifstream in_file((*Paths)[i].c_str(), ios_base::in);
        if(!in_file.is_open())
        {
            std::cout << "FILE : " << (*Paths)[i] << " COULD NOT BE OPENED. ABORTING..." << std::endl;
            return 1;
        }

        std::vector<std::string> previous;//contains strings before commentary in number of FragmentSize
        //and if needed commentary is found then contains previous.size() + more

        long long line = 0;//stores current number of line that was read

        while(/*!feof(in_file)*/!in_file.eof())
        {/*
            char str[256];
            fscanf(in_file, "%[^\n]\n", str);

            std::string S_temp(str);*/

            std::string S_temp;
            std::getline(in_file, S_temp);
            line++;
            Delete_Extra_Spaces(&S_temp);

            if(!in_file.eof() && S_temp.size() > 2 && S_temp[0] == '/' && S_temp[1] == '/' && S_temp.find(WeaknessMarker.c_str()) != std::string::npos)
            {
                Exemplar Exmplr;
                Exmplr.line = line + 1;

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
                        line++;
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
                Exmplr.fragment = previous;
//debug printing
/*
                if(Exmplr.fragment.size() > 0)
                    std::cout << "DIRECTORY : "<< (*Paths)[i] << " ; MARKER : " << S_temp << " ; FRAGMENT : " << std::endl;

                for(size_t j = 0; j < Exmplr.fragment.size(); ++j)
                {
                    std::cout << Exmplr.fragment[j] << " " << Exmplr.line << std::endl;
                    if(j == Exmplr.fragment.size() - 1)
                        std::cout << std::endl;
                }
*/

                if(previous.size() > FragmentSize)
                    previous.erase(previous.begin(), previous.begin() + previous.size() - FragmentSize);

                if(Exmplr.fragment.size() != 0)//because empty weaknesses are useless
                {

                    if(clusters->size() == 0)//if clusters->size() == 0 then we are working with the first weakness in commit
                    {
                        Cluster Clstr;//new cluster
                        Commit Cmmt;//new commit
                        Cmmt.SHA1 = SHA1;
                        FileDescripton FlDscrptn;//new file description
                        FlDscrptn.FilePath = (*Paths)[i];//FilePath is current (*Paths)[i]
                        FlDscrptn.FileState = "start";//meaning that file is in the first commit
                        FlDscrptn.exemplars.push_back(Exmplr);//FileDescription has no exemplars of weakness
                        Cmmt.files.push_back(FlDscrptn);//commit has no FileDescriptions
                        Clstr.commits.push_back(Cmmt);//cluster has no commits
                        clusters->push_back(Clstr);//clusters has no elements
                    }
                    else//there are clusters already
                    {
                        size_t ix = 0;
                        int Found_Equal = 0;
                        while( ix < clusters->size() && Found_Equal != 1 )
                        {
                            Found_Equal = Exemplars_Are_Equal( (*clusters)[ix].commits[0].files[0].exemplars[0], Exmplr );
                            if(Found_Equal != 1) ++ix;//so that at the end we will have either ix = clusters.size() => no equal exemplars were found
                            //or ix value will be index of cluster, that contains equal exemplar
                        }

                        if(Found_Equal == 1)//we have found equal exemplar
                        {
                            size_t J = 0;//finding out if current commits is already in cluster's description
                            int Found_SHA1 = 0;
                            while( J < (*clusters)[ix].commits.size() && Found_SHA1 != 1 )
                            {
                                if( SHA1.compare( (*clusters)[ix].commits[J].SHA1 ) == 0 )
                                {
                                    Found_SHA1 = 1;
                                }
                                else
                                    ++J;
                            }

                            if( Found_SHA1 == 1 )//meaning there's already current commit's description in cluster
                            {
                                //now we have to check, if current Path is among FilePaths in FileDescriptions
                                size_t jx = 0;
                                int Found_Path = 0;
                                while( jx < (*clusters)[ix].commits[J].files.size() && Found_Path != 1)//stop if searched through all files[] or if found Path
                                {
                                    if( (*clusters)[ix].commits[J].files[jx].FilePath.compare((*Paths)[i]) == 0)//if there is a FilePath that is equal to current Path
                                        Found_Path = 1;

                                    if(Found_Path != 1) jx++;//so that at the end jx will be either size() => no equal paths were found
                                    //or it'll be index of file, that contains another weakness of current type
                                }

                                if(Found_Path == 1)
                                {
                                    (*clusters)[ix].commits[J].files[jx].exemplars.push_back(Exmplr);//just adding exemplar to the file that already has similar weaknesses
                                }
                                else//no files that already contain weakness of current type were found
                                {
                                    FileDescripton FlDscrptn;
                                    FlDscrptn.FilePath = (*Paths)[i];
                                    FlDscrptn.FileState = "start";
                                    FlDscrptn.exemplars.push_back(Exmplr);
                                    (*clusters)[ix].commits[J].files.push_back(FlDscrptn);
                                }
                            }
                            else
                            {
                                Commit Cmmt;//new commit
                                Cmmt.SHA1 = SHA1;
                                FileDescripton FlDscrptn;//new file description
                                FlDscrptn.FilePath = (*Paths)[i];//FilePath is current (*Paths)[i]
                                FlDscrptn.FileState = "start";//meaning that file is in the first commit
                                FlDscrptn.exemplars.push_back(Exmplr);//FileDescription has no exemplars of weakness
                                Cmmt.files.push_back(FlDscrptn);//commit has no FileDescriptions
                                (*clusters)[ix].commits.push_back(Cmmt);//cluster has no such commit yet
                            }
                        }
                        else//haven't found any existing cluster of the same type of weakness
                        {
                            Cluster Clstr;//new cluster
                            Commit Cmmt;//new commit
                            Cmmt.SHA1 = SHA1;
                            FileDescripton FlDscrptn;//new file description
                            FlDscrptn.FilePath = (*Paths)[i];//FilePath is current (*Paths)[i]
                            FlDscrptn.FileState = "start";//meaning that file is in the first commit
                            FlDscrptn.exemplars.push_back(Exmplr);//FileDescription has no exemplars of weakness
                            Cmmt.files.push_back(FlDscrptn);//commit has no FileDescriptions
                            Clstr.commits.push_back(Cmmt);//cluster has no commits
                            clusters->push_back(Clstr);//clusters has no element of current weakness type
                        }

                    }

                }
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
        }

        //fclose(in_file);
        in_file.close();

    }

    return 0;
}
#endif // CLUSTER_INIT_H_INCLUDED
