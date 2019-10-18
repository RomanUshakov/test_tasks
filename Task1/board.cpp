#include "stdafx.h"
#include "board.h"

bool Board::Solve(const Board &originalBoard, std::vector<TiltDirection> &solution)
{
	std::deque<Board> boardsQueue;
	std::deque<BoardStateDigest> statesMet;
	boardsQueue.push_back(originalBoard);

	while(!boardsQueue.empty())
	{
		Board &curBoard = boardsQueue.front();
		if(curBoard.IsGameWon())
		{
			solution.resize(curBoard.GetTiltsSequence().size());
			std::copy(curBoard.GetTiltsSequence().begin(), curBoard.GetTiltsSequence().end(), solution.begin());
			return true;
		}
		//if the game is not lost and we didn't meet the cycle (returned to a previous state)
		if(!curBoard.IsGameLost() && !std::any_of(statesMet.begin(), statesMet.end(), boost::bind1st(std::equal_to<BoardStateDigest>(), curBoard.GetStateDigest())))
		{
			statesMet.push_back(curBoard.GetStateDigest());
			boardsQueue.push_back(curBoard);
			boardsQueue.back().TiltWest();
			boardsQueue.push_back(curBoard);
			boardsQueue.back().TiltSouth();
			boardsQueue.push_back(curBoard);
			boardsQueue.back().TiltEast();
			boardsQueue.push_back(curBoard);
			boardsQueue.back().TiltNorth();
		}
		boardsQueue.pop_front();
	}
	return false;
}

Board::Board( size_t boardSize, std::vector<Position> balls, std::vector<Position> holes, std::vector<std::pair<Position, Position>> walls )
	:m_isGameWon(false)
	,m_isGameLost(false)
	,m_tilts()
	,m_cells()
	,m_stateDigest()
{
	//check board size is positive
	if(!boardSize)
	{
		throw std::invalid_argument("boardSize should be possitive");
	}

	//check balls count equals holes count
	if(balls.size() != holes.size())
	{
		throw std::invalid_argument("balls count should be equal to holes count");
	}

	//check balls and holes positions fit the board size
	auto notOnTheBoard = boost::not1(boost::bind2nd(boost::mem_fun_ref(&Position::IsOnABoard), boardSize));
	if(std::any_of(balls.begin(), balls.end(), notOnTheBoard))
	{
		throw std::domain_error("balls positions should be on the board");
	}
	if(std::any_of(holes.begin(), holes.end(), notOnTheBoard))
	{
		throw std::domain_error("holes positions should be on the board");
	}
	
	//check balls and holes positions are unique
	for(std::vector<Position>::iterator it = balls.begin(); it != balls.end(); )
	{
		auto equalToCurrent = boost::bind1st(std::equal_to<Position>(), *it);
		if(std::any_of(++it, balls.end(), equalToCurrent))
		{
			throw std::domain_error("balls positions should be different");
		}
		if(std::any_of(holes.begin(), holes.end(), equalToCurrent))
		{
			throw std::domain_error("balls positions should not intersect with holes positions");
		}
	}
	for(std::vector<Position>::iterator it = holes.begin(); it != holes.end(); )
	{
		auto equalToCurrent = boost::bind1st(std::equal_to<Position>(), *it);
		if(std::any_of(++it, holes.end(), equalToCurrent))
		{
			throw std::domain_error("holes positions should be different");
		}
	}

	//check walls positions fit the board size and are placed on the border of the cells
	for(std::vector<std::pair<Position, Position>>::iterator it = walls.begin(); it != walls.end(); ++it)
	{
		const Position &wallBegin = it->first;
		const Position &wallEnd = it->second;
		if(!(wallBegin.IsOnABoard(boardSize) && wallEnd.IsOnABoard(boardSize)))
		{
			throw std::domain_error("walls positions should be on the board");
		}
		if(!(wallBegin.row == wallEnd.row && 1 == (std::max(wallBegin.column, wallEnd.column) - std::min(wallBegin.column, wallEnd.column))) &&
		   !(wallBegin.column == wallEnd.column && 1 == (std::max(wallBegin.row, wallEnd.row) - std::min(wallBegin.row, wallEnd.row))))
		{
			throw std::domain_error("walls positions should be placed on the border of the cells");
		}
	}

	//initialize the cells
	m_cells.resize(2 * boardSize - 1, 2 * boardSize - 1, false);
	//place the balls
	for(size_t i = 0; i < balls.size(); ++i)
	{
		Cell &c = m_cells(2 * balls[i].row, 2 * balls[i].column);
		c.type = Cell::Ball;
		c.attribute = i;
	}
	//place the holes
	for(size_t i = 0; i < holes.size(); ++i)
	{
		Cell &c = m_cells(2 * holes[i].row, 2 * holes[i].column);
		c.type = Cell::Hole;
		c.attribute = i;
	}
	//place the walls
	for(size_t i = 0; i < walls.size(); ++i)
	{
		const Position &wallBegin = walls[i].first;
		const Position &wallEnd = walls[i].second;
		if(wallBegin.row == wallEnd.row)
		{
			m_cells(2 * wallBegin.row, 2 * std::min(wallBegin.column, wallEnd.column) + 1).type = Cell::Wall;
		}
		else
		{
			m_cells(2 * std::min(wallBegin.row, wallEnd.row) + 1, 2 * wallBegin.column).type = Cell::Wall;
		}
	}

	//fill the state digest
	for(size_t i = 0; i < balls.size(); ++i)
	{
		m_stateDigest[i] = Position(balls[i].row * 2, balls[i].column * 2);
	}
}

void Board::Tilt(TiltDirection direction)
{
	switch(direction)
	{
		case West:
			TiltWest();
			return;
		case East:
			TiltEast();
			return;
		case North:
			TiltNorth();
			return;
		case South:
			TiltSouth();
			return;
		default:
			throw(std::logic_error("Unexpected direction value."));
	}
}

bool Board::IsGameWon() const
{
	return m_isGameWon;
}

bool Board::IsGameLost() const
{
	return m_isGameLost;
}
const std::vector<TiltDirection>& Board::GetTiltsSequence() const
{
	return m_tilts;
}

const BoardStateDigest& Board::GetStateDigest() const
{
	if(m_isGameLost)
	{
		throw std::logic_error("the board is not valid anymore");
	}
	return m_stateDigest;
}

Board::Cell::Cell()
	:type(Empty)
	,attribute()
{
}

void Board::TiltWest()
{
	if(m_isGameLost)
	{
		throw std::logic_error("the board is not valid anymore");
	}

	if(m_isGameWon)
	{
		return;
	}

	//go row by row
	for(size_t i = 0; i < m_cells.size1(); i += 2)
	{
		//search the balls from the left to the right
		for(size_t j = 0; j < m_cells.size2(); j += 2)
		{
			//if a ball is found try to move it
			if(Cell::Ball == m_cells(i, j).type)
			{
				size_t ballNumber = m_cells(i, j).attribute;
				//search for a column where to move the ball (from the current ball position to the left)
				for(size_t targetColumn = j; ; targetColumn -= 2)
				{
					//if we met a board border or a wall or another ball next to the current target column value - we move the ball there
					if(0 == targetColumn || Cell::Wall == m_cells(i, targetColumn - 1).type || Cell::Ball == m_cells(i, targetColumn - 2).type)
					{
						std::swap(m_cells(i, targetColumn), m_cells(i, j));
						m_stateDigest[ballNumber].column = targetColumn;
						break;
					}
					//if we met the hole next to the current target column value - either we lost, or the ball and the whole annihilate
					if(Cell::Hole == m_cells(i, targetColumn - 2).type)
					{
						//if the ball and the hole numbers match
						if(ballNumber == m_cells(i, targetColumn - 2).attribute)
						{
							m_cells(i, targetColumn - 2) = Cell();
							m_cells(i, j) = Cell();
							m_stateDigest.erase(ballNumber);
							//if no balls is left
							if(m_stateDigest.empty())
							{
								m_isGameWon = true;
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

	m_tilts.push_back(West);
}

void Board::TiltEast()
{
	if(m_isGameLost)
	{
		throw std::logic_error("the board is not valid anymore");
	}

	if(m_isGameWon)
	{
		return;
	}

	//go row by row
	for(size_t i = 0; i < m_cells.size1(); i += 2)
	{
		//search the balls from the right to the left
		for(size_t j = m_cells.size2() - 1; ; j -= 2)
		{
			//if a ball is found try to move it
			if(Cell::Ball == m_cells(i, j).type)
			{
				size_t ballNumber = m_cells(i, j).attribute;
				//search for a column where to move the ball (from the current ball position to the rigth)
				for(size_t targetColumn = j; ; targetColumn += 2)
				{
					//if we met a board border or a wall or another ball next to the current target column value - we move the ball there
					if(m_cells.size2() - 1 == targetColumn || Cell::Wall == m_cells(i, targetColumn + 1).type || Cell::Ball == m_cells(i, targetColumn + 2).type)
					{
						std::swap(m_cells(i, targetColumn), m_cells(i, j));
						m_stateDigest[ballNumber].column = targetColumn;
						break;
					}
					//if we met the hole next to the current target column value - either we lost, or the ball and the whole annihilate
					if(Cell::Hole == m_cells(i, targetColumn + 2).type)
					{
						//if the ball and the hole numbers match
						if(ballNumber == m_cells(i, targetColumn + 2).attribute)
						{
							m_cells(i, targetColumn + 2) = Cell();
							m_cells(i, j) = Cell();
							m_stateDigest.erase(ballNumber);
							//if no balls is left
							if(m_stateDigest.empty())
							{
								m_isGameWon = true;
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
			if(0 == j)
			{
				break;
			}
		}
	}

	m_tilts.push_back(East);
}

void Board::TiltNorth()
{
	if(m_isGameLost)
	{
		throw std::logic_error("the board is not valid anymore");
	}

	if(m_isGameWon)
	{
		return;
	}

	//go column by column
	for(size_t j = 0; j < m_cells.size2(); j += 2)
	{
		//search the balls from the top to the bottom
		for(size_t i = 0; i < m_cells.size1(); i += 2)
		{
			//if a ball is found try to move it
			if(Cell::Ball == m_cells(i, j).type)
			{
				size_t ballNumber = m_cells(i, j).attribute;
				//search for a row where to move the ball (from the current ball position to the top)
				for(size_t targetRow = i; ; targetRow -= 2)
				{
					//if we met a board border or a wall or another ball next to the current target column value - we move the ball there
					if(0 == targetRow || Cell::Wall == m_cells(targetRow - 1, j).type || Cell::Ball == m_cells(targetRow - 2, j).type)
					{
						std::swap(m_cells(targetRow, j), m_cells(i, j));
						m_stateDigest[ballNumber].row = targetRow;
						break;
					}
					//if we met the hole next to the current target column value - either we lost, or the ball and the whole annihilate
					if(Cell::Hole == m_cells(targetRow - 2, j).type)
					{
						//if the ball and the hole numbers match
						if(ballNumber == m_cells(targetRow - 2, j).attribute)
						{
							m_cells(targetRow - 2, j) = Cell();
							m_cells(i, j) = Cell();
							m_stateDigest.erase(ballNumber);
							//if no balls is left
							if(m_stateDigest.empty())
							{
								m_isGameWon = true;
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
	
	m_tilts.push_back(North);
}

void Board::TiltSouth()
{
	if(m_isGameLost)
	{
		throw std::logic_error("the board is not valid anymore");
	}

	if(m_isGameWon)
	{
		return;
	}

	//go column by column
	for(size_t j = 0; j < m_cells.size2(); j += 2)
	{
		//search the balls from the bottom to the top
		for(size_t i = m_cells.size1() - 1; ; i -= 2)
		{
			//if a ball is found try to move it
			if(Cell::Ball == m_cells(i, j).type)
			{
				size_t ballNumber = m_cells(i, j).attribute;
				//search for a row where to move the ball (from the current ball position to the bottom)
				for(size_t targetRow = i; ; targetRow += 2)
				{
					//if we met a board border or a wall or another ball next to the current target column value - we move the ball there
					if(m_cells.size1() - 1 == targetRow || Cell::Wall == m_cells(targetRow + 1, j).type || Cell::Ball == m_cells(targetRow + 2, j).type)
					{
						std::swap(m_cells(targetRow, j), m_cells(i, j));
						m_stateDigest[ballNumber].row = targetRow;
						break;
					}
					//if we met the hole next to the current target column value - either we lost, or the ball and the whole annihilate
					if(Cell::Hole == m_cells(targetRow + 2, j).type)
					{
						//if the ball and the hole numbers match
						if(ballNumber == m_cells(targetRow + 2, j).attribute)
						{
							m_cells(targetRow + 2, j) = Cell();
							m_cells(i, j) = Cell();
							m_stateDigest.erase(ballNumber);
							//if no balls is left
							if(m_stateDigest.empty())
							{
								m_isGameWon = true;
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
			if(0 == i)
			{
				break;
			}
		}
	}

	m_tilts.push_back(South);
}