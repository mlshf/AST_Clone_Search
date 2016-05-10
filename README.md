# Course_Realization
Uses Boost library.
Standard: C++11.
Uses GraphViz (dot).
Uses popen(...) function => Works on Linux-like systems. 
Tested on Ubuntu. 
Build using CodeBlocks IDE (open Course_Realization.cbp with it and build) or build using Makefile, that was produced from Course_Realization.cbp with cbp2make utility.

Launching:

<PATH> <SHA1> <SIZE> <MARKER> <OUTPUT>
<PATH> is replaced with path to executable of Course_Realization from directory, in which program is launched.
<SHA1> is replaced with path (analogically, from launching directory) to .txt file, that contains a list of SHA1-hashes of commits, that will be used for initializing.
<SIZE> is replaced with maximal distance in lines, that can be between center of processed fragment and  its borders.
For example for fragment:

aaa
bbb
ccc
ddd
eee
fff
ggg

central line is "ddd" and  <SIZE> is 3.
<MARKER> is replaced with text of the commentary that marks the lines which user wants to track. 
For example if lines are marked with commentary "//#_Weakness_Threat_# ...", then <MARKER> is '#_Weakness_Threat_#'.
<OUTPUT> is a path to file, that will contain result of programm's work, without extension. 
For example '../io_dir/output_file'. If there are two distinct types of fragments, then in ../io_dir directory there will be four files:
output_file_1.gv, output_file_2.gv, output_file_1.png and output_file_2.png, where .gv files are processed by graphviz and .png files present the result in a form of graph - file per distinct type of fragment.

Programm finds all fragments of C code of size not bigger than 2 * <SIZE> + 1 that are clones of those fragment of size <= 2 * <SIZE> + 1 whose central lines were marked with <MARKER> commentary in .c and .h files of each commit after the first one from the SHA1 list in <SHA1>. Clones can be different in names of variables and numeric constants. Result of work is a set of graphs, each of which describes a cluster of clones, that belong to unique type of original fragment with which cluster was initialized. All fragments of code in cluster are clones of some original fragment, clusters ( ~ sets) do not intersect. Originial frament location and size is described in description of the first commit in cluster.

Example of lanch:
../../Course_Realization/bin/Release/Course_Realization '../../io_dir/SHA1_list.txt' 3 ''#_Weakness_Threat_#' '../../io_dir/output_file'
So program will find all fragments of code, not bigger that 2 * 3 + 1 = 7 lines, whose centers were marked with commentary //#_Weakness_Threat_# ..., clones of these fragments and create .png files with graphs describing set of fragments, that belong to one unique type of original fragment (those, that were marked).
