

#ifndef COMP10050_MAIN_H
#define COMP10050_MAIN_H

typedef enum SlotType {
    Ground, City, Hill
} SlotType;

typedef struct Slot {
    SlotType type;
    int player;
} Slot;

typedef enum PlayerType {
    Elf, Human, Ogre, Wizard
} PlayerType;

typedef int bool;
enum {
    false, true};

typedef struct Player {
    char *name;
    enum PlayerType type;
    int slot;
    bool alive;
    int life;
    int smartness;
    int strength;
    int magic;
    int luck;
    int dexterity;
} Player;

void swap(void *a, void *b, size_t size);

unsigned int rand_range(int min, int max);

void shuffle(void *array, size_t num, size_t size);

void await_input();
int Moving(Player *p);
void attack(Player *p);

void print_players();
void print_slots(int position, int direction_movement_from);

#endif //COMP10050_MAIN_H




