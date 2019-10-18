#pragma once

//The struct Position is an utility class describing position on the board.
//Note: rows' and colums' numbers start from zero.
struct Position
{
	size_t row;
	size_t column;

	Position(size_t row = 0, size_t column = 0)
		:row(row)
		,column(column)
	{
	}
	bool operator==(const Position &other) const
	{
		return row == other.row && column == other.column;
	}
	bool IsOnABoard(size_t boardSize) const
	{
		return row < boardSize && column < boardSize;
	}
};

enum TiltDirection {West, East, North, South};

//The class BoardStateDigest is used to store and compare the state of the board though the game.
//As during the game walls and holes do not move and holes disapear only with the corresponding balls
//for a given game (a board instance) we can describe the board state by the positions of the balls left on it.
//Note: we use the fact that map values are sorted for the one pass comparison.
class BoardStateDigest : public std::map<size_t, Position>
{
public:
	 bool operator==(const BoardStateDigest& other) const
	 {
		 if(size() != other.size())
		 {
			 return false;
		 }
		 return std::equal(begin(), end(), other.begin(), ElementEqualTo);
	 }
private:
	static bool ElementEqualTo(const std::pair<size_t, Position>& x, const std::pair<size_t, Position>& y)
	{
		return x.first == y.first && x.second.column == y.second.column && x.second.row == y.second.row;
	}
};

//The class Board represents the game (the game parameters, current game and board states, the acceptable operations).
//Internal represantation of the board is a matrix with the size of 2 * boardSize - 1 x 2 * boardSize - 1.
//This size value is calculated so that we could store wall objects together with the balls and the holes.
//Each cell of the matrix represents the ball, hole, wall or nothing (empty cell) and the cell indexes start from zero.
//Balls' and holes' numbers are stored as an attribute of the cell.
//Note: that balls and holes can be placed and moved through only even positioned cells and walls are alway placed on the odd positions,
//so we always have the one to one correspondence of the internal represantation and the real board. For example:
//Real board (boardSize = 2):
//	-------------
//	| (1) | >2< |
//	=======------      (according to input 2 2 1 1 1 2 2 1 2 2 1 1 1 1 2)
//	| >1< | (2) |
//	-------------
//Internal representation (size = 3):
//	(ball, 0),	(empty, -),	(hole, 1)
//	(wall, -),	(empty, -),	(empty, -)
//	(hole, 0),	(empty, -), (ball, 1)
class Board
{
public:
	static bool Solve(const Board &originalBoard, std::vector<TiltDirection> &solution);

	Board(size_t boardSize, std::vector<Position> balls, std::vector<Position> holes, std::vector<std::pair<Position, Position>> walls);
	bool IsGameWon() const;
	bool IsGameLost() const;
	//all below methods thow logic_error when game is lost
	void Tilt(TiltDirection direction);
	void TiltWest();
	void TiltEast();
	void TiltNorth();
	void TiltSouth();
	const std::vector<TiltDirection>& GetTiltsSequence() const;
	const BoardStateDigest& GetStateDigest() const;

private:
	struct Cell
	{
		enum Value {Empty, Ball, Hole, Wall};
		
		Cell();

		Value type;
		size_t attribute;	//attribute of the object in a cell (balls and holes numbers)
	};

	bool m_isGameWon;
	bool m_isGameLost;
	std::vector<TiltDirection> m_tilts;
	boost::numeric::ublas::matrix<Cell> m_cells;
	BoardStateDigest m_stateDigest;
};