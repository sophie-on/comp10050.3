

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "main.h"
#include "crossfireOperations.h"


char *slotTypeName(SlotType s)
{
    switch (s)
    {
        case Ground:
            return "Ground";
        case City:
            return "City";
        case Hill:
            return "Hill";
        default:
            return "";
    }
}

char *slotTypeLetter(SlotType s)
{
    switch (s)
    {
        case Ground:
            return "G";
        case City:
            return "C";
        case Hill:
            return "H";
        default:
            return "";
    }
}

char *playerTypeName(PlayerType t)
{
    switch (t)
    {
        case Elf:
            return "Elf";
        case Human:
            return "Human";
        case Ogre:
            return "Ogre";
        case Wizard:
            return "Wizard";
        default:
            return "";
    }
}

char *playerPrintName(Player p)
{
    char *printName = (char *) malloc(64 * sizeof(char));
    sprintf(printName, "%s (%s) [%dHP]", p.name, playerTypeName(p.type), p.life);
    return printName;
}

Player *players;
size_t players_count;
size_t players_alive;

Slot *slots;
size_t slots_count;
//slot slotBoard[7][7];

int main(void)
{
    srand((unsigned int) time(NULL));
    
    printf("Welcome to:\n"
           "   _____                   ______ _          \n"
           "  / ____|                 |  ____(_)         \n"
           " | |     _ __ ___  ___ ___| |__   _ _ __ ___ \n"
           " | |    | '__/ _ \\/ __/ __|  __| | | '__/ _ \\\n"
           " | |____| | | (_) \\__ \\__ \\ |    | | | |  __/\n"
           "  \\_____|_|  \\___/|___/___/_|    |_|_|  \\___|\n\n");
    
    // Command prompt must be in utf-8 mode
    system("chcp 65001");
    
    // Demo mode fills in a default set of players
    bool demo_mode;
    char d;
    
    do
    {
        printf("Would you like to use (d)emo mode or (r)egular mode?:\n");
        scanf(" %c", &d);
    }
    
    while (d != 'd' && d != 'r');
    
    demo_mode = d == 'd';
    
    if (!demo_mode)
    {
        do
        {
            printf("Enter number of players [2..6]:\n");
            scanf("%zd", &players_count);
        }
        
        while (players_count < 2 || players_count > 6);
    }
    else
    {
        players_count = 4;
    }
    
    // Allocate players array
    
    players = (Player *) malloc(sizeof(Player) * players_count);
    
    if (!demo_mode)
    {
        // Print list of possible player types
        printf("Player types: (e)lf, (h)uman, (o)gre, (w)izard\n");
        
        // Index at which to insert next player
        
        int players_index = 0;
        
        while (players_index < players_count)
        {
            // Allocate memory for storing player name
            char *name = (char *) malloc(64 * sizeof(char));
            PlayerType type;
            
            char t;
            printf("Enter type and player name:\n");
            scanf(" %c %[^\n]", &t, name);
            
            // Convert character to player type
            
            if (t == 'e')
            {
                type = Elf;
            }
            else if (t == 'h')
            {
                type = Human;
            }
            else if (t == 'o')
            {
                type = Ogre;
            }
            else if (t == 'w')
            {
                type = Wizard;
            }
            else
            {
                printf("Invalid type.\n");
                // If invalid player type is given repeat the loop
                continue;
            }
            
            // Store player information in struct
            
            Player p;
            p.type = type;
            p.name = name;
            
            // Add player to players array
            
            players[players_index++] = p;
        }
    }
    
    else
    {
        // Default names and types
        char *demo_names[] = {"John Sheekey", "Mark Dukes", "Henry McLoughlin", "Liliana Pasquale"};
        PlayerType demo_types[] = {Human, Ogre, Wizard, Elf};
        
        for (int i = 0; i < players_count; i++)
        {
            Player p;
            p.type = demo_types[i];
            p.name = demo_names[i];
            
            players[i] = p;
        }
    }
    
    // Fill in initial skill points
    for (int i = 0; i < players_count; i++)
    {
        // Players start alive at 100 health
        players[i].alive = true;
        players[i].life = 100;
        
        // Different player types have different starting skill points
        if (players[i].type == Elf)
        {
            players[i].luck = rand_range(60, 100);
            players[i].smartness = rand_range(70, 100);
            players[i].strength = rand_range(1, 50);
            players[i].magic = rand_range(51, 79);
            players[i].dexterity = rand_range(1, 100);
        }
        else if (players[i].type == Human)
        {
            // Total skill points must be less than 300, and individual skill points must be > 0
            // Therefore the range of the total values is 5..299
            // Select a random value for total
            int total = rand_range(5, 299);
            
            // Choose 5 random values, and subtract from the remaining total points each time
            // Values are between 1 and 100 or the remaining total whichever is smaller
            // If the total is smaller, a point is reserved for each remaining skill
            
            int values[5];
            
            for (int j = 0; j < 5; j++)
            {
                values[j] = rand_range(1, ((total - 5 + j) < 100) ? (total - 5 + j) : 100);
                total -= values[j];
            }
            
            // Once 5 random equally distributed values have been chosen, shuffle them and
            // insert into each of the 5 skill categories.
            
            shuffle(values, 5, sizeof(int));
            
            players[i].luck = values[0];
            players[i].smartness = values[1];
            players[i].strength = values[2];
            players[i].magic = values[3];
            players[i].dexterity = values[4];
        }
        
        else if (players[i].type == Ogre)
        {
            players[i].strength = rand_range(80, 100);
            players[i].dexterity = rand_range(80, 100);
            players[i].magic = 0;
            
            // Choose total of luck and smartness first
            // Then insert luck, and assign remaining total to smartness
            
            int sum = rand_range(0, 50);
            players[i].luck = rand_range(0, sum);
            players[i].smartness = sum - players[i].luck;
        }
        
        else if (players[i].type == Wizard)
        {
            players[i].luck = rand_range(50, 100);
            players[i].smartness = rand_range(70, 100);
            players[i].strength = rand_range(1, 20);
            players[i].magic = rand_range(80, 100);
            players[i].dexterity = rand_range(1, 100);
        }
    }
    
    if (!demo_mode)
    {
        printf("Enter number of slots(max = 20): \n"); ///<-----*
        scanf("%zd", &slots_count);
    }
    else
    {
        slots_count = 12;
    }
    
    // Allocate slots array
    slots = (Slot *) malloc(sizeof(Slot) * slots_count);
    
    // Assign random slot types
    for (int i = 0; i < slots_count; i++)
    {
        int r = rand_range(0, 2);
        
        if (r == 0)
        {
            slots[i].type = Ground;
        }
        else if (r == 1)
        {
            slots[i].type = City;
        }
        else if (r == 2)
        {
            slots[i].type = Hill;
        }
    }
    
    // Distribute players randomly among slots by first inserting them
    // at slot positions [0..players_count] and then shuffling the slots
    for (int i = 0; i < slots_count; i++)
    {
        slots[i].player = i < players_count ? i : -1;
    }
    shuffle(slots, (size_t) slots_count, sizeof(Slot));
    
    // Once slots have been shuffled, update players with their current slot
    for (int i = 0; i < slots_count; i++)
    {
        if (slots[i].player >= 0) {
            players[slots[i].player].slot = i;
        }
    }
    
    // At the beginning, all players are alive
    
    players_alive = players_count;
    
    // Continue main program loop as long as there is more than 1 player alive
    int round = 1;
    while (players_alive > 1)
    {
        printf("╔══════════╗\n");
        printf("║ Round %2d ║\n", round);
        printf("╚══════════╝\n");
        
        print_players();
        
        // Loop through each player
        for (int i = 0; i < players_count; i++)
        {
            Player p = players[i];
            
            // If player is not alive continue to next player
            if (!p.alive) continue;
            
            // Print player name in rounded box
            char *print_name = playerPrintName(p);
            unsigned long print_length = strlen(print_name) + 2;
            printf("╭──────────┬"); for (int x = 0; x < print_length; x++) printf("─"); printf("╮\n");
            printf("│ Player %d │ %s │\n", i + 1, print_name);
            printf("╰──────────┴"); for (int x = 0; x < print_length; x++) printf("─"); printf("╯\n");
            
            // Print current slots
            print_slots(players[i].slot, 0);
            
            char a;
            do
            {
                printf("Would you like to (m)ove or (a)ttack?:\n");
                scanf(" %c", &a);
            }
            while (a != 'a' && a != 'm');
            
            int m = 0;
            if (a == 'a') {
                attack(&players[i]);
            }
            else
            { // if (a == 'm')
                m = Moving(&players[i]);
                
                // If the player was unable to move, force attack
                if (m == 0)
                {
                    printf("Player can't move so must attack.\n");
                    attack(&players[i]);
                }
            }
            
            // Print new slots
            print_slots(players[i].slot, m);
            
            printf("\n\n");
        }
        
        // End of round standings
        printf("End of round %d\n", round);
        print_players();
        
        await_input();
        
        if (players_alive > 1)
        {
            // Clear screen for next round
            system("cls");
        }
        
        round++;
    }
    
    // Find winning player
    for (int i = 0; i < players_count; i++)
    {
        if (players[i].alive == true)
        {
            printf("%s has won after %d rounds!\n", playerPrintName(players[i]), round);
            break;
        }
    }
    
    int row, column;
    
    struct slot* currSlot = NULL;
    struct slot *foundSlots;
    bool explored[BOARD_SIZE][BOARD_SIZE];
    int count =0;
    
    //pointer to slot (0,0)
    struct slot *upLeft;
    
    //pointer to slot (0,boardSize -1)
    struct slot *upRight;
    
    //pointer to slot (boardSize - 1, 0)
    struct slot *downLeft;
    
    //pointer to slot (boardSize - 1, boardSize -1)
    struct slot *downRight;
    
    createBoard(BOARD_SIZE,&upLeft, &upRight, &downLeft, &downRight);
    
    getDesiredElement(BOARD_SIZE, &row,&column);
    
    if(row >= BOARD_SIZE/2)
    {
        if(column >= BOARD_SIZE/2)
            currSlot = reachDesiredElement(row,column,downRight);
        
        else
            
        /*If the the required slot is closer to the down-left
         * corner of the board the search starts from downLeft,
         * which points to slot at position (boardSize-1, 0)*/
            
            currSlot = reachDesiredElement(row,column,downLeft);
    }
    else
    {
        /*If the the required slot is closer to the up-right
         * corner of the board the search starts from upRight,
         * which points to slot at position (0, boarSize -1)*/
        if(column >= BOARD_SIZE/2)
            currSlot = reachDesiredElement(row,column, upRight);
        /*If the the required slot is closer to the up-left
         * corner of the board the search starts from upLeft,
         * which points to slot at position (0, 0)*/
        else currSlot = reachDesiredElement(row,column,upLeft);
    }
    
    for(int i=0; i<BOARD_SIZE; i++){
        for(int j=0; j<BOARD_SIZE;j++){
            explored[i][j] = false;
        }
    }
    
    foundSlots = malloc(BOARD_SIZE * BOARD_SIZE * sizeof(struct slot ));
    printf("\n\nFunction findSlotsinvoked:\n");
    
    if(currSlot!= NULL){
        //invokes function findSlots. The required distance is a constant
        //However you can reuse this function providing as input any arbitrary distance
        findSlots(REQ_DISTANCE, 0, currSlot, foundSlots, &count, explored);
        for(int i=0; i<count; i++){
            printf("(%d, %d)-> ",foundSlots[i].row, foundSlots[i].column);
        }
    }
    
    return 0;
    
}






void await_input()
{
    printf("Press enter to continue...\n");
    fflush(stdin);
    while(getchar() != '\n');
}


/**
 * Prints players information
 */

void print_players()
{
    printf("┏━━━━━━━━━━━━━━┱"); for (int i = 0; i < players_count; i++) printf("────────────────────%s", i < players_count - 1 ? "┬" : "┐"); printf("\n");
    printf("┃ %-12s ┃", "Player #"); for (int i = 0; i < players_count; i++) printf(" %-18d │", i + 1); printf("\n");
    printf("┣━━━━━━━━━━━━━━╉"); for (int i = 0; i < players_count; i++) printf("────────────────────%s", i < players_count - 1 ? "┼" : "┤"); printf("\n");
    printf("┃ %-12s ┃", "Name"); for (int i = 0; i < players_count; i++) printf(" %-18s │", players[i].name); printf("\n");
    printf("┃ %-12s ┃", "Type"); for (int i = 0; i < players_count; i++) printf(" %-18s │", playerTypeName(players[i].type)); printf("\n");
    printf("┃ %-12s ┃", "Slot"); for (int i = 0; i < players_count; i++) if (players[i].slot >= 0) printf(" #%02d %-14s │", players[i].slot + 1, slotTypeName(slots[players[i].slot].type)); else printf(" %-18s │", "Dead"); printf("\n");
    printf("┃ %-12s ┃", "Health"); for (int i = 0; i < players_count; i++) printf(" %-18d │", players[i].life); printf("\n");
    printf("┃ %-12s ┃", "Strength"); for (int i = 0; i < players_count; i++) printf(" %-18d │", players[i].strength); printf("\n");
    printf("┃ %-12s ┃", "Dexterity"); for (int i = 0; i < players_count; i++) printf(" %-18d │", players[i].dexterity); printf("\n");
    printf("┃ %-12s ┃", "Luck"); for (int i = 0; i < players_count; i++) printf(" %-18d │", players[i].luck); printf("\n");
    printf("┃ %-12s ┃", "Magic"); for (int i = 0; i < players_count; i++) printf(" %-18d │", players[i].magic); printf("\n");
    printf("┃ %-12s ┃", "Smartness"); for (int i = 0; i < players_count; i++) printf(" %-18d │", players[i].smartness); printf("\n");
    printf("┗━━━━━━━━━━━━━━┹"); for (int i = 0; i < players_count; i++) printf("────────────────────%s", i < players_count - 1 ? "┴" : "┘"); printf("\n");
}

/**
 * Prints the current slot standings
 *
 * @param position Slot position to highlight with thicker outline
 * @param direction_movement_to Direction to which player at position has moved, indicated with a small arrow
 */

void print_slots(int position, int direction_movement_to)
{
    /*
     * Row 1
     */
    
    // Top left corner
    printf(position != 0 ? "┌" : "┏");
    
    // Loop through slots
    
    for (int i = 0; i < slots_count; i++)
    {
        
        // Slot contains a player
        if (slots[i].player != -1)
        {
            Player p = players[slots[i].player];
            
            char *player_print_name = playerPrintName(p);
            unsigned long player_name_length = strlen(player_print_name);
            
            // Account for the fact that player name will be printed on row 2 by printing more horizontal lines
            
            for (int j = 0; j < player_name_length + 2; j++)
            {
                printf("%s", position != i ? "─" : "━");
            }
        }
        else
        {
            // No player, print a single horizontal line
            printf("%s", position != i ? "─" : "━");
        }
        
        // Vertical separator unless on last slot
        if (i < (slots_count - 1))
        {
            if (position == i)
            {
                printf("┱");
            }
            
            else if (position == i + 1)
            {
                printf("┲");
            }
            else
            {
                printf("┬");
            }
        }
    }
    
    // Top right corner
    printf(position != slots_count - 1 ? "┐" : "┓");
    
    printf("\n");
    
    /*
     * Row 2
     */
    
    // Middle left
    printf(position != 0 ? "│" : "┃");
    
    // Loop through slots
    
    for (int i = 0; i < slots_count; i++)
    {
        // Slot contains a player
        
        if (slots[i].player != -1)
        {
            Player p = players[slots[i].player];
            
            printf(" %s ", playerPrintName(p));
        }
        
        else
        {
            // If player at adjacent position just moved from this slot, print arrow
            if (i == position - direction_movement_to)
            {
                printf(direction_movement_to == -1 ? "←" : "→");
            }
            else
            {
                printf(" ");
            }
        }
        
        // Vertical separator unless on last slot
        
        if (i < (slots_count - 1))
        {
            printf(position == i || position == i + 1 ? "┃" : "│");
        }
    }
    
    // Middle right
    printf(position != slots_count - 1 ? "│" : "┃");
    
    printf("\n");
    
    /*
     * Row 3
     */
    
    // Bottom left corner
    printf(position != 0 ? "└" : "┗");
    
    // Loop through slots
    for (int i = 0; i < slots_count; i++)
    {
        // Slot contains a player
        if (slots[i].player != -1) {
            Player p = players[slots[i].player];
            
            // Calculations for space filling horizontal lines
            char *player_print_name = playerPrintName(p);
            char *slot_type_name = slotTypeName(slots[i].type);
            
            unsigned long player_name_length = strlen(player_print_name);
            unsigned long slot_type_name_length = strlen(slot_type_name);
            
            unsigned long free_space = player_name_length - slot_type_name_length;
            
            // Horizontal lines
            
            for (int j = 0; j < free_space / 2 + 1; j++)
            {
                printf("%s", position != i ? "─" : "━");
            }
            
            // Slot type
            printf("%s", slotTypeName(slots[i].type));
            
            // Horizontal lines, (+0.5 in case of dividing odd number where int would be truncated down)
            
            for (int j = 0; j < (int)((float)free_space / 2 + 0.5) + 1; j++)
            {
                printf("%s", position != i ? "─" : "━");
            }
        }
        else
        {
            // Short slot type
            printf("%s", slotTypeLetter(slots[i].type));
        }
        
        // Vertical separator unless on last slot
        if (i < (slots_count - 1))
        {
            if (position == i)
            {
                printf("┹");
            }
            else if (position == i + 1)
            {
                printf("┺");
            }
            else
            {
                printf("┴");
            }
        }
    }
    
    // Bottom right corner
    printf(position != slots_count - 1 ? "┘" : "┛");
    
    printf("\n");
}

/**
 * Move player
 *
 * @param p Pointer to player struct
 * @return Direction moved, or 0 if unable to move
 */

int Moving(Player *p)
{
    int slot = p -> slot;
    
    // Check whether the slots to the left and right are free
    bool left_empty = (slot > 0) && slots[slot - 1].player == -1;
    bool right_empty = (slot < slots_count - 1) && slots[slot + 1].player == -1;
    
    // Neither slot is free, unable to move
    if (!left_empty && !right_empty)
    {
        return 0;
    }
    
    int direction = 0;
    // If both sides are free, ask user where to go
    if (left_empty && right_empty)
    {
        char d;
        do {
            printf("Would you like to go (l)eft or (r)ight?:\n");
            scanf(" %c", &d);
        } while (d != 'l' && d != 'r');
        
        if (d == 'l')
        {
            direction = -1;
        }
        else
        { // if (d == 'r')
            direction = 1;
        }
    }
    else if (left_empty)
    {
        direction = -1;
    }
    else
    { // if (right_empty)
        direction = 1;
    }
    
    
    // Move player and update slots
    p->slot += direction;
    slots[p->slot].player = slots[slot].player;
    slots[slot].player = -1;
    
    
    if (slots[slot].type == Hill)
    {
        if (p->dexterity < 50)
        {
            p->strength -= 10;
        }
        else if (p->dexterity >= 60)
        {
            p->strength += 10;
        }
    }
    else if (slots[slot].type == City)
    {
        if (p->smartness > 60) {
            p->magic += 10;
        }
        else if (p->smartness <= 50)
        {
            p->dexterity -= 10;
        }
    }
    
    
    
    return direction;
    
    
}

/**
 * Attack player
 *
 * param p Pointer to player struct
 * return Direction moved, or 0 if unable to move
 */

void attack(Player *p)
{
    int left_player = -1, right_player = -1;
    
    // Find closest players to attack by increasing distance left/right to search until found
    for (int dist = 1; p->slot - dist > 0 || p->slot + dist < slots_count; dist++)
    {
        // If not out of bounds and there is a player to the left at dist, left player is found
        if (p->slot - dist > 0 && slots[p->slot - dist].player != -1)
        {
            left_player = slots[p->slot - dist].player;
        }
        // If not out of bounds and there is a player to the right at dist, right player is found
        if (p->slot + dist < slots_count && slots[p->slot + dist].player != -1)
        {
            right_player = slots[p->slot + dist].player;
        }
        
        // If a player was found, finish looping
        if (left_player != -1 || right_player != -1)
        {
            break;
        }
    }
    
    
    Player *target_player;
    if (left_player != -1 && right_player != -1) {
        char d;
        do {
            printf("Would you like to attack (l)eft or (r)ight?:\n");
            scanf(" %c", &d);
        } while (d != 'l' && d != 'r');
        
        if (d == 'l') {
            target_player = &players[left_player];
        } else { //if (d == 'r') {
            target_player = &players[right_player];
        }
    } else if (left_player != -1) {
        target_player = &players[left_player];
    } else if (right_player != -1) {
        target_player = &players[right_player];
    } else {
        // Shouldn't happen that no player was found, but return anyway to avoid error
        return;
    }
    
    Player *damaged_player;
    
    // Attack player
    printf("Attacking %s\n", playerPrintName((*target_player)));
    if (target_player->strength <= 70) {
        target_player->life -= 0.5 * p->strength;
        damaged_player = target_player;
    } else {
        p->life -= 0.3 * target_player->strength;
        damaged_player = p;
    }
    
    // If player has lost all health they are no longer alive
    if (damaged_player->life <= 0) {
        damaged_player->life = 0;
        damaged_player->alive = false;
        slots[damaged_player->slot].player = -1;
        damaged_player->slot = -1;
        
        players_alive--;
        printf("%s killed - RIP\n", playerPrintName((*damaged_player)));
    }
}

/**
 * Swaps the values at two pointers
 *
 * @param a Pointer a.
 * @param b Pointer b.
 * @param size Size of value pointed at by a and b.
 */

void swap(void *a, void *b, size_t size)
{
    void *temp = malloc(size);
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
    free(temp);
}

/**
 * @param max Maximum value of random integer.
 * @return Random integer [0..max].
 */

unsigned int rand_range(int min, int max)
{
    int r;
    int range = (max - min) + 1;
    int buckets = RAND_MAX / range;
    int limit = buckets * range;
    
    // Ensure fair spread of numbers by excluding some numbers in
    // the upper range of rand() that would create a biased result
    do
    {
        r = rand();
    }
    while (r >= limit);
    
    return min + (unsigned int) (r / buckets);
}

/**
 * Shuffles an array using the Fisher–Yates algorithm
 * @param array Pointer to the first element of the array to be shuffled.
 * @param num Number of elements in the array.
 * @param size Size in bytes of each element in the array.
 */

void shuffle(void *array, size_t num, size_t size)
{
    for (size_t i = 0; i < num; i++)
    {
        size_t j = i + rand_range(0, num - i - 1);
        
        if (i != j)
        {
            swap(array + (i * size), array + (j * size), size);
        }
    }
}


//This function returns the board size provided as input by the user

int getBoardSize()
{
    int boardSize;
    printf("Please enter the required size of the board\n");
    //asks the user to enter the desired board size
    scanf("%d", &boardSize);
    return boardSize;
}


//THis function takes as input the size of the board and the pointers
// to the integers representing the row and the column of the slot that
// the user want to retrieve

void getDesiredElement(int boardSize, int * row, int * col){
    printf("Please enter the column and the Row of the desired slot \n");
    printf("Note that size of the board is %d\n", boardSize);
    
    //it will cycle asking the user to insert the row
    //until the value of the desired row is >= 0 or < of the
    // size of the board
    do {
        printf("Row: ");
        scanf("%d", row);
        printf("%d\n", *row);
        if(row < 0 && *row >= boardSize)
            printf("Error: Incorrect row dimension\n");
    } while(*row < 0 && *row >= boardSize);
    
    //it will cycle asking the user to insert the column
    //until the value of the desired row is >= 0 or < of the
    // size of the board
    do {
        printf("Column: ");
        scanf("%d", col);
        printf("%d\n", *col);
        if(*col < 0 && *col >= boardSize)
            printf("Error: Incorrect column dimension\n");
    } while(*col < 0 && *col >= boardSize);
}


// This function is for creating the board.

void createBoard(int boardSize, struct slot **upLeft, struct slot **upRight, struct slot **downLeft, struct slot **downRight){
    
    //The board is represented as a pointer of pointer to slots
    //This allocates in memory the space for the pointers to each row of the board
    struct slot ** board = malloc(boardSize * sizeof(struct slot *));
    
    
    for(int i =0; i< boardSize; i++){
        //This allocates in memory the space for the slots in each row of the board
        board[i] = malloc(boardSize * sizeof(struct slot));
        
        //For each slot it sets up the row and column number
        for(int j=0;j < boardSize; j++){
            board[i][j].row = i;
            board[i][j].column = j;
        }
    }
    
    //It sets up the adjacent slots for the slots that are
    //not at the borders. These slots have 4 adjacent elements
    for(int i =1; i< boardSize-1; i++){
        for(int j=1;j < boardSize-1; j++){
            board[i][j].up = &board[i-1][j];
            board[i][j].right = &board[i][j+1];
            board[i][j].down = &board[i+1][j];
            board[i][j].left = &board[i][j-1];
        }
    }
    
    //It sets up the adjacent slots for the slots that are
    //in the first and the last row, except the slots at the edges.
    //
    for(int j = 1; j < boardSize -1; j++){
        //It sets up the adjacent slots for the slots that are in the first row.
        //These slots don't have an adjacent element on top of them
        // because they are on the first row of the board
        board[0][j].right = &board[0][j+1];
        board[0][j].left = &board[0][j-1];
        board[0][j].down = &board[1][j];
        board[0][j].up = NULL;
        
        //It sets up the adjacent slots for the slots that are in the last row.
        //These slots don't have an adjacent element on down them
        // because they are on the last row of the board
        board[boardSize - 1][j].right = &board[boardSize - 1][j+1];
        board[boardSize -1][j].left = &board[boardSize - 1][j-1];
        board[boardSize - 1][j].up = &board[boardSize - 2][j];
        board[boardSize - 1][j].down = NULL;
    }
    
    //It sets up the adjacent slots for the slots that are
    //in the first and the last column, except the slots at the edges.
    //
    for(int i = 1; i < boardSize -1; i++){
        //It sets up the adjacent slots for the slots that are in the first column.
        //These slots don't have an adjacent element on the left
        // because they are on the first column of the board
        board[i][0].right = &board[i][1];
        board[i][0].up = &board[i-1][0];
        board[i][0].down = &board[i+1][0];
        board[i][0].left = NULL;
        
        //It sets up the adjacent slots for the slots that are in the last column.
        //These slots don't have an adjacent element on the right
        // because they are on the last column of the board
        board[i][boardSize-1].left = &board[i][boardSize-2];
        board[i][boardSize -1].up = &board[i -1][boardSize-1];
        board[i][boardSize -1].down = &board[i+1][boardSize -1];
        board[i][boardSize -1].right = NULL;
    }
    
    
    //It sets up the adjacent slots for the slot at position (0,0).
    //This only has only 2 adjacent slots: right and down
    board[0][0].right = &board[0][1];
    board[0][0].down = &board[1][0];
    
    //It sets up the adjacent slots for the slot at position (0,boardSize -1).
    //This only has only 2 adjacent slots: left and down
    board[0][boardSize-1].left = &board[0][boardSize-2];
    board[0][boardSize -1].down = &board[1][boardSize -1];
    
    //It sets up the adjacent slots for the slot at position (boarSize -1 ,0).
    //This only has only 2 adjacent slots: up and right
    board[boardSize -1][0].right = &board[boardSize -1][1];
    board[boardSize -1][0].up = &board[boardSize -2][0];
    
    //It sets up the adjacent slots for the slot at position (boarSize -1 ,boardSize-1).
    //This only has only 2 adjacent slots: up and left
    board[boardSize - 1][boardSize-1].up = &board[boardSize-2][boardSize-1];
    board[boardSize - 1][boardSize -1].left = &board[boardSize -1][boardSize -2];
    
    
    
    
    
    
    
    //assigns a pointer to slot at position (0, 0)
    *upLeft = &board[0][0];
    //assigns pointer of pointer to slot at position (0, boardSize -1)
    *upRight = &board[0][boardSize -1];
    //assigns pointer of pointer to slot at position ( boardSize -1,)
    *downLeft = &board[boardSize -1][0];
    //assigns pointer of pointer to slot at position (boardSize -1, boardSize -1)
    *downRight = &board[boardSize -1][boardSize -1];
}


/*
 * This function traverses the board to find a slot at a predefined
 * position (row, column). This function returns a pointer to the desired slot
 * Parameters:
 * 	row: the row in which the desired slot is located
 * 	column: the column in which the desired slot is located
 * 	initialSlot: the slot from which the slot search should start
 */

struct slot * reachDesiredElement(int row, int column, struct slot * initialSlot)
{
    
    bool found = false;
    //current slot
    struct slot * currentSlot = initialSlot;
    
    printf("\nFunction reachDesiredElement invoked\n");
    
    //prints the column and the row of the initial slot from which the search starts
    printf("Initial slot (%d, %d) -> \n",initialSlot->row,initialSlot->column);
    
    
    //while the slot is not found
    while(found == false)
    {
        
        
        //if the row of the current slot is > of the row of the desired slot,
        //we move up
        
        if(currentSlot->row > row)
        {
            //the current slot now points to the slot one row up
            currentSlot = currentSlot->up;
            //prints the column and the row of the current slot
            printf("Current slot (%d, %d) -> \n",currentSlot->row,currentSlot->column);
        }
        
        //if the row of the current slot is < of the row of the desired slot,
        //we move down
        
        if(currentSlot->row < row)
        {
            //the current slot now points to the slot one row down
            currentSlot = currentSlot->down;
            //prints the row and the column of the current slot
            printf("Current slot (%d, %d) -> \n",currentSlot->row,currentSlot->column);
            
        }
        
        //if the column of the current slot is > of the column of the desired slot,
        //we move left
        
        if(currentSlot->column > column)
        {
            
            //the current slot now points to the slot one column left
            currentSlot = currentSlot->left;
            //prints the row and the column of the current slot
            printf("Current slot (%d, %d) -> \n",currentSlot->row,currentSlot->column);
        }
        
        //if the column of the current slot is < of the column of the desired slot,
        //we move right
        
        if(currentSlot->column < column)
        {
            
            //the current slot now points to the slot one column right
            currentSlot = currentSlot->right;
            //prints the row and the column of the current slot
            printf("Current slot (%d, %d) -> \n",currentSlot->row,currentSlot->column);
            
        }
        
        //if the current slot is at a column and a row equal to the desired column and row, respectively
        // we found the slot
        
        if(currentSlot->column == column && currentSlot->row == row){
            printf("Found\n");
            found = true;
            
        }
        
    }
    //returns the found slot
    return currentSlot;
}


/*
 * The recursive function that traverses the board to find the slots at a predefined
 * distance from the current slot and place them in foundSlots.
 * Parameters:
 * 	reqDist: the required distance from the starting slot
 * 	currDist: the distance of the current slot from the starting slot
 * 	currSlot: a pointer to the current slot that is traversed
 * 	foundSlots: the array of slots that are at a required distance from the starting slot
 * 	count: pointer to an integer representing the number of slots that are found to be at a required distance from the starting slot
 * 	explored: matrix indicating for each slot at row x and column y has been traversed (true) or not (false)
 */
void findSlots(int reqDist, int currDist,  struct slot * currSlot, struct slot * foundSlots, int * count,  bool explored[BOARD_SIZE][BOARD_SIZE])
{
    
    
    
    //The base case: the current slot is at the required distance from the starting slot
    
    if(currDist == reqDist)
    {
        //the current slot was not explored
        
        if(explored[currSlot->row][currSlot->column] == false)
        {
            //The next availbale position (indicated by count) at foundSlots points to the current slot
            *(foundSlots + *count) = *currSlot;
            //the counter is incremented
            (*count)++;
            //the matrix of the explored slots set to true the element at the row and column of the current slot
            explored[currSlot->row][currSlot->column] = true;
        }
    }
    
    //The recursive call: the current slot is at a distance that is less than the required distance (more slots still have to be traversed)
    else
    {
        //if the current slot has the up slot != NULL (i.e. the slot is not in the first row)
        
        if(currSlot->up != NULL)
        {
            //invokes function find slots incrementing the current Distance (currDist) and setting the current slot to the up slot
            findSlots(reqDist, currDist +1,  currSlot->up, foundSlots, count, explored);
        }
        
        //if the current slot has the right slot != NULL (i.e. the slot is not in the last column)
        
        if(currSlot->right != NULL)
        {
            //invokes function find slots incrementing the current Distance (currDist) and setting the current slot to the right slot
            findSlots(reqDist, currDist +1,  currSlot->right, foundSlots, count, explored);
        }
        
        //if the current slot has the down slot != NULL (i.e. the slot is not in the last row)
        
        if(currSlot->down != NULL)
        {
            //invokes function find slots incrementing the current Distance (currDist) and setting the current slot to the down slot
            findSlots(reqDist, currDist +1,  currSlot->down, foundSlots, count, explored);
        }
        //if the current slot has the left slot != NULL (i.e. the slot is not in the first column)
        
        if(currSlot->left != NULL)
        { 
            //invokes function find slots incrementing the current Distance (currDist) and setting the current slot to the left slot
            findSlots(reqDist, currDist +1,  currSlot->left, foundSlots, count, explored);
        }
       
    }
    
    
}


