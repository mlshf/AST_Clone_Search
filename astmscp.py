from pycparser import parse_file,c_parser, c_generator, c_ast
import io
from contextlib import redirect_stdout
import sys
from anytree import RenderTree, Node

#with open("C:/Users/fu3uk/Desktop/a/main.c", 'r') as fin:
    #print(fin.read())

#with open("C:/Users/fu3uk/Desktop/a/main_pp.c", 'r') as fin:
    #print(fin.read())

#practically, this generates ast for arguments of operators like x += a + b + 0.5 + sizeof(const volatile int) + sizeof(x/0.5 + 223) + int(y - 25 + printf(k))
def ast_value(value, parent_node):
    if "BinaryOp" in str(value):
        n = Node(str(value.op), parent=parent_node)
        ast_value(value.left, n)
        ast_value(value.right, n)
        return
    elif "UnaryOp" in str(value):
        n = Node(str(value.op), parent=parent_node)
        ast_value(value.expr, n)
        return
    elif "ID" in str(value):
        n = Node(str(value.name), parent=parent_node)
        return
    elif "Constant" in str(value):
        n = Node(str(value.value), parent=parent_node)
        return
    elif "Typename" in str(value):
        temp_str = ""
        for qual in value.quals:
            temp_str += str(qual) + " "
        for name in value.type.type.names:
            temp_str += str(name)
        n = Node(temp_str, parent=parent_node)
        return
    elif "FuncCall" in str(value):
        n = Node(str(value.name.name) + "()", parent=parent_node)
        for i in value.args.exprs:
            ast_value(i, n)
        return
    elif "Cast" in str(value):
        n = Node("("+ str(value.to_type.type.type.names[0]) + ")" + "()", parent=parent_node)
        ast_value(value.expr, n)
    elif "Assignment" in str(value):
        n = Node(str(value.op), parent=parent_node)
        ast_value(value.lvalue, n)
        ast_value(value.rvalue, n)
    elif "ArrayRef" in str(value):
        n = Node("ArrayRef", parent=parent_node)
        ast_value(value.name, n)
        ast_value(value.subscript, n)
        return
    elif "StructRef" in str(value):
        n = Node(str(value.type), parent=parent_node)
        ast_value(value.name, n)
        ast_value(value.field, n)
    else:
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" + str(value) + "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        return

def type_decl(Line, n):
    temp_str = "Variable qualifiers: "
    #for qual in Line.quals:
        #temp_str += str(qual) + " "
    if "const" in Line.quals:
        temp_str += "const "
    if "volatile" in Line.quals:
        temp_str += "volatile "
    if "restrict" in Line.quals:
        temp_str += "restrict "
    if "IdentifierType" in str(Line.type):
        for type in Line.type.names:
            temp_str += str(type)
    elif "Enum" in str(Line.type):
        temp_str += str(Line.type.name)
    definition = Node(temp_str, parent=n)
    name = Node("Name: " + str(Line.declname), parent=n)
    return


def ast_compound(Compound, parent_node):
    for Line in Compound:
        if "BinaryOp" in str(Line):
            ast_value(Line, parent_node)
        elif "UnaryOp" in str(Line):
            ast_value(Line, parent_node)
        elif "Break" in str(Line):
            n = Node("break", parent=parent_node)
        elif "Continue" in str(Line):
            n = Node("continue", parent=parent_node)
        elif "Decl" in str(Line):
            if "ArrayDecl" in str(Line.type):
                n = Node("ArrayDecl", parent=parent_node)
                if "TypeDecl" in str(Line.type.type):
                    type_decl(Line.type.type, n)
                #name = Node(str(Line.name), parent=n)
                dimension = Node("Dimension: ", parent=n)
                ast_value(Line.type.dim, dimension)
                if "const" in Line.storage:
                    n.children[0].name += " const"
                if "volatile" in Line.storage:
                    n.children[0].name += " volatile"
                if "restrict" in Line.storage:
                    n.children[0].name += " restrict"
                if "static" in Line.storage:
                    n.children[0].name += " static"
                #writing dimension qualifiers
                dim_qualfs = Node("Dimension qualifiers: ", parent=n)
                if "const" in Line.type.dim_quals:
                    dim_qualfs.name += "const "
                if "volatile" in Line.type.dim_quals:
                    dim_qualfs.name += "volatile "
                if "restrict" in Line.type.dim_quals:
                    dim_qualfs.name += "restrict "
                if "static" in Line.type.dim_quals:
                    dim_qualfs.name += "static "
                n_initlist = Node("InitList", parent=n)
                if not "None" in str(Line.init):
                    for initializer in Line.init.exprs:
                        ast_value(initializer, n_initlist)
            elif "TypeDecl" in str(Line.type):
                n = Node("TypeDecl", parent=parent_node)
                type_decl(Line.type, n)
                for qual in Line.storage:
                    n.children[0].name += " " + str(qual)
                n_init = Node("Init", parent=n)
                if not "None" in str(Line.init):
                    ast_value(Line.init, n_init)
            elif "Enum" in str(Line.type):
                n = Node("Enum", parent=parent_node)
                name = Node("Name: " + str(Line.type.name), parent=n)
                values = Node("Values", parent=n)
                for value in Line.type.values.enumerators:
                    ni = Node(value.name, parent = values)
                    if not "None" in str(value.value):
                        ast_value(value.value, ni)
                for qual in Line.storage:
                    n.children[0].name = " " + str(qual)
            elif "PtrDecl" in str(Line.type):
                n = Node("PtrDecl", parent=parent_node)
                name = Node(str(Line.name), parent=n)
                for qual in Line.storage:
                    n.children[0].name = " " + str(qual)
        elif "If" in str(Line):
            n = Node("If", parent = parent_node)
            n1 = Node(str(Line.cond), parent=n)
            n2 = Node(str(Line.iftrue), parent=n)
            n2 = Node(str(Line.iffalse), parent=n)
    return



ast1 = parse_file("C:/Users/fu3uk/Desktop/a/main_I.c")
#ast2 = parse_file("C:/Users/fu3uk/Desktop/a/main.c")
#ast1.show()
for i in ast1.ext:
    if not ("Typedef" in str(i)):
        i.show()
#print("")
#ast2.show()
print("")
lines = [8, 2]

ast_root = Node(";")

for i in range(0, len(ast1.ext)):
    if not "Typedef" in str(ast1.ext[i]):
        compound = ast1.ext[i].body.block_items
        ast_compound(compound, ast_root)

for pre, fill, node in RenderTree(ast_root):
    print("%s%s" % (pre, node.name))
print("")


def createAUT(T1, T2, AUT, num, rules1, rules2):
    if(T1.name == T2.name) and (len(T1.children) == len(T2.children)):
        AUT.name = T1.name
        if(len(T1.children) > 0):
            for i,j in zip(T1.children, T2.children):
                k = Node("", parent = AUT)
                num += 1
                createAUT(i, j, k, num, rules1, rules2)
        else:
            return
    else:
        AUT.name = "?" + str(num)
        rules1.append([AUT.name, T1])
        rules2.append([AUT.name, T2])
        num += 1
        return

def distanceAUT(s1, s2):
    count1 = 0
    count2 = 0
    for a, b in zip(s1, s2):
        # print(a[0], "->")
        for pre, fill, node in RenderTree(a[1]):
            if node.is_leaf:
                count1 += 1
                # print("%s%s" % (pre, node.name))
        # print("")
        # print(b[0], "->")
        for pre, fill, node in RenderTree(b[1]):
            if node.is_leaf:
                count2 += 1
                # print("%s%s" % (pre, node.name))
                # print("")
    print("Distance between trees is:", count1 + count2 - len(rules1))

r = Node("?")
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

num = 1
rules1 = []
rules2 = []
createAUT(ast_root, r2, r, num, rules1, rules2)

for pre, fill, node in RenderTree(r1):
    print("%s%s" % (pre, node.name))
print("")

for pre, fill, node in RenderTree(r2):
    print("%s%s" % (pre, node.name))
print("")

for pre, fill, node in RenderTree(r):
    print("%s%s" % (pre, node.name))
print("")

distanceAUT(rules1, rules2)

#print(aString) #This prints "None" instead of printing the parse tree

