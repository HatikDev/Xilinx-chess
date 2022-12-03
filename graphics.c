#include "constants.h"
#include "figures.h"
#include "graphics.h"
#include "types.h"

extern struct Board board;
extern uint16_t frame[FRAMESIZE];

void init_frame()
{
    for (int i = 0; i < FRAMESIZE; ++i)
        frame[i] = 0x008;

    for (int i = 0; i < 64; ++i)
        board.cells[i] = EMPTY;

    board.cells[0] = ROOK | BLACK;
    board.cells[1] = KNIGHT | BLACK;
    board.cells[2] = BISHOP | BLACK;
    board.cells[3] = KING | BLACK;
    board.cells[4] = QUEEN | BLACK;
    board.cells[5] = BISHOP | BLACK;
    board.cells[6] = KNIGHT | BLACK;
    board.cells[7] = ROOK | BLACK;

    for (int i = 8; i <= 15; ++i)
        board.cells[i] = PAWN | BLACK;

    board.cells[56] = ROOK | WHITE;
    board.cells[57] = KNIGHT | WHITE;
    board.cells[58] = BISHOP | WHITE;
    board.cells[59] = QUEEN | WHITE;
    board.cells[60] = KING | WHITE;
    board.cells[61] = BISHOP | WHITE;
    board.cells[62] = KNIGHT | WHITE;
    board.cells[63] = ROOK | WHITE;

    for (int i = 48; i <= 55; ++i)
        board.cells[i] = PAWN | WHITE;
}

void draw_board(cairo_t *cr)
{
    int cellSize = HEIGHT / 8;
    int leftOffset = (WIDTH - HEIGHT) / 2;

    uint16_t color;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if ((i + j) % 2)
                color = 0xd75;
            else
                color = 0xfca;

            int offsetX = i * cellSize;
            int offsetY = j * cellSize;
            for (int k = 0; k < cellSize; ++k)
            {
                for (int l = 0; l < cellSize; ++l)
                {
                    frame[leftOffset + offsetX + k + (offsetY + l) * 640] = color;

                }
            }
        }
    }
}

void draw_figure_polygon(cairo_t *cr, int x, int y, int minCellX, int minCellY, uint16_t color)
{
    int globalOffset = (WIDTH - HEIGHT) / 2;
    int cellSize = HEIGHT / 8; // it's costil
    int minCellWidth = cellSize / FIGURE_WIDTH;
    int minCellHeight = cellSize / FIGURE_HEIGHT - 1;

    for (int i = 0; i <= minCellWidth; ++i)
    {
        for (int j = 0; j <= minCellHeight; ++j)
        {
            int offset = minCellX + minCellY * 640 + minCellX + minCellY * 640 * minCellHeight + globalOffset + x * cellSize + y * cellSize * 640;
            frame[offset + i + j * 640] = color;
        }
    }
}

void draw_figure(cairo_t *cr, const char** figure, int index, uint16_t color)
{
    int x = index % 8;
    int y = index / 8;

    for (int j = 0; j < FIGURE_WIDTH; ++j)
    {
        for (int k = 0; k < FIGURE_HEIGHT; ++k)
        {
            if (figure[k][j] == '#')
            {
                draw_figure_polygon(cr, x, y, j, k, color);
            }
        }
    }
}

void draw_figures(cairo_t *cr)
{
    int cellSize = HEIGHT / 8;
    int leftOffset = (WIDTH - HEIGHT) / 2;
    int miniCellWidth = cellSize / FIGURE_WIDTH * FIGURE_WIDTH_COEFF;
    int miniCellHeight = cellSize / FIGURE_HEIGHT * FIGURE_HEIGHT_COEFF;
    int figureOffsetX = cellSize * (1 - FIGURE_WIDTH_COEFF) / 2;
    int figureOffsetY = cellSize * (1 - FIGURE_HEIGHT_COEFF) / 2;
    const char** figure;

    for (int i = 0; i < 64; ++i)
    {
        if (board.cells[i] == EMPTY)
            continue;

        switch (board.cells[i] & 127)
        {
        case PAWN:
            figure = Pawn;
            break;
        case ROOK:
            figure = Rook;
            break;
        case BISHOP:
            figure = Bishop;
            break;
        case KNIGHT:
            figure = Knight;
            break;
        case QUEEN:
            figure = Queen;
            break;
        case KING:
            figure = King;
            break;
        }

        uint16_t color;
        if ((board.cells[i] & COLOR) == BLACK)
            color = 0x012;
        else
            color = 0xfff;

        draw_figure(cr, figure, i, color);
    }
}

void draw_selector(int x, int y)
{
    int cellSize = HEIGHT / 8;
    int globalOffset = (WIDTH - HEIGHT) / 2;

    for (int i = x * cellSize; i < (x + 0.1) * cellSize; ++i)
    {
        for (int j = y * cellSize; j < (y + 1) * cellSize; ++j)
        {
            frame[globalOffset + i + 640 * j] = 0xa00;
        }
    }

    for (int i = x * cellSize; i < (x + 1) * cellSize; ++i)
    {
        for (int j = y * cellSize; j < (y + 0.1)* cellSize; ++j)
        {
            frame[globalOffset + i + 640 * j] = 0xa00;
        }
    }

    for (int i = (x + 0.9) * cellSize; i < (x + 1) * cellSize; ++i)
    {
        for (int j = y * cellSize; j < (y + 1) * cellSize; ++j)
        {
            frame[globalOffset + i + 640 * j] = 0xa00;
        }
    }

    for (int i = x * cellSize; i < (x + 1) * cellSize; ++i)
    {
        for (int j = (y + 0.9) * cellSize; j < (y + 1) * cellSize; ++j)
        {
            frame[globalOffset + i + 640 * j] = 0xa00;
        }
    }
}
