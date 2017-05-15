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

//its used to find all files in current directoriy and all its subdirectories
using namespace boost::filesystem;
struct recursive_directory_range
{
    typedef recursive_directory_iterator iterator;
    recursive_directory_range(path p) : p_(p) {}

    iterator begin() { return recursive_directory_iterator(p_); }
    iterator end() { return recursive_directory_iterator(); }

    path p_;
};

//returns True if file is of .c or .h extentsion, False otherwise
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

//creates list of all files with .c or .h extensions in current directory and all its subdirectories
int list_dir_contents(std::vector<std::string>* Paths, char showflag, ofstream& logfile)
{
    if(showflag == 1)
        logfile << "LISTING FILES WITH ACCEPTABLE EXTENSIONS : " << std::endl;
    for (directory_entry it : recursive_directory_range("."))
    {
        std::string path = it.path().string();
        if(path.find(".git") == std::string::npos && (string_found_C_extension(path)))
        {
            if(showflag == 1)
                logfile << path << std::endl;
            Paths->push_back(path);
        }
    }

    if(showflag == 1)
        logfile << std::endl;

    return 0;
}

//1 - does contain non-space characters, 0 otherwise
int Is_String_Not_Empty(std::string S_temp)
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

//deletes space symbols in the beginning, at the end and inside the String
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

    for( size_t i = Str->size() - 1; i >= 0 && isspace( (*Str)[i] ); --i)
        Str->erase( Str->begin() + i );

    return;
}

//this function generates vector of all locations of Str_to_find in Source distance between which is Step or greater
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

//this function generates vectors of IDs in lines of Text_out and vector of their corresponding locations in lines of Text_out
int gen_id(vector<string> Text, vector<string>* Text_out, vector<vector<string>>* IDs, vector<vector<size_t>>* ID_locations, char showflag, ofstream& logfile)
{
    for(size_t i = 0; i < Text.size(); ++i)
    {
        string temp_str_out;
        vector<string> i_a_o;
        if( Parametrization(Text[i], &temp_str_out, &i_a_o, showflag, logfile ) == 1 )
        {
            if(showflag == 1)
                logfile << "Was unable to parametrize " << Text[i] << endl;
            continue;
        }
        Text_out->push_back(temp_str_out);
        IDs->push_back(i_a_o);

        vector<size_t> locations_id_for_line;
        find_locations(&locations_id_for_line, temp_str_out, "ID", 2);
        ID_locations->push_back(locations_id_for_line);
    }

    return 0;
}

//this function generates vector of pseudo-typedefs for Text according to vectors of ID locations and IDs themselves
//pseudo-typedef is added if variable declaration uses obvious user defined types
//pseudo-typedef is added if user defined type is used in type cast ~ (Type)...
vector<string> gen_typedefs(vector<vector<size_t>> ID_locations, vector<vector<string>> IDs, vector<string>* Text_param)
{
    vector<string> typedefs;

    if(ID_locations.size() != IDs.size())
    {
        typedefs.push_back("FAIL");
        return typedefs;
    }
    //typedef in variable declaration ID[*...*]ID
    for(size_t i = 0; i < ID_locations.size(); ++i)
    {
        vector<size_t> line_id_locks = ID_locations[i];

        for(size_t j = 1; j < line_id_locks.size(); ++j)
        {
            //if IDID or ID*...*ID is found
            size_t p = line_id_locks[j - 1] + 2;
            if( p == 2)//meaning we're right in the beginning of the line
            {
                while( ((*Text_param)[i])[ p ] == '*' )
                {
                    ++p;
                }
            }

            if(line_id_locks[j] == p)
            {
                if( (IDs[i]).size() > 0 && j > 0 && typedefs.end() == find( typedefs.begin(), typedefs.end(), (IDs[i])[j - 1] ) )
                {
                    typedefs.push_back((IDs[i])[j - 1]);
                }
            }
        }

        //typedef in type cast ~ (Type)...
        string PFPF("CBP4_PARAMETERIZED_FUNCNAME_POSSIBLE_FUNCDEF");

        for(size_t j = 0; j < line_id_locks.size(); ++j)
        {

            if( (int)line_id_locks[j] - 1 <= 0 )
                continue;

            string ID_LOC = (*Text_param)[i].substr( line_id_locks[j] - 1, 4 );

            //if IDs[i] size > 0
            //and if ID_LOC = (ID)
            //and ( if there is PFPF before ID_LOC and it's further than PFPF size
            //or if there is no PFPF before ID_LOC )
            //and if there is no such type in typedef yet
            if( (IDs[i]).size() > 0
            && ID_LOC == "(ID)"
            && ( ( (*Text_param)[i].rfind( PFPF, line_id_locks[j] - 2 ) != string::npos
                && (long long)(*Text_param)[i].rfind( PFPF, line_id_locks[j] - 2 ) < (long long)(line_id_locks[j] - 1 - PFPF.size() ) )
            || (*Text_param)[i].rfind( PFPF, line_id_locks[j] - 2 ) == string::npos )
            && typedefs.end() == find( typedefs.begin(), typedefs.end(), (IDs[i])[j] ) )
            {
                typedefs.push_back((IDs[i])[j]);
            }
        }
    }

    return typedefs;
}

//this function restores braces balance which is needed for preprocessing before comparison
int braces_balance(vector<string>* v_Str)
{
    int l_brace_counter = 0, r_brace_counter = 0;
    size_t r_brace_beg_counter = 0;

    for(size_t i = 0; i < v_Str->size(); ++i)//each line
    {
        string line = (*v_Str)[i];

        size_t j = line.size() - 1;

        char endflag = 0;
        while(j >= 0 && endflag != 1)
        {
            if( line[j] == '}' )
            {
                if (l_brace_counter == 0)
                {
                    ++r_brace_beg_counter;
                }
                else
                {
                    ++r_brace_counter;
                }
                --j;
            }
            else
            {
                if( j >= 1 && line[j] == ';' && line[j - 1] == '}' )
                {
                    if (l_brace_counter == 0)
                    {
                        ++r_brace_beg_counter;
                    }
                    else
                    {
                        ++r_brace_counter;
                    }
                    j -= 2;
                }
                else
                {
                    endflag = 1;
                }
            }
        }

        j = 0;
        while( j < line.size() && line[j] == '{' )
        {
            ++l_brace_counter;
            ++j;
        }
    }

    string add_r_brace_beg;
    for(size_t q = 0; q < r_brace_beg_counter; ++q)
    {
        add_r_brace_beg += "{";
    }
    v_Str->insert(v_Str->begin(), add_r_brace_beg);


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

//writes Text to file with path filename using ofstream
int write_to_file(string filename, vector<string> text)
{
    ofstream outfile;
    outfile.open( filename.c_str() , ofstream::out | ofstream::trunc);

    for(size_t i = 0; i < text.size(); ++i)
    {
        outfile << text[i] << endl;
    }

    outfile.close();

    if(outfile.is_open())
        outfile.close();

    return 0;

}

//this function prepares fragments for comparison by Python AST comparison module
// returns 1 if clones, 0 if not
int Exemplars_Are_Equal(Exemplar Original, Exemplar Compared, string path_to_fake_libc, char showflag, ofstream& logfile)
{
    //if original is bigger than compared and compared size is 1 than there's no comparison and 0 is returned
    if( Original.fragment.size() != 1 && Compared.fragment.size() == 1 )
    {
        return 0;
    }

    //cuts bigger fragment so that fragments of equal size are compared
    Exemplar big = Original, small = Compared;

    int offset  = ((int)big.fragment.size() - (int)small.fragment.size()) / 2;

    if(offset < 0)
    {
        offset = abs(offset);
        big = Compared;
        small = Original;
    }

    size_t i = 0;
    std::vector<string> first, second;
    for(; i < small.fragment.size() && offset + i < big.fragment.size() ; ++i)
    {
        first.push_back( small.fragment[i] );
        second.push_back( big.fragment[offset + i] );
    }

    i = 0;
    for(; i < first.size(); ++i)
    {
        if(first[i] != second[i])
            break;
    }
    //it means that two fragments are absolutely equal (as plain text) and no further comparison is needed
    if(i > 0 && i == first.size())
    {
        return 1;

    }

    //generating versions of first and second where identificators have been changed for ID in text_out_i, stored in ID_i and their locations stored in ID_locs_i
    std::vector<std::string> Text_out_1;
    std::vector<std::vector<std::string>> ID_1;
    std::vector<std::vector<size_t>> ID_locs_1;//ID_locations
    if(gen_id(first, &Text_out_1, &ID_1, &ID_locs_1, showflag, logfile) == 1)
    {
        if(showflag == 1)
            logfile << "Gen ID failed." << endl;
        return 0;
    }

    std::vector<std::string> Text_out_2;
    std::vector<std::vector<std::string>> ID_2;
    std::vector<std::vector<size_t>> ID_locs_2;//ID_locations
    if(gen_id(second, &Text_out_2, &ID_2, &ID_locs_2, showflag, logfile) == 1)
    {
        if(showflag == 1)
            logfile << "Gen ID failed." << endl;
        return 0;
    }

    //generating lists of IDs that have to be described in typedefs
    std::vector<string> typedefs1 = gen_typedefs(ID_locs_1, ID_1, &Text_out_1), typedefs2 = gen_typedefs(ID_locs_2, ID_2, &Text_out_2);

    //were unable to genereate typedefs
    if( ( typedefs1.size() > 0 && typedefs1[0] == "FAIL" ) || ( typedefs2.size() > 0 && typedefs2[0] == "FAIL" ) )
    {
        if(showflag == 1)
            logfile << "Could not create additional pseudo-typedefs." << endl;
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
    first.push_back("}");
    first.insert(first.begin(), "void ast1_func(){");
    for(size_t j = 0; j < typedefs1.size(); ++j)
    {
        first.insert(first.begin(), "typedef void* " + typedefs1[j] + ";");
    }
    first.insert(first.begin(), "#include <stdlib.h>");
    first.insert(first.begin(), "#include <stdio.h>");

    second.push_back("}");
    second.insert(second.begin(), "void ast2_func(){");
    for(size_t j = 0; j < typedefs2.size(); ++j)
    {
        second.insert(second.begin(), "typedef void* " + typedefs2[j] + ";");
    }
    second.insert(second.begin(), "#include <stdlib.h>");
    second.insert(second.begin(), "#include <stdio.h>");

    //now preprocession and Python comparison

    string filename = "CPR4_GCC_PP_C99_AST_1.c";

    write_to_file(filename, first);

    filename = "CPR4_GCC_PP_C99_AST_2.c";

    write_to_file(filename, second);

    exec_git_command("gcc -w -E -I" + path_to_fake_libc + "fake_libc_include CPR4_GCC_PP_C99_AST_1.c -o out_CPR4_GCC_PP_C99_AST_1.c", showflag, logfile);
    exec_git_command("gcc -w -E -I" + path_to_fake_libc + "fake_libc_include CPR4_GCC_PP_C99_AST_2.c -o out_CPR4_GCC_PP_C99_AST_2.c", showflag, logfile);

    filename = "CPR4_GCC_PP_C99_AST_1.c";
    remove( filename.c_str() );
    filename = "CPR4_GCC_PP_C99_AST_2.c";
    remove( filename.c_str() );

    vector<string> result;
    exec_command("python3 " + path_to_fake_libc + "compare_as_ast.py out_CPR4_GCC_PP_C99_AST_1.c out_CPR4_GCC_PP_C99_AST_2.c", &result, showflag, logfile);

    filename = "out_CPR4_GCC_PP_C99_AST_1.c";
    remove( filename.c_str() );
    filename = "out_CPR4_GCC_PP_C99_AST_2.c";
    remove( filename.c_str() );

    if(result.size() > 1)
    {
        result.erase( result.begin() + 1, result.end() );
    }

    if(showflag == 1 && result.size() > 0 )
        logfile << result[0] << endl;

    if( result.size() < 1 || result[0].size() < 1 || result[0][0] != '1')
        return 0;

    return 1;
}

//this function will find locations of defects in file PATH and store them in RESULT
int find_defects(string* path, vector<long long>* result, char showflag, ofstream& logfile)
{
    //for file described by PATH cppcheck is launched, it's result stored in CPR_cppcheck_output.txt
    exec_git_command("cppcheck --std=c99 " + *path + " 2> CPR4_C99_cppcheck_output.txt", showflag, logfile);

    ifstream cppcheck_out("CPR4_C99_cppcheck_output.txt", ios_base::in);//processing cppcheck result
    if(!cppcheck_out.is_open())
    {
        if(showflag == 1)
            logfile << "FILE : CPR4_C99_cppcheck_output.txt COULD NOT BE OPENED. SKIPPING..." << std::endl;
    }

    while(!cppcheck_out.eof())
    {
        std::string S_temp;
        getline(cppcheck_out, S_temp);

        if( S_temp.size() > 0 && S_temp.find(":") != string::npos && S_temp.find("]") != string::npos )//if line looks like this [file.c:line] ...
        {
            size_t i_beg = S_temp.find(":"), i_end = S_temp.find("]");//we leave only line number
            string file = "./" + S_temp.substr(1, i_beg - 1);
            S_temp = S_temp.substr(i_beg + 1, i_end - i_beg - 1);

            if( file != *path )
                continue;

            if( result->end() == find(result->begin(), result->end(), std::stoll(S_temp)) )
                result->push_back(std::stoll(S_temp));//string is translated to long long
        }
    }

    cppcheck_out.close();

    remove("CPR4_C99_cppcheck_output.txt");

    return 0;
};

//for each file in version we find fragment located at position where CppCheck found and either creates new cluster or add to existing
int initialize_clusters(vector<string>* Paths, vector<Cluster>* clusters, string SHA1, size_t FragmentSize, string path_to_fake_libc, char showflag, ofstream& logfile)
{
    for(size_t i = 0; i < Paths->size(); ++i)
    {
        ifstream in_file((*Paths)[i].c_str(), ios_base::in);
        if(!in_file.is_open())
        {
            if(showflag == 1)
                logfile << "FILE : " << (*Paths)[i] << " COULD NOT BE OPENED. ABORTING..." << std::endl;
            continue;
        }

        //create vector of defect locations and fill it
        vector<long long> defect_lines;
        find_defects(&((*Paths)[i]), &defect_lines, showflag, logfile);

        //for each processed file we list defects found by CppCheck
        if(showflag == 1)
        {
            logfile << "Defect lines in " << (*Paths)[i] << " : " << endl;
            for(size_t iii = 0; iii < defect_lines.size();++iii)
                logfile << defect_lines[iii] << endl;
            logfile << endl;
        }

        std::vector<std::string> previous;//contains strings before commentary in number of FragmentSize
        //and if needed commentary is found then contains previous.size() + more

        long long line = 0;//stores current number of line that was read

        //this flag is needed to find multiline comments
        char multiline_comment = 0;

        while(!in_file.eof() && defect_lines.size() > 0)
        {
            //this flag is used to skip parametrization
            char continue_flag = 0;

            std::string S_temp;//S_temp stores line that has been read
            std::getline(in_file, S_temp);
            line++;//number of current line

            Delete_Extra_Spaces(&S_temp);

            //if we've read not end of file, not empty string and not a commentary
            if(!in_file.eof() && Is_String_Not_Empty(S_temp) == 1 && S_temp[0] != '/' && S_temp[0] != '#' )
            {
                if(multiline_comment == 1)
                {
                    if( S_temp.size() >= 2 && S_temp[ S_temp.size() - 1 ] == '/' && S_temp[ S_temp.size() - 2 ] == '*' )
                    {
                        multiline_comment = 0;
                        continue;
                    }
                    else
                    {
                        continue;
                    }
                }

                //we check if last line in prev does not end with ; or { or } - meaning it's an incomplete line
                if(previous.size() > 0)
                {
                    string last = previous[previous.size() - 1];//last string from previous
                    //incomplete line can be found for example when defining a function in some of C coding styles
                    if(last[last.size() - 1] != ';' && last[last.size() - 1] != '}' && last[last.size() - 1] != '{')
                    {
                        S_temp = last + " " + S_temp;//new S_temp is last + S_temp
                        previous.pop_back();//because last element was incomplete - we will add it again - full this time
                    }
                    else
                    {
                        if(last == "{" || last == ";")
                        {
                            S_temp = last + S_temp;
                            previous.pop_back();
                        }
                        else
                        {
                            if(S_temp == "}" || S_temp == "};" || S_temp == ";")
                            {
                                S_temp = last + S_temp;
                                previous.pop_back();
                                continue_flag = 1;
                            }
                        }
                    }
                }
                else
                {
                    if(S_temp == "}" || S_temp == "};")
                        continue;
                }

                Delete_Extra_Spaces(&S_temp);

                std::string outstr;
                std::vector<string> temp_unused;
                if(continue_flag == 0)
                {
                    if(Parametrization(S_temp, &outstr, &temp_unused, showflag, logfile) == 1)//if we encountered a function declaration then
                    {//we'll check if we encountered a function declaration and skip it if so
                        if( showflag == 1)
                        {
                            logfile << "String number " << line << " contains NOT C lexeme." << endl;
                            logfile << S_temp << endl << outstr << endl;
                        }
                        continue;
                    }
                }

                //here we check if we encountered a function declaration - if so skip it
                string PFPF("CBP4_PARAMETERIZED_FUNCNAME_POSSIBLE_FUNCDEF");
                if( continue_flag == 0 && ( outstr.find( PFPF + "(" ) != string::npos || outstr.find( PFPF + " (") != string::npos ) && outstr[ outstr.size() - 1] != ';'
                && outstr.find("do") != 0 && outstr.find("else") != 0 && outstr.find("enum") != 0 && outstr.find("for") != 0 && outstr.find("if") != 0
                && outstr.find("sizeof") != 0 && outstr.find("return") != 0 && outstr.find("switch") != 0 && outstr.find("while") != 0 && outstr.find("{") != 0
                && outstr.find("case") != 0  && outstr[0] != '{' && outstr[0] != ';' && outstr[0] != '{' && outstr[0] != ';')
                {
                    previous.clear();//at this point we have some strings in PREVIOUS and we encountered a string that contains
                }
                else
                {
                    //if current line is in defect lines we need to form a fragment
                    if( !in_file.eof() && defect_lines.end() != find(defect_lines.begin(), defect_lines.end(), line) )
                    {
                        Exemplar Exmplr;
                        Exmplr.line = line;

                        int j = 0, prev_size = previous.size();//prev_size - size of previous[] before adding string with weakness
                        //j counts number of lines that have been added to previous[]

                        previous.push_back(S_temp);

                        char unexpected_eof = 0;
                        while(unexpected_eof != 1 && j < prev_size && !in_file.eof())
                        {
                            continue_flag = 0;

                                std::string S_temp2;
                                std::getline(in_file, S_temp2);
                                line++;

                                Delete_Extra_Spaces(&S_temp2);

                                //if we did not encounter end of file, string is not empty and it is not a commentary
                                if(!in_file.eof() && Is_String_Not_Empty(S_temp2) == 1 && S_temp2[0] != '/' && S_temp2[0] != '#')
                                {

                                    if(multiline_comment == 1)
                                    {
                                        if( S_temp2.size() >= 2 && S_temp2[ S_temp2.size() - 1 ] == '/' && S_temp2[ S_temp2.size() - 2 ] == '*' )
                                        {
                                            multiline_comment = 0;
                                            continue;
                                        }
                                        else
                                        {
                                            continue;
                                        }
                                    }

                                    //we check if last line in prev does not end with ; or { or } - meaning it's an incomplete line
                                    if(previous.size() > 0)
                                    {
                                        string last = previous[previous.size() - 1];//last string from previous
                                        //incomplete line can be found for example when defining a function in some of C coding styles
                                        if(last[last.size() - 1] != ';' && last[last.size() - 1] != '}' && last[last.size() - 1] != '{')
                                        {
                                            S_temp2 = last + " " + S_temp2;//new S_temp is last + S_temp
                                            previous.pop_back();//because last element was incomplete - we will add it again - full this time
                                        }
                                        else
                                        {
                                            if(last == "{")
                                            {
                                                S_temp2 = last + S_temp2;
                                                previous.pop_back();
                                            }
                                            else
                                            {
                                                if(S_temp2 == "}" || S_temp2 == "};" || S_temp2 == ";")
                                                {
                                                    S_temp2 = last + S_temp2;
                                                    previous.pop_back();
                                                    continue_flag = 1;
                                                    --j;
                                                }
                                            }
                                        }
                                    }

                                    Delete_Extra_Spaces(&S_temp2);

                                    std::string outstr2;
                                    std::vector<string> temp2_unused;

                                    if(continue_flag == 0)
                                    {
                                        if(Parametrization(S_temp2, &outstr2, &temp2_unused, showflag, logfile) == 1)//if we encountered a function declaration then
                                        {
                                            if(showflag == 1)
                                            {
                                                logfile << "String number " << line << " contains NOT C lexeme." << endl;
                                                logfile << S_temp << endl << outstr2 << endl;
                                            }
                                            continue;
                                        }
                                    }

                                    //here we check if we encountered a function declaration - if so skip it
                                    if( continue_flag == 0 && ( outstr2.find( PFPF + "(" ) != string::npos || outstr2.find( PFPF + " (") != string::npos )
                                    && outstr2[outstr2.size() - 1] != ';' && outstr2.find("do") != 0 && outstr2.find("else") != 0 && outstr2.find("enum") != 0
                                    && outstr2.find("for") != 0 && outstr2.find("if") != 0 && outstr2.find("sizeof") != 0 && outstr2.find("return") != 0
                                    && outstr2.find("switch") != 0 && outstr2.find("while") != 0 && outstr2.find("{") != 0 && outstr2.find("case") != 0
                                    && outstr2[0] != '{' && outstr2[0] != ';')
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
                                            if(showflag == 1)
                                                logfile << "Unexpected end of file " << (*Paths)[i] << endl;
                                            unexpected_eof = 1;
                                            break;
                                        }
                                    }

                                    if(S_temp2.size() >= 2 && S_temp2[0] == '/' && S_temp2[1] == '*')
                                    {
                                        multiline_comment = 1;
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
                                    Found_Equal = Exemplars_Are_Equal( (*clusters)[ix].commits[0].files[0].exemplars[0], Exmplr, path_to_fake_libc, showflag, logfile );
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
                        if(showflag == 1)
                            logfile << "Unexpected end of file " << (*Paths)[i] << endl;
                        break;
                    }
                }

                if(S_temp.size() >= 2 && S_temp[0] == '/' && S_temp[1] == '*')
                {
                    multiline_comment = 1;
                }
            }
        }

        in_file.close();

    }

    return 0;
}
#endif // CLUSTER_INIT_H_INCLUDED
