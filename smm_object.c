//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODENR        100
#define MAX_NODETYPE      7




#define GRADE_A_P          0
#define GRADE_A0           1
#define GRADE_A_M          2
#define GRADE_B_P          3
#define GRADE_B0           4
#define GRADE_B_M          5
#define GRADE_C_P          6
#define GRADE_C0           7
#define GRADE_C_M          8
#define GRADE_D_P          9
#define GRADE_D0          10
#define GRADE_D_M         11
#define GRADE_F           12



static char smmObj_nodeName[MAX_NODETYPE][MAX_CHARNAME] = {
       "lecture",
       "restaurant",
       "laboratory",
       "home",
       "gotoLab",
       "foodChance",
       "festival"
};

static char smmObj_gradeName[SMMNODE_MAX_GRADE][MAX_CHARNAME] = {
       "A_P",
       "A0",
       "A_M",
       "B_P",
       "B0",
       "B_M",
       "C_P",
       "C0",
       "C_M",
       "D_P",
       "D0",
       "D_M",
       "F"
};
// grade point 4.3 max
static const double gradePoint[SMMNODE_MAX_GRADE] = {
    4.3,  // A+
    4.0,  // A0
    3.7,  // A-
    3.3,  // B+
    3.0,  // B0
    2.7,  // B-
    2.3,  // C+
    2.0,  // C0
    1.7,  // C-
    1.3,  // D+
    1.0,  // D0
    0.7,  // D-
    0.0   // F
};

//structure type definition
typedef struct {
    char name[MAX_CHARNAME];
    int objType;
    int type;
    int credit;
    int energy;
    int grade;
} smmObj_object_t;

static smmObj_object_t smmObj_board[MAX_NODENR];
static int smmObj_board_nr = 0;

//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade)
{
    smmObj_object_t* ptr;
    ptr = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
    
    strcpy(ptr->name, name);
    ptr->type = type;
    ptr->objType = objType;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ((void*)ptr);
}

//node generation
int smmObj_genNode(char* name, int type, int credit, int energy)
{
    smmObj_object_t* p;

    if (smmObj_board_nr >= MAX_NODENR) return smmObj_board_nr;

    p = &smmObj_board[smmObj_board_nr];
    strcpy(p->name, name);
    p->objType = SMMNODE_OBJTYPE_BOARD;
    p->type = type;
    p->credit = credit;
    p->energy = energy;
    p->grade = -1;

    smmObj_board_nr++;
    return smmObj_board_nr;
}

// grade 계산 
double smmObj_getGradePoint(int grade)
{
    if (grade < 0 || grade >= SMMNODE_MAX_GRADE)
        return 0.0;
    return gradePoint[grade];
}



//member retrieving_object
char* smmObj_getObjectName(void *ptr)
{
      smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
      
      return (objPtr->name);
}

int smmObj_getObjectCredit(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    if (objPtr == NULL) return 0;
    return objPtr->credit;
}

int smmObj_getObjectType(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    if (objPtr == NULL) return -1;
    return objPtr->type;
}

int smmObj_getObjectEnergy(void *ptr)
{
      smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
      
      return (objPtr->energy);
}

//member retrieving_node
char* smmObj_getTypeName(int node_type)
{
      return (smmObj_nodeName[node_type]);
}

char* smmObj_getGradeName(int grade)
{
    return smmObj_gradeName[grade];
}

char* smmObj_getName(int node_nr)
{
    if (node_nr < 0 || node_nr >= smmObj_board_nr) return "";
    return smmObj_board[node_nr].name;
}

int smmObj_getType(int node_nr)
{
    if (node_nr < 0 || node_nr >= smmObj_board_nr) return -1;
    return smmObj_board[node_nr].type;
}

int smmObj_getNodeCredit(int node_nr)
{
    if (node_nr < 0 || node_nr >= smmObj_board_nr) return 0;
    return smmObj_board[node_nr].credit;
}

int smmObj_getNodeEnergy(int node_nr)
{
    if (node_nr < 0 || node_nr >= smmObj_board_nr) return 0;
    return smmObj_board[node_nr].energy;
}
//gpa4.3 변환 
int smmObj_getObjectGrade(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return objPtr->grade;
}


