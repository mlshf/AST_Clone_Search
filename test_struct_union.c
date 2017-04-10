#include <stdio.h>
#include <stdlib.h>

void temp_for_AST(int b, int k, char* l[])
{
    const static volatile struct STRUCT_1
    {
        int FIELD_1[12];
        const volatile enum ENUM_1 {a,b,c,d} FIELD_2;
        const volatile float* FIELD_3;
        union UNION_1 FIELD_4;

    }* VAR_1 = ID_1;

    const static volatile union UNION_2
    {
        char FIELD_5;
        long FIELD_6;
        struct STRUCT_2 {int** a; int* b[];} FIELD_7;
    } VAR_2 = {ID_2, ID_3, ID_4, ID_5, .SF1.SSF_x = 100 / sizeof(BIG) + FUNC_1(fnctn(sizeof(T + (long)ID)))};

    struct STRUCT_3 S_T_R_U_C_T_U_R_E;
}
