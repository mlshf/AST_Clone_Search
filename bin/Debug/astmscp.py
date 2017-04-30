from pycparser import parse_file,c_parser, c_generator, c_ast
import io
from contextlib import redirect_stdout
import sys
from anytree import RenderTree, Node


# creates an AST of qualifiers
def quals(Line, n):
    # for qual in Line.quals:
    # temp_str += str(qual) + " "
    # adding qualifiers to the temp_str.
    # They are added via IF because thus it won't depend on the order, in which quals were written in code
    qual = Node("Qualifiers: ", parent = n)
    temp_str = ""
    #const = Node("", parent=qual)
    #volatile = Node("", parent=qual)
    #restrict = Node("", parent=qual)
    #static = Node("", parent=qual)
    if "const" in Line.quals:
        #const.name = "const"
        temp_str += "const "
    if "volatile" in Line.quals:
        #volatile.name = "volatile"
        temp_str += "volatile "
    if "restrict" in Line.quals:
        #restrict.name = "restrict"
        temp_str += "restrct "
    if ".Decl" in str(Line):
        if "static" in Line.storage:
            #static.name = "static"
            temp_str += "static"
    qual.name += temp_str
    return


# practically, this generates ast for arguments of operators like x += a + b + 0.5 + sizeof(const volatile int)
# + sizeof(x/0.5 + 223) + int(y - 25 + printf(k))
def ast_value(value, parent_node):
    # operand is a binary operator
    if "BinaryOp" in str(value):
        n = Node(str(value.op), parent=parent_node)
        ast_value(value.left, n)
        ast_value(value.right, n)
        return
    # operand is an unary operator
    elif "UnaryOp" in str(value):
        n = Node(str(value.op), parent=parent_node)
        ast_value(value.expr, n)
        return
    # operand is and ID
    elif "ID" in str(value):
        n = Node(str(value.name), parent=parent_node)
        return
    # operand is a constant
    elif "Constant" in str(value):
        n = Node(str(value.value), parent=parent_node)
        return
    # operand is a typename
    elif "Typename" in str(value):
        n = Node("Typename", parent=parent_node)
        if_decl(value, n)
        return
    # operand is a function call
    elif "FuncCall" in str(value):
        n = Node(str(value.name.name) + "()", parent=parent_node)
        if "ExprList" in str(value.args):
            for i in value.args.exprs:
                ast_value(i, n)
        return
    # operand is a cast
    elif "Cast" in str(value):
        n = Node("Cast", parent=parent_node)
        n_type = Node("Type", parent=n)
        ast_value(value.to_type, n_type)
        ast_value(value.expr, n)
        return
    # operand is an assignment
    elif "Assignment" in str(value):
        n = Node(str(value.op), parent=parent_node)
        ast_value(value.lvalue, n)
        ast_value(value.rvalue, n)
        return
    # operand is an array reference
    elif "ArrayRef" in str(value):
        n = Node("ArrayRef", parent=parent_node)
        ast_value(value.name, n)
        ast_value(value.subscript, n)
        return
    # operand is a struct reference
    elif "StructRef" in str(value):
        n = Node(str(value.type), parent=parent_node)
        ast_value(value.name, n)
        ast_value(value.field, n)
        return
    # operand is named initializer
    elif "NamedInitializer" in str(value):
        n = Node("NamedInitializer", parent=parent_node)
        n_name = Node("Name", parent = n)
        for name in value.name:
            ast_value(name, n_name)
        ast_value(value.expr, n)
        return
    # operand is initialization list
    elif "InitList" in str(value):
        n = Node("InitList", parent = parent_node)
        if not "None" in str(value.exprs):
            for expr in value.exprs:
                ast_value(expr, n)
        return
    # operand is Case
    elif "Case" in str(value):
        n = Node("Case", parent=parent_node)
        ast_value(value.expr, n)
        ast_compound(value.stmts, n, n)
        return
    # operand is Default
    elif "Default" in str(value):
        n = Node("Default", parent=parent_node)
        ast_compound(value.stmts, n, n)
        return
    # operand is Compound
    elif ("Compound" in str(value)) and not ("CompoundLiteral" in str(value)):
        n = Node("Compound", parent=parent_node)
        if not "None" in str(value.block_items):
            ast_compound(value.block_items, n, n)
        return
    # operand is break
    elif "Break" in str(value):
        n = Node("break", parent=parent_node)
        return
    # line is continue operator
    elif "Continue" in str(value):
        n = Node("continue", parent=parent_node)
        return
    # operand is return
    elif "Return" in str(value):
        n = Node("return", parent = parent_node)
        if not "None" in str(value.expr):
            ast_value(value.expr, n)
        return
    # operand is a ternary operator
    elif "TernaryOp" in str(value):
        n = Node("TernaryOp", parent=parent_node)
        ast_value(value.cond, n)
        ast_value(value.iftrue, n)
        ast_value(value.iffalse, n)
        return
    elif "CompoundLiteral" in str(value):
        n = Node("CompoundLiteral", parent=parent_node)
        ast_value(value.type, n)
        ast_value(value.init, n)
        return
    else:
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" + str(value) + "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" + parent_node.name)
        return


# creates a in-program tree for Type Declaration subtree of PyCParser
def type_decl(Line, n):
    temp_str = "Variable type: "
    #built-in or typedef-defined type
    if "IdentifierType" in str(Line.type):
        for type in Line.type.names:
            temp_str += str(type)
        definition = Node(temp_str, parent=n)
        name = Node("Name: " + str(Line.declname), parent=n)
        return
    #enum
    elif "Enum" in str(Line.type):
        temp_str += "enum " + str(Line.type.name)
        # AST for values of ENUM
        definition = Node(temp_str, parent=n)
        name = Node("Name: " + str(Line.declname), parent=n)
        values = Node("Values", parent=n)
        # for each value (since it's practically an operand) an AST is created
        if "EnumeratorList" in str(Line.type.values):
            for value in Line.type.values.enumerators:
                ni = Node(value.name, parent=values)
                if not "None" in str(value.value):
                    ast_value(value.value, ni)
    elif ("Struct" in str(Line.type)) or ("Union" in str(Line.type)):
        definition = Node(temp_str + "struct " + str(Line.type.name), parent=n)
        if "Union" in str(Line.type):
            definition.name = temp_str + "union " + str(Line.type.name)
        name = Node("Name: " + str(Line.declname), parent=n)
        # create ast for fields
        n_decls = Node("Fields", parent=n)
        if not "None" in str(Line.type.decls):
            for decl in Line.type.decls:
                if not "None" in str(decl):
                    if_decl(decl, n_decls)
    elif "Decl" in str(Line.type):
        n_decl_name = ""
        if "PtrDecl" in str(Line.type):
            n_decl_name = "PtrDecl"
        elif "TypeDecl" in str(Line.type):
            n_decl_name = "TypeDecl"
        elif "ArrayDecl" in str(Line.type):
            n_decl_name = "ArrayDecl"
        n_decl = Node(n_decl_name + " ", parent = n)
        type_decl(Line.type, n_decl)
    return


# function that creates ast for declaration of some sort
def if_decl(Line, parent_node):
    # array is being declared
    if "ArrayDecl" in str(Line.type):
        n = Node("ArrayDecl", parent=parent_node)
        # create subtree of quals
        quals(Line, n)
        # calling a function that will build a tree with type qualifiers and typename
        if "Decl" in str(Line.type.type):
            type_decl(Line.type, n)
        # name = Node(str(Line.name), parent=n)
        # this fragment creates a tree for dimension of array
        dimension = Node("Dimension: ", parent=n)
        if not "None" in str(Line.type.dim):
            ast_value(Line.type.dim, dimension)
        # A node that contains dimension qualifiers
        dim_qualfs = Node("Dimension qualifiers: ", parent=n)
        if "const" in Line.type.dim_quals:
            dim_qualfs.name += "const "
        if "volatile" in Line.type.dim_quals:
            dim_qualfs.name += "volatile "
        if "restrict" in Line.type.dim_quals:
            dim_qualfs.name += "restrict "
        if "static" in Line.type.dim_quals:
            dim_qualfs.name += "static "
        # An AST for initialization list of array
        if ".Decl" in str(Line):
            n_initlist = Node("InitList", parent=n)
            if "InitList" in str(Line.init):
                for initializer in Line.init.exprs:
                    if not "None" in str(initializer):
                        ast_value(initializer, n_initlist)
            elif not "None" in str(Line.init):
                n_initlist.name = "Init"
                ast_value(Line.init, n_initlist)
    # variable is being declared
    elif "TypeDecl" in str(Line.type):
        n = Node("TypeDecl", parent=parent_node)
        # create subtree of quals
        quals(Line, n)
        # calling a function that will build an AST with type, type qualifiers and variable name
        type_decl(Line.type, n)
        # an AST for initializier of a declared variable
        if ".Decl" in str(Line):
            n_init = Node("Init", parent=n)
            if "InitList" in str(Line.init):
                n_init.name = "InitList"
                for initializer in Line.init.exprs:
                    if not "None" in str(initializer):
                        ast_value(initializer, n_init)
            elif not "None" in str(Line.init):
                    ast_value(Line.init, n_init)
    # enum declaration
    elif "Enum" in str(Line.type):
        n = Node("Enum", parent=parent_node)
        # create subtree of quals
        quals(Line, n)
        # Node - name of enum type being declared
        name = Node("Name: " + str(Line.type.name), parent=n)
        # AST for values of ENUM
        values = Node("Values", parent=n)
        # for each value (since it's practically an operand) an AST is created
        if not ("None" in str(Line.type.values)) and not ("None" in str(Line.type.values.enumerators)):
            for value in Line.type.values.enumerators:
                ni = Node(value.name, parent=values)
                if not "None" in str(value.value):
                    ast_value(value.value, ni)
    # pointer declaration
    elif "PtrDecl" in str(Line.type):
        n = Node("PtrDecl", parent=parent_node)
        # create subtree of quals
        quals(Line, n)
        # calling a function that will build an AST with type, type qualifiers and variable name
        if "Decl" in str(Line.type.type):
            type_decl(Line.type, n)
        # an AST for initializier of a declared variable
        if ".Decl" in str(Line):
            n_init = Node("Init", parent=n)
            if "InitList" in str(Line.init):
                n_init.name = "InitList"
                for initializer in Line.init.exprs:
                    if not "None" in str(initializer):
                        ast_value(initializer, n_init)
            elif not "None" in str(Line.init):
                    ast_value(Line.init, n_init)
    #struct or union declaration
    elif ("Struct" in str(Line.type)) or ("Union" in str(Line.type)):
        n = Node("Struct", parent=parent_node)
        if "Union" in str(Line.type):
            n.name = "Union"
        # create subtree of quals
        quals(Line, n)
        #create ast for fields
        n_decls = Node("Fields", parent=n)
        if not "None" in str(Line.type.decls):
            for decl in Line.type.decls:
                if not "None" in str(decl):
                    if_decl(decl, n_decls)
    return


#function creates a tree which represents an array of labels (if > 1) before some operator
def if_label(Line, grandparent_node, label_node, prev_node):
    temp_node = Node("Label", parent=prev_node)
    temp_label_name_node = Node("Name: " + str(Line.name), parent=temp_node)
    if "Label" in str(Line.stmt):
        if_label(Line.stmt, grandparent_node, label_node, temp_node)
    elif not "None" in str(Line.stmt):
        none_node = Node("Last label in row", parent = temp_node)
        ast_compound([Line.stmt], grandparent_node, temp_node)
    return


# creates a in-program tree for compound inside a function
def ast_compound(Compound, parent_node, prev_node):
    value_list = ["BinaryOp", "UnaryOp", "ID", "Constant", "Typename", "FuncCall", "Cast", "Assignment", "ArrayRef"]
    v_l_temp = ["StructRef", "NamedInitializer", "InitList", "Case", "Default", "CompoundLiteral", "Break", "Continue", "Return", "TernaryOp"]
    value_list.extend(v_l_temp)
    for Line in Compound:
        if any(word in str(Line) for word in value_list):
            ast_value(Line, parent_node)
        #something is being declared in this line
        elif "Decl" in str(Line):
            if_decl(Line, parent_node)
        elif ("Compound" in str(Line)) and not ("CompoundLiteral" in str(Line)):
            n = Node("Compound", parent = parent_node)
            if not "None" in str(Line.block_items):
                ast_compound(Line.block_items, n, n)
        elif "Switch" in str(Line):
            n = Node("Switch", parent = parent_node)
            n_cond = Node("Condition", parent=n)
            ast_value(Line.cond, n_cond)
            if ("Case" in str(Line.stmt)) or ("Default" in str(Line.stmt)):
                ast_value(Line.stmt, n)
            elif ("Compound" in str(Line.stmt)) and not ("CompoundLiteral" in str(Line.stmt)):
                for case in Line.stmt.block_items:
                    ast_value(case, n)
        elif "If" in str(Line):
            n = Node("If", parent = parent_node)
            n_cond = Node("Condition", parent=n)
            ast_value(Line.cond, n_cond)
            n_true = Node("IfTrue", parent=n)
            n_false = Node("IfFalse", parent=n)
            if not "None" in str(Line.iftrue):
                ast_compound([Line.iftrue], n_true, n_true)
            else:
                n_true_none = Node("None", parent=n_true)
            if not "None" in str(Line.iffalse):
                ast_compound([Line.iffalse], n_false, n_false)
            else:
                n_false_none = Node("None", parent=n_false)
        elif "Label" in str(Line):
            n = Node("Label")
            label_name_node = Node("Name: " + str(Line.name), parent = n)
            if "Label" in str(Line.stmt):
                if_label(Line.stmt, parent_node, n, n)
            elif not "None" in str(Line.stmt):
                ast_compound([Line.stmt], parent_node, n)
                none_node = Node("Last label in row", parent = n)
            n.parent = parent_node.children[len(parent_node.children) - 1]
        elif "Goto" in str(Line):
            n = Node("Goto", parent=parent_node)
            name = Node("Name: " + str(Line.name), n)
        elif ("DoWhile" in str(Line)) or (("While" in str(Line)) and not ("Do" in str(Line))):
            n = Node("While", parent=parent_node)
            if "DoWhile" in str(Line):
                n.name = "DoWhile"
            n_cond = Node("Condition", parent=n)
            ast_value(Line.cond, n_cond)
            if ("Compound" in str(Line.stmt)) and not ("CompoundLiteral" in str(Line.stmt)):
                ast_value(Line.stmt, n)
            elif not "None" in str(Line.stmt):
                ast_compound([Line.stmt], n, n)
        elif "For" in str(Line):
            n = Node("For", parent=parent_node)
            n_init = Node("Init", parent=n)
            if "DeclList" in str(Line.init):
                ast_compound(Line.init.decls, n_init, n_init)
            elif "ExprList" in str(Line.init):
                for expr in Line.init.exprs:
                    ast_value(expr, n_init)
            elif not "None" in str(Line.init):
                ast_value(Line.init, n_init)
            #
            n_cond = Node("Condition", parent=n)
            if "ExprList" in str(Line.cond):
                for expr in Line.cond.exprs:
                    ast_value(expr, n_cond)
            elif not "None" in str(Line.cond):
                ast_value(Line.cond, n_cond)
            #
            n_next = Node("Next", parent=n)
            if "ExprList" in str(Line.next):
                for expr in Line.next.exprs:
                    ast_value(expr, n_next)
            elif not "None" in str(Line.next):
                ast_value(Line.next, n_next)
            #
            if ("Compound" in str(Line.stmt)) and not ("CompoundLiteral" in str(Line.stmt)):
                ast_value(Line.stmt, n)
            elif not "None" in str(Line.stmt):
                ast_compound([Line.stmt], n, n)
        elif "EmptyStatement" in str(Line):
            n = Node("Empty Statement;", parent = parent_node)
    #this is needed so that if instruction is different from compared one only in having label before it
    #distance would increase only by 1
    if (prev_node.name == ";") or (prev_node.name == "Compound"):
        for child in parent_node.children:
            if not "Label" in str(child.children):
                placeholder_label_node = Node("Label", parent = child)
                label_name_node = Node("None", parent = placeholder_label_node)
                none_node = Node("Last label in row", parent = placeholder_label_node)
    return


#creates Most Specialized Common Pattern of two trees
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


#finds distance between two trees
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
    return (count1 + count2 - len(s1))

#counts size of tree
def AST_size( tree ):
	count = 0
	for pre, fill, node in RenderTree( tree ):
		if node.is_leaf:
			count += 1
	return count

