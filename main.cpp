#include <iostream>
#include <vector>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <random>
#include <cstring>

using namespace std;

#define USAGE "\
\x1b[0;32mUsage: %s [-h, --help] [-t SIZE] [--dead COLOR] [--alive COLOR]\n\n\
Options:\n\n\
    -t SIZE          Définit la taille de la grille (ex : -t 40 pour une grille 40x40).\n\
    --dead COLOR Définit la couleur des cellules mortes. Couleurs disponibles :\n\
                 0: Noir, 1: Rouge, 2: Vert, 3: Jaune, 4: Bleu, 5: Violet, 6: Cyan, 7: Blanc.\n\
    --alive COLOR Définit la couleur des cellules vivantes. Couleurs disponibles :\n\
                 0: Noir, 1: Rouge, 2: Vert, 3: Jaune, 4: Bleu, 5: Violet, 6: Cyan, 7: Blanc.\n\
    -h, --help          Affiche l'aide.\n\
\n\
Exemple :\n\
   %s -t 50 --dead 1 --alive 2\n\
    Cela créera une grille de 50x50, avec les cellules mortes en rouge et les cellule\n\
\x1b[0m"

int GRID_SIZE = 40;

int COLORS[] = {
    40, // Black
    41, // Red
    42, // Green
    43, // Yellow
    44, // Blue
    45, // Purple
    46, // Cyan
    47, // White
};

int COLOR[2] = {47, 40};

using Grid = std::vector<std::vector<short>>;

struct coord{int x,y;};

void clrscr()
{
    #ifdef _WIN32
	    system("cls");
	#elif __unix__
	    system("clear");
	#endif
}

bool isValidCoord(const coord &pos, const Grid &grid)
{
    return ((pos.x >= 0 && pos.x < GRID_SIZE) && (pos.y >= 0 && pos.y < GRID_SIZE));
}

short neighbor(const vector<vector<short>> &grid, const coord &pos)
{
    const std::vector<coord> directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, // North-West, North, North-East
        {0, -1},           {0, 1},  // West, East
        {1, -1}, {1, 0}, {1, 1}     // South-West, South, South-East
    };
    int nb = 0;

    for(const auto& dir : directions) {
        coord neighbor = {pos.x + dir.x, pos.y + dir.y};
        if (isValidCoord(neighbor, grid) && (grid[neighbor.x][neighbor.y] == 1)) {
            nb++;
        }
    }

    return nb;
}

void update(Grid &grid)
{
    Grid copy_ = grid;

    for(int i = 0; i < GRID_SIZE; ++i)
    {
        for(int j = 0; j < GRID_SIZE; ++j)
        {
            int n = neighbor(grid, {i,j});

            if((grid[i][j] == 1) && (n < 2 || n > 3))
                copy_[i][j] = 0; // death;
            if((grid[i][j] == 0) && n == 3)
                copy_[i][j] = 1; // birth
        }
    }

    swap(copy_, grid);
}

void display(const Grid &grid)
{
    clrscr();

    for(int i = 0; i < GRID_SIZE; ++i)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            cout << "\x1b[" << COLOR[grid[i][j]] << "m  ";
        }
        cout << "\x1b[0m" << endl;
    }
}

void initialize(Grid& grid) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j] = dis(gen);
        }
    }
}

void help(char *str)
{
    printf(USAGE, str, str);
}

int main(int argc, char **argv)
{
    if(argc >= 1)
    {
        // Si l'utilisateur demande de l'aide, on affiche immédiatement l'aide et on termine le programme
        for (int i = 1; i < argc; ++i) {
            if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0) {
                help(argv[0]);
                return 0;
            }
        }

        // Vérification que le nombre d'arguments est correct (doit être pair)
        if ((argc - 1) % 2 != 0) {
            help(argv[0]); // Si le nombre d'arguments est incorrect, afficher l'aide
            return 1;
        }

        for(int i = 1; i < argc; i += 2)
        {
            int value = std::abs(atoi(argv[i + 1]));
            if (strcmp("-t", argv[i]) == 0)
                GRID_SIZE = value;
            
            if (strcmp("--dead", argv[i]) == 0)
            {
                int color = COLORS[value % 8];

                if(color == COLOR[1])
                    color = COLORS[(color + 1) % 8];

                COLOR[0] = color;
            }

            if (strcmp("--alive", argv[i]) == 0)
            {
                int color = COLORS[value % 8];

                if(color == COLOR[0])
                    color = COLORS[(color + 1) % 8];

                COLOR[1] = color;
            }
        }
    }

    // char quit = 'o';
    Grid board(GRID_SIZE, vector<short>(GRID_SIZE, false));

    initialize(board);

    while(/*quit != 'q' && quit != 'Q'*/ true)
    {
        display(board);
        update(board);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
