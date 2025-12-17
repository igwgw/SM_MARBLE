//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h

/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/
#define SMMNODE_TYPE_LECTURE                0
#define SMMNODE_TYPE_RESTAURANT             1
#define SMMNODE_TYPE_LABORATORY             2
#define SMMNODE_TYPE_HOME                   3
#define SMMNODE_TYPE_GOTOLAB                4
#define SMMNODE_TYPE_FOODCHANGE             5
#define SMMNODE_TYPE_FESTIVAL               6

#define SMMNODE_OBJTYPE_BOARD     0
#define SMMNODE_OBJTYPE_GRADE     1
#define SMMNODE_OBJTYPE_FOOD      2
#define SMMNODE_OBJTYPE_FEST      3

typedef enum {
    GRADE_A_P = 0,
    GRADE_A0,
    GRADE_A_M,
    GRADE_B_P,
    GRADE_B0,
    GRADE_B_M,
    GRADE_C_P,
    GRADE_C0,
    GRADE_C_M,
    GRADE_D_P,
    GRADE_D0,
    GRADE_D_M,
    GRADE_F
} smmGrade_e;
#define SMMNODE_MAX_GRADE         13


//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade);
int smmObj_getNodeType(int node_nr);

char* smmObj_getTypeName(int node_type);
int smmObj_getNodeCredit(int node_nr);
//member retrieving


//element to string
char* smmObj_getTypeName(int node_type);
char* smmObj_getGradeName(int grade);

//
int smmObj_genNode(char* name, int type, int credit, int energy);

char* smmObj_getName(int node_nr);
int   smmObj_getType(int node_nr);
int   smmObj_getNodeCredit(int node_nr);
int   smmObj_getNodeEnergy(int node_nr);

int   smmObj_getObjectGrade(void *ptr);
int   smmObj_getObjectEnergy(void *ptr);
char* smmObj_getObjectName(void *ptr);
int   smmObj_getObjectType(void *ptr);
int   smmObj_getObjectCredit(void *ptr);

extern const double gradePoint[SMMNODE_MAX_GRADE];

#endif /* smm_object_h */
