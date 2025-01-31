
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Board.h"
#include "List.h"
#include "Set.h"

const char alphabet[26] = "abcdefghijklmnopqrstuvwxyz";

// Probability of letters in english words in percentage

const float probaLetters[26] = {7.8, 2.0, 4.0, 3.8, 11.0, 1.4, 3.0, 2.3, 8.6,
                                0.21, 0.97, 5.3, 2.7, 7.2, 6.1, 2.8, 0.19, 7.3,
                                8.7, 6.7, 3.3, 1.0, 0.91, 0.27, 1.6, 0.44};

#define MIN(a, b) a < b ? a : b

struct Board_t
{
    size_t size;
    char **grid;
    bool **flag;
};

/* Prototypes */

static void terminate(char *m);
static char getRandomLetter(void);
static void boardInitFlag(Board *board);
static bool searchDirection(Board *board, const char *word, int len, int r, int c, int incr, int incc);

/* static functions */

/**
 * @brief Terminate the program and display a message
 *
 * @param m
 */
static void terminate(char *m)
{
    fprintf(stderr, "%s\n", m);
    exit(EXIT_FAILURE);
}

/**
 * @brief Generate a random letter according to the distribution in probaLetters
 *
 * @return char
 */
static char getRandomLetter(void)
{

    float r = (float)rand() / (float)(RAND_MAX / 99.59);
    float cumsum = probaLetters[0];
    int i = 0;
    while (i < 26 && cumsum < r)
    {
        i++;
        cumsum += probaLetters[i];
    }
    if (i >= 26)
        printf("Erreur\n");
    return alphabet[i];
}

/**
 * @brief Initialize the flags to false.
 *
 * @param board
 */
static void boardInitFlag(Board *board)
{
    for (size_t r = 0; r < board->size; r++)
        for (size_t c = 0; c < board->size; c++)
            board->flag[r][c] = false;
}

/**
 * @brief return true if the word is found at position (r,c) in the direction
 *        obtained by incrementing row and column by incr and incc respetively
 *
 * @param board       a pointer to a board
 * @param word        the word to be found
 * @param len         the length of the word
 * @param r           the starting row
 * @param c           the starting column
 * @param incr        the row increment
 * @param incc        the column increment
 * @return true       if the word is found
 * @return false      otherwise
 */
static bool searchDirection(Board *board, const char *word, int len, int r, int c, int incr, int incc)
{
    int i = 0;
    int sr = r;
    int sc = c;
    while (i < len && board->grid[sr][sc] == word[i])
    {
        i++;
        sr += incr;
        sc += incc;
    }

    if (i == len)
    {
        i = 0;
        while (i < len)
        {
            board->flag[r][c] = true;
            r += incr;
            c += incc;
            i++;
        }
        return true;
    }

    return false;
}

/* header functions */

Board *boardCreate(size_t size, const char *letters)
{
    if (letters != NULL && strlen(letters) < size * size)
        terminate("createBoard: letters does not have the correct size.");

    Board *board = malloc(sizeof(Board));

    if (board == NULL)
        terminate("createBoard: allocation failed.");

    board->size = size;
    board->grid = malloc(size * sizeof(char *));
    if (board->grid == NULL)
        terminate("createBoard: allocation failed.");
    board->flag = malloc(size * sizeof(bool *));
    if (board->flag == NULL)
        terminate("createBoard: allocation failed.");

    size_t i = 0;

    for (size_t r = 0; r < size; r++)
    {
        board->grid[r] = malloc(size * sizeof(char));
        if (board->grid[r] == NULL)
            terminate("createBoard: allocation failed.");
        board->flag[r] = malloc(size * sizeof(bool));
        if (board->flag[r] == NULL)
            terminate("createBoard: allocation failed.");

        for (size_t c = 0; c < size; c++)
        {
            if (letters != NULL)
                board->grid[r][c] = letters[i++];
            else
                board->grid[r][c] = getRandomLetter();

            board->flag[r][c] = false;
        }
    }

    return board;
}

void boardFree(Board *board)
{
    if (board == NULL)
        return;

    for (size_t r = 0; r < board->size; r++)
    {
        free(board->grid[r]);
        free(board->flag[r]);
    }
    free(board->grid);
    free(board->flag);
    free(board);
}

bool boardContainsWord(Board *board, const char *word)
{
    if (board == NULL || word == NULL)
        return false;

    int len = strlen(word);
    int size = board->size;
    boardInitFlag(board);

    for (int r = 0; r < size; r++)
    {
        for (int c = 0; c < size; c++)
        {
            // vertical
            if (board->grid[r][c] == word[0])
            {
                if (size - r >= len)
                {
                    if (searchDirection(board, word, len, r, c, 1, 0)) // down
                        return true;
                    if (size - c >= len && searchDirection(board, word, len, r, c, 1, 1)) // down - right
                        return true;
                    if (c + 1 >= len && searchDirection(board, word, len, r, c, 1, -1)) // down - left
                        return true;
                }
                if (r + 1 >= len)
                {
                    if (searchDirection(board, word, len, r, c, -1, 0)) // up
                        return true;
                    if (size - c >= len && searchDirection(board, word, len, r, c, -1, 1)) // up - right
                        return true;
                    if (c + 1 >= len && searchDirection(board, word, len, r, c, -1, -1)) // up - left
                        return true;
                }
                if (size - c >= len && searchDirection(board, word, len, r, c, 0, 1)) // right
                    return true;
                if (c + 1 >= len && searchDirection(board, word, len, r, c, 0, -1)) // left
                    return true;
            }
        }
    }

    return false;
}

void boardDisplay(Board *board)
{
    if (board != NULL && board->size <= 20)
    {
        printf("\t");
        for (size_t i = 0; i < board->size; i++)
            printf("....");
        printf(".\n");
        for (size_t i = 0; i < board->size; i++)
        {
            printf("\t");

            for (size_t j = 0; j < board->size; j++)
                if (board->flag[i][j])
                    printf(":[%c]", board->grid[i][j]);
                else
                    printf(": %c ", board->grid[i][j]);
            printf(":\n");
            printf("\t");
            for (size_t j = 0; j < board->size; j++)
                printf(":...");
            printf(":\n");
        }
        printf("\n");
    }
    else
    {
        printf("board is too large to be printed.\n");
    }
}


/* Student code starts here */

/* Prototypes */

static char *duplicate_string(const char *str);
static bool isInBoard(int r, int c, int size);
static bool getWord(Board *board, char *word, int r, int c, int incr, int incc);
static void addFoundPrefixes(Board *board, Set* set, Set* filledSet, List* wordsList, int r, int c, int incr, int incc);

/* static functions */


/**
 * @brief Duplicate a string
 * ADOPTED FROM  SET_BST.c
 *
 * @param str
 * @return char*
 */
static char *duplicate_string(const char *str)
{
    char *copy = malloc(strlen(str) + 1);
    if (!copy)
        return NULL;
    memcpy(copy, str, strlen(str) + 1);
    return copy;
}

/**
 * @brief Checks if a given position is valid in the grid (board)
 *
 * @param r row
 * @param c column
 * @param size size of the grid
 * 
 * @return bool : true if the pos is valid
 *                false itherwise
 */
static bool isInBoard(int r, int c, int size){
    return r >= 0 && r < size
        && c >= 0 && c < size;
}

/**
 * @brief return true if the word is found at position (r,c) in the direction
 *        obtained by incrementing row and column by incr and incc respetively
 *
 * @param board       a pointer to a board
 * @param word        the word to be found
 * @param r           the starting row
 * @param c           the starting column
 * @param incr        the row increment
 * @param incc        the column increment
 * @return true       if the word is found
 * @return false      otherwise
 */
static bool getWord(Board *board, char *word, int r, int c, int incr, int incc){
    int i = 0;
    int sr = r;
    int sc = c;
    size_t n = board->size;
    if (isInBoard(sr + incr, sc + incc, n)){

        word[i] = board->grid[sr][sc];
        while (isInBoard(sr + incr, sc + incc, n)){ 

            sr += incr;
            sc += incc;
            i++;
            word[i] = board->grid[sr][sc];
        }
        word[i+1] = '\0';
        return true;
    }
    return false;
}


/**
 * @brief Searches for a word forming a line on the grid, and then adds to a list all its prefixes found in the set.
 * 
 * @param board a pointer to a board
 * @param set a pointer to a set
 * @param filledSet a pointer to a set (used for checking duplicates)
 * @param wordsList a pointer to a list (will contain found prefixes in the set)
 * @param r starting row 
 * @param c starting column
 * @param incr the row increment
 * @param incc the column increment
 *
 */
static void addFoundPrefixes(Board *board, Set* set, Set* filledSet, List* wordsList, int r, int c, int incr, int incc){
    size_t n = board->size;
    char *word = malloc((sizeof(char) * n + 1));
    if (!word){
        boardFree(board);
        setFree(set);
        setFree(filledSet);
        listFree(wordsList, true);
        terminate("Failed to allocate memory for word");
    }

    if (getWord(board, word, r, c, incr, incc)){

        List *foundPrefixes = setGetAllStringPrefixes(set, word);

        if (!foundPrefixes){
            boardFree(board);
            setFree(set);
            setFree(filledSet);
            listFree(wordsList, true);
            listFree(foundPrefixes, true);
            terminate("Failed to get prefixes of the word");
        }
        for (LNode *p = foundPrefixes->head; p != NULL; p = p->next){

            char *copy = duplicate_string(p->value);
            if (!copy){
                boardFree(board);
                setFree(set);
                setFree(filledSet);
                listFree(wordsList, true);
                listFree(foundPrefixes, true);
                terminate("Failed to duplicate string");
            }

            if (setInsert(filledSet, copy) == 1){

                if (!listInsertLast(wordsList, copy)){ 
                    free(copy);
                    boardFree(board);
                    setFree(set);
                    setFree(filledSet);
                    listFree(wordsList, true);
                    listFree(foundPrefixes, true);
                    terminate("Failed to add matching word to list");
                }
            }
            else {
                free(copy);
            }
        }

        listFree(foundPrefixes, true);
    }
    free(word);
}

List *boardGetAllWordsFromSet(Board *board, Set *set)
{
    Set *filledSet = setCreateEmpty(); // for duplicates
    if (!filledSet){
        printf("Failed to get words from set\n");
        return NULL;
    }

    List *wordsList = listNew(); // contains found words in the grid
    if (!wordsList){
        printf("Failed to get words from set\n");
        setFree(filledSet);
        return NULL;
    }

    size_t n = board->size;
    for (size_t i = 0; i < n; i++){
        for (size_t j = 0; j < n; j++){

            addFoundPrefixes(board, set, filledSet, wordsList, i, j, 0, 1); // right

            addFoundPrefixes(board, set, filledSet, wordsList, i, j, 0, -1); // left

            addFoundPrefixes(board, set, filledSet, wordsList, i, j, -1, 0); // up

            addFoundPrefixes(board, set, filledSet, wordsList, i, j, 1, 0); // down

            addFoundPrefixes(board, set, filledSet, wordsList, i, j, -1, 1); // up-right

            addFoundPrefixes(board, set, filledSet, wordsList, i, j, 1, -1); // down-left

            addFoundPrefixes(board, set, filledSet, wordsList, i, j, -1, -1); // up-left

            addFoundPrefixes(board, set, filledSet, wordsList, i, j, 1, 1); // down-right
        }

    }
    setFree(filledSet);

    return wordsList;
}
