/* COP 3502C PA1
This program is written by: Quyen Le */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Represents a player in the system
typedef struct {
    char *playerName;//dynamically allocated name of the player without wasting space
    int totalScore; // cumulative score across all puzzles
} Player;

// Represents one puzzle instance (e.g., Wordle 751, Crossword 202)
typedef struct {
    char *puzzleType; // points to an already allocated puzzle type string (Wordle, Crossword, etc.). No malloc/calloc for this property
    int puzzleNo; // ID number of the puzzle (e.g., 751, 202)
    Player** players; // dynamic array of pointers to players who played this puzzle
    int *scores; // dynamic array of scores corresponding to each player (index-aligned with players)
    int playerCount; // number of players who played this puzzle
} Puzzle;

// Represents the archive of all puzzle instances
typedef struct {
    Puzzle* puzzles; // dynamic array of puzzles
    int puzzleCount; // number of puzzles stored
} Archive;

char** readPuzzleTypes(int *countOut);
Player *createPlayer(char *name);
Archive *createArchive(int puzzleCount);
Player** readPlayerPool(int *playerCount);
char* getPuzzleTypePtr(char **puzzleTypes, int count, char *type);
Player* getPlayerPtrByName(Player **playerPool, int playerCount, char *playerName);
void printBestScorer(Puzzle *puzzle);
void printMaxTotalScorer(Archive *archive);
void freePlayerPool(Player **pool, int count);
void freeArchive(Archive *archive);

int main() {
    char** puzzleTypes; // to store array of dynamically allocated strings for puzzle types (e.g., {"Wordle", "Crossword", "Connections"})
    int puzzleTypeCount; // number of puzzle types

    Player** playerPool;
    int playerPoolCount = 0;

    puzzleTypes = readPuzzleTypes(&puzzleTypeCount);
    playerPool = readPlayerPool(&playerPoolCount);

    int numPuzzles = 0;
    scanf("%d", &numPuzzles);

    Archive *archive = createArchive(numPuzzles);

    for(int i = 0; i < numPuzzles; i++) {
        char puzzle[21];
        int id = 0, playerCount = 0;

        scanf("%s %d %d", puzzle, &id, &playerCount);

        // Assign puzzle type pointer, puzzle id, and player count
        archive->puzzles[i].puzzleType = getPuzzleTypePtr(puzzleTypes, puzzleTypeCount, puzzle);
        archive->puzzles[i].puzzleNo = id;
        archive->puzzles[i].playerCount = playerCount;

        archive->puzzles[i].players = malloc(playerCount * sizeof(Player*));
        archive->puzzles[i].scores = malloc(playerCount * sizeof(int));
        
        for (int j = 0; j < playerCount; j++) {
            char name[21];
            int score;
            scanf("%s %d", name, &score);
            
            // Get player pointer from pool and assign to puzzle's players array
            Player *player = getPlayerPtrByName(playerPool, playerPoolCount, name);
            archive->puzzles[i].players[j] = player;
            archive->puzzles[i].scores[j] = score;
                
            player->totalScore += score;
        }
    }

    // Read number of queries and process each query type
    int numQueries = 0;
    scanf("%d", &numQueries);

    for(int i = 0; i < numQueries; i++) {
        int queryType = 0;
        scanf("%d", &queryType);

        if(queryType == 1) {
            printMaxTotalScorer(archive);
        } else if(queryType == 2) {
            printf("Top scorer per puzzle:\n");
            for(int j = 0; j < archive->puzzleCount; j++) {
                printBestScorer(&archive->puzzles[j]);
            }
        }
    }

    freePlayerPool(playerPool, playerPoolCount);
    freeArchive(archive);

    return 0;
}

// Allocates memory for an array of puzzle types and reads them from input
char** readPuzzleTypes(int *countOut) {
    scanf("%d", countOut);
    char **puzzleTypes = malloc(*countOut * sizeof(char*));
    
    for(int i = 0; i < *countOut; i++) {
        char puzzle[21];
        scanf("%s", puzzle);
        int len = strlen(puzzle)+1;
        puzzleTypes[i] = malloc(len * sizeof(char));
        strcpy(puzzleTypes[i], puzzle);
    }

    return puzzleTypes;
}

// Allocates memory for a Player structure with the player's name stored and other properties initialized
Player *createPlayer(char *name) {
    Player *onePlayer = malloc(sizeof(Player));
    int len = strlen(name) + 1;
    onePlayer->playerName = malloc(len * sizeof(char));
    strcpy(onePlayer->playerName, name);
    onePlayer->totalScore = 0;

    return onePlayer;
}

// Allocates memory for an Archive structure and its puzzles array
Archive *createArchive(int puzzleCount) {
    Archive *newArchive = malloc(sizeof(Archive));
    
    newArchive->puzzles = malloc(puzzleCount * sizeof(Puzzle));
    newArchive->puzzleCount = puzzleCount;
    return newArchive;
}

// Uses the createPlayer function to create each player and store in the pool then returns the pool
Player** readPlayerPool(int *playerCount) {
    scanf("%d", playerCount);
    Player **playerPool = malloc(*playerCount * sizeof(Player*));

    for(int i = 0; i < *playerCount; i++) {
        char name[21];
        scanf("%s", name);
        playerPool[i] = createPlayer(name);
    }

    return playerPool;
}

// Returns a pointer to the matching puzzle type string, or NULL if not found
char* getPuzzleTypePtr(char **puzzleTypes, int count, char *type) {
    for(int i = 0; i < count; i++) {
        if(strcmp(puzzleTypes[i], type) == 0) {
            return puzzleTypes[i];
        }
    }
    return NULL;
}

// Returns a pointer to the matching Player structure, or NULL if not found
Player* getPlayerPtrByName(Player **playerPool, int playerCount, char *playerName) {
    for(int i = 0; i < playerCount; i++) {
        if(strcmp(playerPool[i]->playerName, playerName) == 0) {
            return playerPool[i];
        }
    }
    return NULL;
}

// Query Type 2
// Displays the name and score of the top scorer for the specified puzzle
void printBestScorer(Puzzle *puzzle) {
    if(puzzle->playerCount == 0) {
        printf("%s#%d No player yet for this puzzle\n", puzzle->puzzleType, puzzle->puzzleNo);
        return;
    }
    
    int bestScore = 0;
    int bestPlayerIndex = 0;

    // Traverse the scores to find the highest score and corresponding player index
    for(int i = 0; i < puzzle->playerCount; i++) {
        if(puzzle->scores[i] > bestScore) {
            bestScore = puzzle->scores[i];
            bestPlayerIndex = i;
        }
    }

    printf("%s#%d %s %d\n", puzzle->puzzleType, puzzle->puzzleNo, puzzle->players[bestPlayerIndex]->playerName, bestScore);
}

// Query Type 1
// Displays the player with the highest total score
void printMaxTotalScorer(Archive *archive) {
    char *topPlayerName = NULL;
    int maxTotalScore = 0;

    // Traverse all puzzles and their players to find the player with the highest total score
    for(int i = 0; i < archive->puzzleCount; i++) {
        for(int j = 0; j < archive->puzzles[i].playerCount; j++) {
            // Check if the current player's total score is higher than the max found so far
            if(archive->puzzles[i].players[j]->totalScore > maxTotalScore) {
                maxTotalScore = archive->puzzles[i].players[j]->totalScore;
                topPlayerName = archive->puzzles[i].players[j]->playerName;
            }
        }
    }

    printf("Top player: %s with total score %d\n", topPlayerName, maxTotalScore);
}

// Last two functions: Free all dynamically allocated memory for player pool and archive
void freePlayerPool(Player **pool, int count) {
    for(int i = 0; i < count; i++) {
        free(pool[i]->playerName);
        free(pool[i]);
    }
    free(pool);
}

void freeArchive(Archive *archive) {
    for(int i = 0; i < archive->puzzleCount; i++) {
        free(archive->puzzles[i].players);
        free(archive->puzzles[i].scores);
    }
    free(archive->puzzles);
    free(archive);
}