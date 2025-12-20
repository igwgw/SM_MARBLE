//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH  "marbleFoodConfig.txt"
#define FESTFILEPATH  "marbleFestivalConfig.txt"

//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

//player
typedef struct {
    char name[MAX_CHARNAME];
    int  position;        // current node
    int  energy;
    int  accumCredit;
    int  isExperiment;    // 1=experiment-ing
    int  expGoal;         // experiment success threshold
} smmPlayer_t;

static smmPlayer_t cur_player[MAX_PLAYER];
static int player_nr = 0;

// ===========================================================
// function prototype
// ===========================================================
#if 0
int  isGraduated(void);                      //check if any player is graduated
void generatePlayers(int n, int initEnergy); //generate a new player
void printGrades(int player);                //print grade history of the player
void goForward(int player, int step);        //make player go "step" steps on the board
void printPlayerStatus(int player);          //print player status
float calcAverageGrade(int player);          //calculate average grade of the player
void* findGrade(int player, char *lectureName); //find grade from grade history
void actionNode(int player);                 //action code when a player stays at a node
int  rolldie(int player);
#endif


// ===========================================================
// opening
// ===========================================================
void opening(void)
{
    printf("\n");
    printf("=====================================================================\n");
    printf("||                                                                 ||\n");
    printf("||           ★.* S O O K M Y U N G   M A R B L E *.★               ||\n");
    printf("||                                                                 ||\n");
    printf("||                    (Total Credit : 30)                          ||\n");
    printf("||                                                                 ||\n");
    printf("=====================================================================\n\n");
}

// ===========================================================
// check if any player is graduated
// ===========================================================
int isGraduated(void)
{   int i;
    //credit 채우고 home이동 시 graduate 
    for (i = 0; i < player_nr; i++) {
        if (cur_player[i].accumCredit >= GRADUATE_CREDIT &&
            smmObj_getType(cur_player[i].position) == SMMNODE_TYPE_HOME)
        {
            return i;   // 졸업한 플레이어 return 
        }
    }
    return -1;          
}

// ===========================================================
// generate players
// ===========================================================
void generatePlayers(int n, int initEnergy)
{
    int i;

    for (i = 0; i < n; i++) {
        printf("Enter name of player %d: ", i + 1);
        fgets(cur_player[i].name, MAX_CHARNAME, stdin);
        cur_player[i].name[strcspn(cur_player[i].name, "\n")] = 0;

        cur_player[i].position     = 0;          // main에서 HOME
        cur_player[i].energy       = initEnergy;
        cur_player[i].accumCredit  = 0;
        cur_player[i].isExperiment = 0;
        cur_player[i].expGoal      = 0;
    }
}

// ===========================================================
// print player status
// ===========================================================
void printPlayerStatus(int player)
{
    int pos = cur_player[player].position;
    printf(" player[%d] %s / pos=%d(%s) / exp=%d(goal=%d) / credit=%d / energy=%d\n",
           player,
           cur_player[player].name,
           pos,
           smmObj_getName(pos),
           cur_player[player].isExperiment,
           cur_player[player].expGoal,
           cur_player[player].accumCredit,
           cur_player[player].energy);
}

// ===========================================================
// go forward 
// ===========================================================
void goForward(int player, int step)
{
    int pos;
    int s;

    pos = cur_player[player].position;

    if (step <= 0) return;

    printf("  Path: ");
    for (s = 0; s < step; s++) {
        pos = (pos + 1) % board_nr;

        printf("%s", smmObj_getName(pos));
        if (s != step - 1) printf(" -> ");

        // home 지나갈 때마다 에너지 보충 
        if (smmObj_getType(pos) == SMMNODE_TYPE_HOME) {
            cur_player[player].energy += smmObj_getNodeEnergy(pos);
        }
    }
    printf("\n");

    cur_player[player].position = pos;
}

// ===========================================================
// find grade
// ===========================================================
void* findGrade(int player, char *lectureName)
{
    int list_nr;
    int len;
    int i;

    list_nr = LISTNO_OFFSET_GRADE + player;
    len = smmdb_len(list_nr);

    for (i = 0; i < len; i++) {
        void* obj = smmdb_getData(list_nr, i);
        if (!obj) continue;

        if (strcmp(smmObj_getObjectName(obj), lectureName) == 0) {
            return obj;
        }
    }
    return NULL;
}

// ===========================================================
// calculate Average GPA 
// ===========================================================
float calcAverageGrade(int player)
{
    int list_nr;
    int len;
    int i;

    double sumPointXCredit;
    int sumCredit;

    list_nr = LISTNO_OFFSET_GRADE + player;
    len = smmdb_len(list_nr);

    sumPointXCredit = 0.0;
    sumCredit = 0;

    for (i = 0; i < len; i++) {
        void* obj = smmdb_getData(list_nr, i);
        int g, c;

        if (!obj) continue;

        g = smmObj_getObjectGrade(obj);
        c = smmObj_getObjectCredit(obj);

        if (g < 0 || g >= SMMNODE_MAX_GRADE) continue;

        sumPointXCredit += smmObj_getGradePoint(g) * (double)c;
        sumCredit += c;
    }

    if (sumCredit == 0) return 0.0f;
    return (float)(sumPointXCredit / (double)sumCredit);
}

// ===========================================================
// print grades
// ===========================================================
void printGrades(int player)
{
    int list_nr;
    int len;
    int i;


    list_nr = LISTNO_OFFSET_GRADE + player;
    len = smmdb_len(list_nr);

    printf("\n[Grade History] %s\n", cur_player[player].name);
    printf("  Lecture\tCredit\tGrade\tPoint\n");

    for (i = 0; i < len; i++) {
        void* obj = smmdb_getData(list_nr, i);
        int g;

        if (!obj) continue;

        g = smmObj_getObjectGrade(obj);

        printf("  %s\t%d\t%s\t%.1f\n",
               smmObj_getObjectName(obj),
               smmObj_getObjectCredit(obj),
               smmObj_getGradeName(g),
               smmObj_getGradePoint(g));

    }
    printf("\n");
}

// ===========================================================
// action node
// ===========================================================
void actionNode(int player)
{
    int pos;
    int type;

    pos  = cur_player[player].position;
    type = smmObj_getType(pos);

    printf("node: %s, type:%i(%s)\n",
           smmObj_getName(pos),
           type,
           smmObj_getTypeName(type));

    switch (type)
    {
        case SMMNODE_TYPE_LECTURE:
        {
            int choice;     //1=join, 0=drop 입력받아 판별 
            int credit;
            int costE;
            char *lecName;

            int   gradeIdx;
            void *gradeObj;

            credit  = smmObj_getNodeCredit(pos);
            costE   = smmObj_getNodeEnergy(pos);
            lecName = smmObj_getName(pos);

            if (findGrade(player, lecName) != NULL)
            {
                printf("  -> Already taken %s\n", lecName);
                break;
            }

            if (cur_player[player].energy < costE)
            {
                printf("  -> Too hungry to take %s (remained:%d, required:%d)\n",
                       lecName,
                       cur_player[player].energy,
                       costE);
                break;
            }

            printf("  -> Lecture %s (credit:%d, energy:%d)\n",
                   lecName,
                   credit,
                   costE);
            printf("     Join = 1, Drop = 0 : ");

            if (scanf("%d", &choice) != 1)
            {
                
                while (getchar() != '\n');
                printf("  -> Invalid input. Dropped.\n");
                break;
            }

            while (getchar() != '\n');

            if (choice != 1)
            {
                printf("  -> Dropped:(\n");
                break;
            }

            // 수강 처리 
            cur_player[player].energy      -= costE;
            cur_player[player].accumCredit += credit;

            //성적 랜덤 (A+~C-)
            gradeIdx = rand() % 9;  

            gradeObj = smmObj_genObject(
                lecName,
                SMMNODE_OBJTYPE_GRADE,
                SMMNODE_TYPE_LECTURE,
                credit,
                0,
                gradeIdx
            );

            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradeObj);

            printf("  -> %s takes %s with grade %s (avg: %.2f), remained energy: %d\n",
                   cur_player[player].name,
                   lecName,
                   smmObj_getGradeName(gradeIdx),
                   calcAverageGrade(player),
                   cur_player[player].energy);
        }
        break;


        case SMMNODE_TYPE_RESTAURANT:
        {
            int gain;
            gain = smmObj_getNodeEnergy(pos);
            cur_player[player].energy += gain;
            printf("  -> Restaurant: energy +%d => %d\n", gain, cur_player[player].energy);
        }
        break;

        case SMMNODE_TYPE_HOME:
            printf("  -> Home!!^_^ \n");
            break;

        case SMMNODE_TYPE_FOODCHANCE:
        {
            int idx;
            void* card;
            int gain;

            if (food_nr <= 0) {
                printf("  -> FoodChance: no food cards.\n");
                break;
            }

            idx = rand() % food_nr;
            card = smmdb_getData(LISTNO_FOODCARD, idx);//random food 선택 후 에너지 보충 
            if (!card) break;

            gain = smmObj_getObjectEnergy(card);
            cur_player[player].energy += gain;

            printf("  -> FoodChance: got [%s], energy +%d => %d\n",
                   smmObj_getObjectName(card), gain, cur_player[player].energy);
        }
        break;

        case SMMNODE_TYPE_FESTIVAL:
        {
            int idx;
            void* card;

            if (festival_nr <= 0) {
                printf("  -> Festival: no festival cards.\n");
                break;
            }

            idx = rand() % festival_nr;
            card = smmdb_getData(LISTNO_FESTCARD, idx); //미션 1개 random drop
            if (!card) break;

            printf("  -> MISSION: %s\n", smmObj_getObjectName(card));
        }
        break;

        case SMMNODE_TYPE_GOTOLAB:
        {
            int lab;
            int i;

            cur_player[player].isExperiment = 1;
            cur_player[player].expGoal = (rand() % MAX_DIE) + 1; //실험 성공값 랜덤 지정 


            lab = -1;
            for (i = 0; i < board_nr; i++) {
                if (smmObj_getType(i) == SMMNODE_TYPE_LABORATORY) { lab = i; break; }
            }

            if (lab < 0) {
                printf("  [ERROR] No LABORATORY node.\n");
                break;
            }

            cur_player[player].position = lab; //실험시 실험실로 향함 
            printf("  -> GotoLab: experiment START, goal=%d, moved to lab(%s)\n",
                   cur_player[player].expGoal, smmObj_getName(lab));
        }
        break;

        case SMMNODE_TYPE_LABORATORY:
        {
            if (cur_player[player].isExperiment) {
                int cost;
                int die;

                cost = smmObj_getNodeEnergy(pos);
                die  = (rand() % MAX_DIE) + 1;

                cur_player[player].energy -= cost;

                printf("  -> Experiment roll=%d (goal=%d), energy -%d => %d\n",
                       die, cur_player[player].expGoal, cost, cur_player[player].energy);

                if (die >= cur_player[player].expGoal) {
                    cur_player[player].isExperiment = 0;
                    printf("  -> SUCCESS! You can move next turn.\n");
                } else {
                    printf("  -> FAIL. Stay in lab.\n");
                }
            } else {
                printf("  -> Laboratory: (not experiment) pass.\n"); //실험 중 아닌 실험실인 경우  
            }
        }
        break;

        default:
            break;
    }
}

// ===========================================================
// roll die
// ===========================================================
int rolldie(int player)
{
    int c;

    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);

    if (c == 'g' || c == 'G')
        printGrades(player);

    return (rand() % MAX_DIE + 1);
}

// ===========================================================
// main
// ===========================================================
int main(int argc, const char * argv[])
{
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;

    int i;
    int turn;

    int homePos;
    int initEnergy;

    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;

    srand((unsigned)time(NULL));
    opening();

    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }

    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4)
    {
        board_nr = smmObj_genNode(name, type, credit, energy);
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);

    //2. food card config
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[WARN] failed to open %s (food cards will be empty)\n", FOODFILEPATH);
    }
    else
    {
        printf("\nReading food card component......\n");
        while (fscanf(fp, "%s %i", name, &energy) == 2)
        {
            void* obj = smmObj_genObject(name, SMMNODE_OBJTYPE_FOOD, 0, 0, energy, -1);
            smmdb_addTail(LISTNO_FOODCARD, obj);
            food_nr++;
        }
        fclose(fp);
        printf("Total number of food cards : %i\n", food_nr);
    }

    //3. festival card config
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[WARN] failed to open %s (festival cards will be empty)\n", FESTFILEPATH);
    }
    else
    {
        printf("\nReading festival card component......\n");
        while (fscanf(fp, "%s", name) == 1)
        {
            void* obj = smmObj_genObject(name, SMMNODE_OBJTYPE_FEST, 0, 0, 0, -1);
            smmdb_addTail(LISTNO_FESTCARD, obj);
            festival_nr++;
        }
        fclose(fp);
        printf("Total number of festival cards : %i\n", festival_nr);
    }

    //2. Player configuration ---------------------------------------------------------------------------------
    do
    {
        printf("\nEnter number of players (1~%d): ", MAX_PLAYER);
        scanf("%d", &player_nr);
        getchar(); // consume '\n'
    }
    while (player_nr < 1 || player_nr > MAX_PLAYER);

    // HOME 위치 찾기 
    homePos = 0;
    for (i = 0; i < board_nr; i++) {
        if (smmObj_getType(i) == SMMNODE_TYPE_HOME) { homePos = i; break; }
    }
    initEnergy = smmObj_getNodeEnergy(homePos);

    generatePlayers(player_nr, initEnergy);

    // 시작 위치 HOME으로
    for (i = 0; i < player_nr; i++) {
        cur_player[i].position = homePos;
        cur_player[i].energy   = initEnergy;
        cur_player[i].accumCredit = 0;
        cur_player[i].isExperiment = 0;
        cur_player[i].expGoal = 0;
    }

    //3. SM Marble game starts ---------------------------------------------------------------------------------
    turn = 0;
    while (1) //graduate 여부 확인 
    {
        int die_result;

        //4-1. initial printing
        
        //누구 turn인지 표시 
        printf("\n---------------------------------------------------------------------\n");
        printf(">> TURN : Player %d [%s] <<\n", turn, cur_player[turn].name);
        printf("---------------------------------------------------------------------\n");

        printf("\n========================== PLAYER STATUS ============================\n");
        for (i = 0; i < player_nr; i++)
            printPlayerStatus(i);
        printf("=====================================================================\n\n");

        //4-2. die rolling (if not in experiment)
        if (cur_player[turn].isExperiment == 0)
        {
            die_result = rolldie(turn);
            printf("---> result: %i\n", die_result);
        }
        else
        {
            die_result = 0; // 실험중이면 이동 금지
        }

        //4-3. go forward
        goForward(turn, die_result);

        //4-4. take action at the destination node of the board
        actionNode(turn);
        
        int gradPlayer;

        gradPlayer = isGraduated();
        if (gradPlayer != -1)
        {
               printf("\n=================================================\n");
               printf("++  CONGRATULATIONS! PLAYER %d [%s] GRADUATED!   ++\n",
                       gradPlayer,
                       cur_player[gradPlayer].name);
               printf("=================================================\n");

    
               printGrades(gradPlayer);

               printf("\n** ♡ GAME OVER ♡ **\n");
               break;   // credit채워 graduate 시 게임 종료
         }


        //4-5. next turn
        turn = (turn + 1) % player_nr;
    }

    system("PAUSE");

    return 0;
}

