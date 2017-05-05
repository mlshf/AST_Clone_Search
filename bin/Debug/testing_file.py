from pycparser import parse_file, c_parser, c_generator, c_ast
import io
from contextlib import redirect_stdout
import sys
from anytree import RenderTree, Node
import astmscp

# with open("C:/Users/fu3uk/Desktop/a/main.c", 'r') as fin:
    # print(fin.read())

# with open("C:/Users/fu3uk/Desktop/a/main_pp.c", 'r') as fin:
    # print(fin.read())

if __name__ == "__main__":

    '''print(len(sys.argv))
    for i in range(0, len(sys.argv)):
        print(str(i) + " " + sys.argv[i])
    print("")'''

    if len(sys.argv) < 3:
        print("Too few arguments - need >= 2")
        quit()
try:
    ast1 = parse_file(sys.argv[1])
    ast2 = parse_file(sys.argv[2])
except:
    print("Failed to parse file")
    quit()

ast1_root = Node(";")
ast2_root = Node(";")

for i in range(0, len(ast1.ext)):
    if (not "Typedef" in str(ast1.ext[i])) and (not "Pragma" in str(ast1.ext[i])):
        compound = ast1.ext[i].body.block_items
        if not "None" in str(compound):
            astmscp.ast_compound(compound, ast1_root, ast1_root)

for i in range(0, len(ast2.ext)):
    if (not "Typedef" in str(ast2.ext[i])) and (not "Pragma" in str(ast2.ext[i])):
        compound = ast2.ext[i].body.block_items
        if not "None" in str(compound):
            astmscp.ast_compound(compound, ast2_root, ast2_root)

r = Node("?")

'''
r1 = Node(";")
r1n1 = Node("=", parent = r1)
r1n11 = Node("x", parent = r1n1)
r1n12 = Node("+", parent=r1n1)
r1n121 = Node("a", parent = r1n12)
r1n122 = Node("/", parent = r1n12)
r1n1221 = Node("1", parent=r1n122)
r1n1222 = Node("27", parent=r1n122)
r1n2 = Node("return", parent = r1)

r2 = Node(";")
r2n1 = Node("=", parent = r2)
r2n11 = Node("X2", parent = r2n1)
r2n12 = Node("+", parent=r2n1)
r2n121 = Node("B", parent = r2n12)
r2n122 = Node("*", parent = r2n12)
r2n1221 = Node("3", parent=r2n122)
r2n1222 = Node("-", parent=r2n122)
r2n12221 = Node("y", parent=r2n1222)
r2n12222 = Node("ZZZ", parent=r2n1222)
r2n2 = Node("exit", parent = r2)
'''

num = 1
rules1 = []
rules2 = []

astmscp.createAUT(ast1_root, ast2_root, r, num, rules1, rules2)

distance = astmscp.distanceAUT(rules1, rules2)
#if distance is <= 4/3 * sqrt( size1, size2 ) - average geometric * 4/3
if distance * distance * 9 <= 16 * astmscp.AST_size( ast1_root ) * astmscp.AST_size( ast2_root ):
	print(1)
else:
	print(0)

if "-show" in sys.argv:
    #ast1.show()
    print("AST #1:\n")
    for i in ast1.ext:
        if not ("Typedef" in str(i)):
            i.show()
    print("")
    print("AST #2:\n")
    for i in ast2.ext:
        if not ("Typedef" in str(i)):
            i.show()
    #ast2.show()
    print("")
    print("\nAST #1:\n")
    for pre, fill, node in RenderTree(ast1_root):
        print("%s%s" % (pre, node.name))
    print("")
    print("\nAST #2:\n")
    for pre, fill, node in RenderTree(ast2_root):
        print("%s%s" % (pre, node.name))
    print("")
    print("\nAUT:\n")
    for pre, fill, node in RenderTree(r):
        print("%s%s" % (pre, node.name))
    print("")

#print(aString) #This prints "None" instead of printing the parse tree
