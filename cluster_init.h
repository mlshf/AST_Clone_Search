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
#include "lexical.h"

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
    bool Found = (path.find(".c", path.size() - 2) != std::string::npos);
    //Found = Found || (path.find(".H", path.size() - 2) != std::string::npos);
    //Found = Found || (path.find(".hh", path.size() - 3) != std::string::npos);
   // Found = Found || (path.find(".hpp", path.size() - 4) != std::string::npos);
    //Found = Found || (path.find(".h++", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".hxx", path.size() - 4) != std::string::npos);

    Found = Found || (path.find(".h", path.size() - 2) != std::string::npos);
    //Found = Found || (path.find(".C", path.size() - 2) != std::string::npos);
    //Found = Found || (path.find(".cc", path.size() - 3) != std::string::npos);
    //Found = Found || (path.find(".cpp", path.size() - 4) != std::string::npos);
    //Found = Found || (path.find(".c++", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".cxx", path.size() - 4) != std::string::npos);

   // Found = Found || (path.find(".i", path.size() - 2) != std::string::npos);
   // Found = Found || (path.find(".I", path.size() - 2) != std::string::npos);
   // Found = Found || (path.find(".ii", path.size() - 3) != std::string::npos);
   // Found = Found || (path.find(".ipp", path.size() - 4) != std::string::npos);
    //Found = Found || (path.find(".i++", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".inl", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".ixx", path.size() - 4) != std::string::npos);

   // Found = Found || (path.find(".t", path.size() - 2) != std::string::npos);
   // Found = Found || (path.find(".T", path.size() - 2) != std::string::npos);
   // Found = Found || (path.find(".tt", path.size() - 3) != std::string::npos);
   // Found = Found || (path.find(".tpp", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".t++", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".tpl", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".txx", path.size() - 4) != std::string::npos);

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

void Delete_Extra_Spaces(std::string* Str)//deletes space symbols except \n and comment
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
            if(isspace((*Str)[i]) && (*Str)[i] != '\n')
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
            if(isspace((*Str)[i]) && (*Str)[i] != '\n')
            {
                if(whitespace_row == 1)
                {
                    Str->erase(Str->begin() + i);
                }
                else//it's first whitespace in a row
                {
                    (*Str)[i] = ' ';
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

    if( isspace((*Str)[Str->size() - 1]) && (*Str)[Str->size() - 1] != '\n')//deleting last space symbol
        Str->erase(Str->begin() + Str->size() - 1);

//serious doubt! deleting commentaries at end of line
    /*if( Str->rfind(";//") != string::npos )
        Str->erase( Str->rfind(";//") + 1,  Str->size() - Str->rfind(";//") - 1);

    if( Str->rfind("; //") != string::npos )
        Str->erase( Str->rfind("; //") + 1,  Str->size() - Str->rfind("; //") - 1);

    if( Str->rfind(")//") != string::npos )
        Str->erase( Str->rfind(")//") + 1,  Str->size() - Str->rfind(")//") - 1);

    if( Str->rfind(") //") != string::npos )
        Str->erase( Str->rfind(") //") + 1,  Str->size() - Str->rfind(") //") - 1);

    if( Str->rfind("{//") != string::npos )
        Str->erase( Str->rfind("{//") + 1,  Str->size() - Str->rfind("{//") - 1);

    if( Str->rfind("{ //") != string::npos )
        Str->erase( Str->rfind("{ //") + 1,  Str->size() - Str->rfind("{ //") - 1);

    if( Str->rfind("}//") != string::npos )
        Str->erase( Str->rfind("}//") + 1,  Str->size() - Str->rfind("}//") - 1);

    if( Str->rfind("} //") != string::npos )
        Str->erase( Str->rfind("} //") + 1,  Str->size() - Str->rfind("} //") - 1);

    if( Str->size() >= 2 && (*Str)[0] != '/')
        if( Str->find("//") != string::npos )
            Str->erase( Str->rfind("//") + 1,  Str->size() - Str->rfind("//") - 1 );*/

    //deleting figure braces at the end and beginning of the string - they can be located at different lines, but fragment still can be clones
    /*if((*Str)[Str->size() - 1] == '}')
        Str->erase(Str->begin() + Str->size() - 1);

    if( Str->size() >= 2 && (*Str)[Str->size() - 2] == ')' && (*Str)[Str->size() - 1] == '{' )
        Str->erase(Str->begin() + Str->size() - 1);

    if( Str->size() >= 1 && (*Str)[0] == '{' )
        Str->erase(Str->begin() + Str->size() - 1);*/

    return;
}

int find_locations(vector<size_t>* locations, string source, string str_to_find, size_t step)
{
    if(source.find(str_to_find) != string::npos)
    {
        locations->push_back(source.find(str_to_find));
        while((*locations)[locations->size() - 1] + step < source.size())
        {
            size_t i = (*locations)[locations->size() - 1] + step;
            if(source.find(str_to_find, i) == string::npos)
            {
                break;
            }
            else
            {
                locations->push_back(source.find(str_to_find, i));
            }
        }
    }

    return 0;
}

int gen_id(vector<string> Text, vector<string>* Text_out, vector<vector<string>>* IDs, vector<vector<size_t>>* ID_locations)
{
    for(size_t i = 0; i < Text.size(); ++i)
    {
        string temp_str_out;
        vector<string> i_a_o;
        if( Parametrization(Text[i], &temp_str_out, &i_a_o) == 1 )
        {
            cout << "Was unable to parametrize " << Text[i] << endl;
            return 1;
        }
        Text_out->push_back(temp_str_out);
        IDs->push_back(i_a_o);

        vector<size_t> locations_id_for_line;
        find_locations(&locations_id_for_line, temp_str_out, "ID", 2);
        ID_locations->push_back(locations_id_for_line);
    }

    return 0;
}

vector<string> gen_typedefs(vector<vector<size_t>> ID_locations, vector<vector<string>> IDs)
{
    vector<string> typedefs;

    if(ID_locations.size() != IDs.size())
    {
        typedefs.push_back("FAIL");
        return typedefs;
    }

    for(size_t i = 0; i < ID_locations.size(); ++i)
    {
        vector<size_t> line_id_locks = ID_locations[i];
        for(size_t j = 0; j < line_id_locks.size(); ++j)
        {
            if(line_id_locks[j] == line_id_locks[j - 1] + 2)
                if( typedefs.end() == find( typedefs.begin(), typedefs.end(), (IDs[i])[j - 1] ) )
                    typedefs.push_back((IDs[i])[j - 1]);
        }
    }

    return typedefs;
}

int braces_balance(vector<string>* v_Str)
{
    int l_brace_counter = 0, r_brace_counter = 0;

    for(size_t i = 0; i < v_Str->size(); ++i)//each line
    {
        string line = (*v_Str)[i];

        if(line[line.size() - 1] == '{')
        {
            ++l_brace_counter;
        }
        if(line[line.size() - 1] == '}')
        {
            ++r_brace_counter;
        }
        else
        {
            if(line.size() >= 2 && line[line.size() - 1] == ';' && line[line.size() - 2] == '}')
                ++r_brace_counter;
        }
    }

    if(l_brace_counter > r_brace_counter)
    {
        string addition;
        for(int i = 0; i < l_brace_counter - r_brace_counter; ++i)
            addition += "};";
        v_Str->push_back(addition);
    }
    else
    {
        if(r_brace_counter > l_brace_counter)
        {
            string addition;
            for(int i = 0; i < r_brace_counter - l_brace_counter; ++i)
                addition += "{";
            v_Str->insert(v_Str->begin(), addition);
        }
        else
        {

        }
    }


    return 0;
}

//this function is needed because when initializing compared can be a part of original
int Exemplars_Are_Equal(Exemplar Original, Exemplar Compared, string path_to_fake_libc)// returns 1 if clones, 0 if not
{
    int offset  = ((int)Original.fragment.size() - (int)Compared.fragment.size()) / 2;
    Exemplar small = Compared, big = Original;

    if(offset < 0)//if original is included in compared
    {
        small = Original;
        big = Compared;
    }

    //making fragments have same size
    offset = abs(offset);
    size_t i = 0;
    std::vector<string> first, second;
    for(; i < small.fragment.size(); ++i)
    {
        first.push_back( small.fragment[i] );
        second.push_back( big.fragment[offset + i] );
    }

    //generating versions of first and second where identificators have been changed for ID in text_out_i, stored in ID_i and their locations stored in ID_locs_i
    std::vector<std::string> Text_out_1;
    std::vector<std::vector<std::string>> ID_1;
    std::vector<std::vector<size_t>> ID_locs_1;//ID_locations
    if(gen_id(first, &Text_out_1, &ID_1, &ID_locs_1) == 1)
    {
        cout << "Gen ID failed." << endl;
        return 1;
    }

    std::vector<std::string> Text_out_2;
    std::vector<std::vector<std::string>> ID_2;
    std::vector<std::vector<size_t>> ID_locs_2;//ID_locations
    if(gen_id(second, &Text_out_2, &ID_2, &ID_locs_2) == 1)
    {
        cout << "Gen ID failed." << endl;
        return 1;
    }

    //generating lists of IDs that have to be described in typedefs
    std::vector<string> typedefs1 = gen_typedefs(ID_locs_1, ID_1), typedefs2 = gen_typedefs(ID_locs_2, ID_2);

    //were unable to genereate typedefs
    if(typedefs1[0] == "FAIL" || typedefs2[0] == "FAIL")
    {
        cout << "Could not create additional pseudo-typedefs." << endl;
        return 0;
    }

    //make fragments have same number of left and right figure braces
    braces_balance(&first);
    braces_balance(&second);

    //make fragments look like this:
    //#include <stdio.h>
    //#include <stdlib.h>
    //typedef void* ...;
    //...
    //typedef void* ...;
    //void ast_i_func(){
    //FRAGMENT CODE
    //...
    //return;}
    //which is needed for fragment to be processed with PyCParser
    first.push_back("return;}");
    first.insert(first.begin(), "void ast1_func(){");
    for(size_t i = 0; i < typedefs1.size(); ++i)
    {
        first.insert(first.begin(), "typedef void* " + typedefs1[i] + ";");
    }
    first.insert(first.begin(), "#include <stdlib.h>");
    first.insert(first.begin(), "#include <stdio.h>");

    second.push_back("return;}");
    second.insert(second.begin(), "void ast2_func(){");
    for(size_t i = 0; i < typedefs2.size(); ++i)
    {
        second.insert(second.begin(), "typedef void* " + typedefs2[i] + ";");
    }
    second.insert(second.begin(), "#include <stdlib.h>");
    second.insert(second.begin(), "#include <stdio.h>");


    /*if(Perform_Comparison(&first, &second) == 1)//returns 1 if not clones, 0 if clones
        are_equal = 0;//so return value is 0 if not clones
*/

    std::ofstream outfile1 ("CPR4_GCC_PP_C99_AST_1.c", std::ofstream::out | std::ofstream::trunc);

    for(size_t i = 0; i < first.size(); ++i)
    {
        outfile1 << first[i] << std::endl;
    }

    std::ofstream outfile2 ("CPR4_GCC_PP_C99_AST_2.c", std::ofstream::out | std::ofstream::trunc);

    for(size_t i = 0; i < second.size(); ++i)
    {
        outfile2 << second[i] << std::endl;
    }

    outfile1.close();

    exec_git_command("gcc -E -I" + path_to_fake_libc + "fake_libc_include CPR4_GCC_PP_C99_AST_1.c -o out_CPR4_GCC_PP_C99_AST_1.c");
    exec_git_command("gcc -E -I" + path_to_fake_libc + "fake_libc_include CPR4_GCC_PP_C99_AST_2.c -o out_CPR4_GCC_PP_C99_AST_2.c");

    remove("CPR4_GCC_PP_C99_AST_1.c");
    remove("CPR4_GCC_PP_C99_AST_2.c");

    vector<string> result;
    exec_command("python3 " + path_to_fake_libc + "testing_file.py out_CPR4_GCC_PP_C99_AST_1.c out_CPR4_GCC_PP_C99_AST_2.c", &result);
    if(result.size() > 1)
    {
        result.erase( result.begin() + 1, result.end() );
    }

    remove("out_CPR4_GCC_PP_C99_AST_1.c");
    remove("out_CPR4_GCC_PP_C99_AST_2.c");

    if( result[0] == "Failed to parse file" )
        return 0;

    return std::stoi( result[0] );
}

//this function will find locations of defects in file PATH and store them in RESULT
int find_defects(string* path, vector<long long>* result)
{
    //for file described by PATH cppcheck is launched, it's result stored in CPR_cppcheck_output.txt
    exec_git_command("cppcheck --std=c99 " + *path + " 2> CPR4_C99_cppcheck_output.txt");

    ifstream cppcheck_out("CPR4_C99_cppcheck_output.txt", ios_base::in);//processing cppcheck result
    if(!cppcheck_out.is_open())
    {
        std::cout << "FILE : CPR4_C99_cppcheck_output.txt COULD NOT BE OPENED. ABORTING..." << std::endl;
        return 1;
    }

    //vector<string> CppCheck_result;

    while(!cppcheck_out.eof())
    {
        std::string S_temp;
        getline(cppcheck_out, S_temp);

        if( S_temp.size() > 0 && S_temp.find(":") != string::npos && S_temp.find("]") != string::npos )//if line looks like this [file.c:line] ...
        {
            size_t i_beg = S_temp.find(":"), i_end = S_temp.find("]");//we leave only line number
            S_temp = S_temp.substr(i_beg + 1, i_end - i_beg - 1);
            result->push_back(std::stoll(S_temp));//string is translated to long long
        }
    }

    cppcheck_out.close();

    remove("CPR4_C99_cppcheck_output.txt");

    return 0;
};

//for each file in version we find fragment located at position where CppCheck found and either creates new cluster or add to existing
int initialize_clusters(vector<string>* Paths, vector<Cluster>* clusters, string SHA1, size_t FragmentSize, string path_to_fake_libc)
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

        //create vector of defect locations and fill it
        vector<long long> defect_lines;
        find_defects(&((*Paths)[i]), &defect_lines);

        cout << "Defect lines in " << (*Paths)[i] << " : " << endl;
        for(size_t iii = 0; iii < defect_lines.size();++iii)
            cout << defect_lines[iii] << endl;
        cout << endl;

        std::vector<std::string> previous;//contains strings before commentary in number of FragmentSize
        //and if needed commentary is found then contains previous.size() + more

        long long line = 0;//stores current number of line that was read

        while(/*!feof(in_file)*/!in_file.eof() && defect_lines.size() > 0)
        {/*
            char str[256];
            fscanf(in_file, "%[^\n]\n", str);

            std::string S_temp(str);*/

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

                    /*if( S_temp.find("//") != string::npos )
                        cout << (*Paths)[i] << " : " << S_temp << " , " << S_temp[0] << endl;*/

                    //if current line is in defect lines we need to form a fragment
                    if(!in_file.eof() && defect_lines.end() != find(defect_lines.begin(), defect_lines.end(), line))
                    {
                        Exemplar Exmplr;
                        Exmplr.line = line;

                        int j = 0, prev_size = previous.size();//prev_size - size of previous[] before adding string with weakness
                        //j counts number of lines that have been added to previous[]
                        previous.push_back(S_temp);

                        while(j < prev_size && !in_file.eof())
                        {
                            /*
                                char str_t[256];
                                fscanf(in_file, "%[^\n]\n", str_t);

                                string S_temp_2(str_t);*/
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
                                            cout << "Unexpected end of file " << (*Paths)[i] << endl;
                                            return 1;
                                        }
                                    }
                                }
                            //j++;
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

                            if(clusters->size() == 0)//if clusters->size() == 0 then we are working with the first weakness in commit
                            {
                                Cluster Clstr;//new cluster
                                Commit Cmmt;//new commit
                                Cmmt.SHA1 = SHA1;
                                FileDescripton FlDscrptn;//new file description
                                FlDscrptn.FilePath = (*Paths)[i];//FilePath is current (*Paths)[i]
                                FlDscrptn.FileState = "Original";//meaning that file is in the first commit
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
                                    Found_Equal = Exemplars_Are_Equal( (*clusters)[ix].commits[0].files[0].exemplars[0], Exmplr, path_to_fake_libc );
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
                                            FlDscrptn.FileState = "Original";
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
                                        FlDscrptn.FileState = "Original";//meaning that file is in the first commit
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
                                    FlDscrptn.FileState = "Original";//meaning that file is in the first commit
                                    FlDscrptn.exemplars.push_back(Exmplr);//FileDescription has no exemplars of weakness
                                    Cmmt.files.push_back(FlDscrptn);//commit has no FileDescriptions
                                    Clstr.commits.push_back(Cmmt);//cluster has no commits
                                    clusters->push_back(Clstr);//clusters has no element of current weakness type
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
                        cout << "Unexpected end of file " << (*Paths)[i] << endl;
                        return 1;
                    }
                }
            }
        }

        //fclose(in_file);
        in_file.close();

    }

    return 0;
}
#endif // CLUSTER_INIT_H_INCLUDED
