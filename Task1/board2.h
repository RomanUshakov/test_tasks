#pragma once

#include "board.h"

//The class Board represents the game (the game parameters, current game and board states, the acceptable operations).
//Internal represantation of the board is a vector of cells with the size of boardSize x boardSize and two vectors of vectors of iterators,
//which represent the lists of sequences of adjacent cells (horizontally and vertically adjacent cells).
//Each cell represents the ball, hole or nothing (empty cell).
//Balls' and holes' numbers are stored as an attribute of the cell.
//For example:
//Real board (boardSize = 2):
//	-------------
//	| (1) | >2< |
//	=======------      (according to input 2 2 1 1 1 2 2 1 2 2 1 1 1 1 2)
//	| >1< | (2) |
//	-------------
//Internal representation:
//cells vector: {(ball, 0), (hole, 1), (hole, 0), (ball, 1)}
//horizontally adjacent cells (here we put indexes in cells vectors, to depict the iterators values): {{0, 1}, {2, 3}}
//vertically adjacent cells (here we put indexes in cells vectors, to depict the iterators values): {{0}, {2}, {1, 3}}
class Board2
{
public:
	static bool Solve(const Board2 &originalBoard, std::vector<TiltDirection> &solution);

	Board2(size_t boardSize, std::vector<Position> balls, std::vector<Position> holes, std::vector<std::pair<Position, Position>> walls);
	Board2(const Board2 &other);
	bool IsGameWon() const;
	bool IsGameLost() const;
	//all below methods thow logic_error when game is lost
	void Tilt(TiltDirection direction);
	const std::vector<TiltDirection>& GetTiltsSequence() const;
	const BoardStateDigest& GetStateDigest() const;

private:
	struct Cell
	{
		enum Value {Empty, Ball, Hole};
		
		Cell(Value type = Empty, size_t attribute = 0);

		Value type;
		size_t attribute;	//balls and holes numbers
	};
	typedef std::vector<Cell>::iterator CellsIterator;

	void FillAdjacentSequencesLists();
	template <typename AdjacentCellsIterator>
	void MoveBallsToTheStartingAdjacentCell(AdjacentCellsIterator adjacentCellsBegin, AdjacentCellsIterator adjacentCellsEnd);

	bool m_isGameWon;
	bool m_isGameLost;
	std::vector<TiltDirection> m_tilts;
	size_t m_boardSize;
	std::vector<Cell> m_cells;
	std::vector<std::vector<CellsIterator>> m_horizontallyAdjacentCells;
	std::vector<std::vector<CellsIterator>> m_verticallyAdjacentCells;
	BoardStateDigest m_stateDigest;
};

template <typename AdjacentCellsIterator>
void Board2::MoveBallsToTheStartingAdjacentCell(AdjacentCellsIterator adjacentCellsBegin, AdjacentCellsIterator adjacentCellsEnd)
{
	//go through adjacent cells
	for(auto adjacentCellsIt = adjacentCellsBegin; adjacentCellsIt != adjacentCellsEnd; ++adjacentCellsIt)
	{
		//if a ball is found try to move it
		if(Cell::Ball == (*adjacentCellsIt)->type)
		{
			auto ballNumber = (*adjacentCellsIt)->attribute;
			//search for a position where to move the ball (from the current ball position through adjacent cells)
			for(auto targetPos = adjacentCellsIt; ; --targetPos)
			{
				//if we met a board border or another ball next to the current target position - we move the ball there
				if(adjacentCellsBegin == targetPos || Cell::Ball == (*std::prev(targetPos))->type)
				{
					std::iter_swap(*targetPos, *adjacentCellsIt);
					size_t linearNumber = *targetPos - m_cells.begin();
					m_stateDigest[ballNumber].row = linearNumber / m_boardSize;
					m_stateDigest[ballNumber].column = linearNumber % m_boardSize;
					break;
				}
				if(Cell::Hole == (*std::prev(targetPos))->type)
				{
					//if the ball and the hole numbers match
					if(ballNumber == (*std::prev(targetPos))->attribute)
					{
						**std::prev(targetPos) = Cell();
						**adjacentCellsIt = Cell();
						m_stateDigest.erase(ballNumber);
						//if no balls is left
						if(m_stateDigest.empty())
						{
							m_isGameWon = true;
							return;
						}
						break;
					}
					else
					{
						m_isGameLost = true;
						return;
					}
				}
			}
		}
	}
}
