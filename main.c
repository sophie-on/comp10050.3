#include <stdio.h>
#include <mem.h>
#include <malloc.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

#include "main.h"

char *slotTypeName(SlotType s) {
    switch (s) {
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

char *slotTypeLetter(SlotType s) {
    switch (s) {
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

char *playerTypeName(PlayerType t) {
    switch (t) {
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

char *playerPrintName(Player p) {
    char *printName = (char *) malloc(64 * sizeof(char));
    sprintf(printName, "%s (%s) [%dHP]", p.name, playerTypeName(p.type), p.life);
    return printName;
}

Player *players;
size_t players_count;
size_t players_alive;

Slot *slots;
size_t slots_count;

int main(void) {
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
    do {
        printf("Would you like to use (d)emo mode or (r)egular mode?:\n");
        scanf(" %c", &d);
    } while (d != 'd' && d != 'r');
    demo_mode = d == 'd';

    if (!demo_mode) {
        do {
            printf("Enter number of players [2..6]:\n");
            scanf("%u", &players_count);
        } while (players_count < 2 || players_count > 6);
    } else {
        players_count = 4;
    }

    // Allocate players array
    players = (Player *) malloc(sizeof(Player) * players_count);

    if (!demo_mode) {
        // Print list of possible player types
        printf("Player types: (e)lf, (h)uman, (o)gre, (w)izard\n");

        // Index at which to insert next player
        int players_index = 0;
        while (players_index < players_count) {
            // Allocate memory for storing player name
            char *name = (char *) malloc(64 * sizeof(char));
            PlayerType type;

            char t;
            printf("Enter type and player name:\n");
            scanf(" %c %[^\n]", &t, name);

            // Convert character to player type
            if (t == 'e') {
                type = Elf;
            } else if (t == 'h') {
                type = Human;
            } else if (t == 'o') {
                type = Ogre;
            } else if (t == 'w') {
                type = Wizard;
            } else {
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
    } else {
        // Default names and types
        char *demo_names[] = {"John Sheekey", "Mark Dukes", "Henry McLoughlin", "Liliana Pasquale"};
        PlayerType demo_types[] = {Human, Ogre, Wizard, Elf};

        for (int i = 0; i < players_count; i++) {
            Player p;
            p.type = demo_types[i];
            p.name = demo_names[i];

            players[i] = p;
        }
    }

    // Fill in initial skill points
    for (int i = 0; i < players_count; i++) {
        // Players start alive at 100 health
        players[i].alive = true;
        players[i].life = 100;

        // Different player types have different starting skill points
        if (players[i].type == Elf) {
            players[i].luck = rand_range(60, 100);
            players[i].smartness = rand_range(70, 100);
            players[i].strength = rand_range(1, 50);
            players[i].magic = rand_range(51, 79);
            players[i].dexterity = rand_range(1, 100);
        } else if (players[i].type == Human) {
            // Total skill points must be less than 300, and individual skill points must be > 0
            // Therefore the range of the total values is 5..299
            // Select a random value for total
            int total = rand_range(5, 299);

            // Choose 5 random values, and subtract from the remaining total points each time
            // Values are between 1 and 100 or the remaining total whichever is smaller
            // If the total is smaller, a point is reserved for each remaining skill
            int values[5];
            for (int j = 0; j < 5; j++) {
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
        } else if (players[i].type == Ogre) {
            players[i].strength = rand_range(80, 100);
            players[i].dexterity = rand_range(80, 100);
            players[i].magic = 0;
            // Choose total of luck and smartness first
            // Then insert luck, and assign remaining total to smartness
            int sum = rand_range(0, 50);
            players[i].luck = rand_range(0, sum);
            players[i].smartness = sum - players[i].luck;
        } else if (players[i].type == Wizard) {
            players[i].luck = rand_range(50, 100);
            players[i].smartness = rand_range(70, 100);
            players[i].strength = rand_range(1, 20);
            players[i].magic = rand_range(80, 100);
            players[i].dexterity = (1, 100);
        }
    }

   /* if (!demo_mode) {
        printf("Enter number of slots(max = 20): \n");//<-------*
        scanf("%d", &slots_count);
    } else {
        slots_count = 12;
    }

    // Allocate slots array
    slots = (Slot *) malloc(sizeof(Slot) * slots_count);

    // Assign random slot types
    for (int i = 0; i < slots_count; i++) {
        int r = rand_range(0, 2);

        if (r == 0) {
            slots[i].type = Ground;
        } else if (r == 1) {
            slots[i].type = City;
        } else if (r == 2) {
            slots[i].type = Hill;
        }
    }

    // Distribute players randomly among slots by first inserting them
    // at slot positions [0..players_count] and then shuffling the slots
    for (int i = 0; i < slots_count; i++) {
        slots[i].player = i < players_count ? i : -1;
    }
    shuffle(slots, (size_t) slots_count, sizeof(Slot));

    // Once slots have been shuffled, update players with their current slot
    for (int i = 0; i < slots_count; i++) {
        if (slots[i].player >= 0) {
            players[slots[i].player].slot = i;
        }
    }*/

    // At the beginning, all players are alive
    players_alive = players_count;

    // Continue main program loop as long as there is more than 1 player alive
    int round = 1;
    while (players_alive > 1) {
        printf("╔══════════╗\n");
        printf("║ Round %2d ║\n", round);
        printf("╚══════════╝\n");

        print_players();

        // Loop through each player
        for (int i = 0; i < players_count; i++) {
            Player p = players[i];

            // If player is not alive continue to next player
            if (!p.alive) continue;

            // Print player name in rounded box
            char *print_name = playerPrintName(p);
            int print_length = strlen(print_name) + 2;
            printf("╭──────────┬"); for (int x = 0; x < print_length; x++) printf("─"); printf("╮\n");
            printf("│ Player %d │ %s │\n", i + 1, print_name);
            printf("╰──────────┴"); for (int x = 0; x < print_length; x++) printf("─"); printf("╯\n");

           /* // Print current slots
            print_slots(players[i].slot, 0);*/

            char a;
            do {
                printf("Would you like to (m)ove or (a)ttack?:\n");
                scanf(" %c", &a);
            } while (a != 'a' && a != 'm');

            int m = 0;
            if (a == 'a') {
                attack(&players[i]);
            } else { // if (a == 'm') {
                m = move(&players[i]);

                // If the player was unable to move, force attack
                if (m == 0) {
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

        if (players_alive > 1) {
            // Clear screen for next round
            system("cls");
        }

        round++;
    }

    // Find winning player
	for (int i = 0; i < players_count; i++) {
		if (players[i].alive == true) {
			printf("%s has won after %d rounds!\n", playerPrintName(players[i]), round);
            break;
		}
	}
}

void await_input() {
    printf("Press enter to continue...\n");
    fflush(stdin);
    while(getchar() != '\n');
}

/**
 * Prints players information
 */
void print_players() {
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
 /*
void print_slots(int position, int direction_movement_to) {
    /*
     * Row 1
     */

    // Top left corner
   /* printf(position != 0 ? "┌" : "┏");

    // Loop through slots
    for (int i = 0; i < slots_count; i++) {
        // Slot contains a player
        if (slots[i].player != -1) {
            Player p = players[slots[i].player];

            char *player_print_name = playerPrintName(p);
            int player_name_length = strlen(player_print_name);

            // Account for the fact that player name will be printed on row 2 by printing more horizontal lines
            for (int j = 0; j < player_name_length + 2; j++) {
                printf("%s", position != i ? "─" : "━");
            }
        } else {
            // No player, print a single horizontal line
            printf("%s", position != i ? "─" : "━");
        }

        // Vertical separator unless on last slot
        if (i < (slots_count - 1)) {
            if (position == i) {
                printf("┱");
            } else if (position == i + 1) {
                printf("┲");
            } else {
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
    /*printf(position != 0 ? "│" : "┃");

    // Loop through slots
    for (int i = 0; i < slots_count; i++) {
        // Slot contains a player
        if (slots[i].player != -1) {
            Player p = players[slots[i].player];

            printf(" %s ", playerPrintName(p));
        } else {
            // If player at adjacent position just moved from this slot, print arrow
            if (i == position - direction_movement_to) {
                printf(direction_movement_to == -1 ? "←" : "→");
            } else {
                printf(" ");
            }
        }

        // Vertical separator unless on last slot
        if (i < (slots_count - 1)) {
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
   /* printf(position != 0 ? "└" : "┗");

    // Loop through slots
    for (int i = 0; i < slots_count; i++) {
        // Slot contains a player
        if (slots[i].player != -1) {
            Player p = players[slots[i].player];

            // Calculations for space filling horizontal lines
            char *player_print_name = playerPrintName(p);
            char *slot_type_name = slotTypeName(slots[i].type);

            int player_name_length = strlen(player_print_name);
            int slot_type_name_length = strlen(slot_type_name);

            int free_space = player_name_length - slot_type_name_length;

            // Horizontal lines
            for (int j = 0; j < free_space / 2 + 1; j++) {
                printf("%s", position != i ? "─" : "━");
            }

            // Slot type
            printf("%s", slotTypeName(slots[i].type));

            // Horizontal lines, (+0.5 in case of dividing odd number where int would be truncated down)
            for (int j = 0; j < (int)((float)free_space / 2 + 0.5) + 1; j++) {
                printf("%s", position != i ? "─" : "━");
            }
        } else {
            // Short slot type
            printf("%s", slotTypeLetter(slots[i].type));
        }

        // Vertical separator unless on last slot
        if (i < (slots_count - 1)) {
            if (position == i) {
                printf("┹");
            } else if (position == i + 1) {
                printf("┺");
            } else {
                printf("┴");
            }
        }
    }

    // Bottom right corner
    printf(position != slots_count - 1 ? "┘" : "┛");

    printf("\n"); */
}

/**
 * Move player
 *
 * @param p Pointer to player struct
 * @return Direction moved, or 0 if unable to move
 */
int move(Player *p) {
    int slot = p->slot;

    // Check whether the slots to the left and right are free
    bool left_empty = (slot > 0) && slots[slot - 1].player == -1;
    bool right_empty = (slot < slots_count - 1) && slots[slot + 1].player == -1;

    // Neither slot is free, unable to move
    if (!left_empty && !right_empty) {
        return 0;
    }

    int direction = 0;
    // If both sides are free, ask user where to go
    if (left_empty && right_empty) {
        char d;
        do {
            printf("Would you like to go (l)eft or (r)ight?:\n");
            scanf(" %c", &d);
        } while (d != 'l' && d != 'r');

        if (d == 'l') {
            direction = -1;
        } else { // if (d == 'r') {
            direction = 1;
        }
    } else if (left_empty) {
        direction = -1;
    } else { // if (right_empty) {
        direction = 1;
    }
	

    // Move player and update slots
    p->slot += direction;
    slots[p->slot].player = slots[slot].player;
    slots[slot].player = -1;


    if (slots[slot].type == Hill) {
        if (p->dexterity < 50) {
            p->strength -= 10;
        } else if (p->dexterity >= 60) {
            p->strength += 10;
        }
    } else if (slots[slot].type == City) {
        if (p->smartness > 60) {
            p->magic += 10;
        } else if (p->smartness <= 50) {
            p->dexterity -= 10;
        }
    }
	
	
	
	return direction;
	
	
}

 /**
 * Attack player
 *
 * @param p Pointer to player struct
 * @return Direction moved, or 0 if unable to move
 */
void attack(Player *p) {
    int left_player = -1, right_player = -1;

    // Find closest players to attack by increasing distance left/right to search until found
    for (int dist = 1; p->slot - dist > 0 || p->slot + dist < slots_count; dist++) {
        // If not out of bounds and there is a player to the left at dist, left player is found
        if (p->slot - dist > 0 && slots[p->slot - dist].player != -1) {
            left_player = slots[p->slot - dist].player;
        }
        // If not out of bounds and there is a player to the right at dist, right player is found
        if (p->slot + dist < slots_count && slots[p->slot + dist].player != -1) {
            right_player = slots[p->slot + dist].player;
        }

        // If a player was found, finish looping
        if (left_player != -1 || right_player != -1) {
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
void swap(void *a, void *b, size_t size) {
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
unsigned int rand_range(int min, int max) {
    int r;
    int range = (max - min) + 1;
    int buckets = RAND_MAX / range;
    int limit = buckets * range;

    // Ensure fair spread of numbers by excluding some numbers in
    // the upper range of rand() that would create a biased result
    do {
        r = rand();
    } while (r >= limit);

    return min + (unsigned int) (r / buckets);
}

/**
 * Shuffles an array using the Fisher–Yates algorithm
 * @param array Pointer to the first element of the array to be shuffled.
 * @param num Number of elements in the array.
 * @param size Size in bytes of each element in the array.
 */
void shuffle(void *array, size_t num, size_t size) {
    for (size_t i = 0; i < num; i++) {
        size_t j = i + rand_range(0, num - i - 1);

        if (i != j) {
            swap(array + (i * size), array + (j * size), size);
        }
    }
}