#include "stdafx.h"
#include "board2.h"

bool Board2::Solve(const Board2 &originalBoard, std::vector<TiltDirection> &solution)
{
	std::deque<Board2> boardsQueue;
	std::deque<BoardStateDigest> statesMet;
	boardsQueue.push_back(originalBoard);

	while(!boardsQueue.empty())
	{
		Board2 &curBoard = boardsQueue.front();
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
			boardsQueue.back().Tilt(West);
			boardsQueue.push_back(curBoard);
			boardsQueue.back().Tilt(South);
			boardsQueue.push_back(curBoard);
			boardsQueue.back().Tilt(East);
			boardsQueue.push_back(curBoard);
			boardsQueue.back().Tilt(North);
		}
		boardsQueue.pop_front();
	}
	return false;
}

Board2::Board2( size_t boardSize, std::vector<Position> balls, std::vector<Position> holes, std::vector<std::pair<Position, Position>> walls )
	:m_isGameWon(false)
	,m_isGameLost(false)
	,m_tilts()
	,m_boardSize()
	,m_cells()
	,m_horizontallyAdjacentCells()
	,m_verticallyAdjacentCells()
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
	
	m_boardSize = boardSize;

	//initialize the cells
	m_cells.resize(boardSize * boardSize);
	//place the balls
	for(size_t i = 0; i < balls.size(); ++i)
	{
		Cell &c = m_cells[balls[i].row * boardSize + balls[i].column];
		c.type = Cell::Ball;
		c.attribute = i;
	}
	//place the holes
	for(size_t i = 0; i < holes.size(); ++i)
	{
		Cell &c = m_cells[holes[i].row * boardSize + holes[i].column];;
		c.type = Cell::Hole;
		c.attribute = i;
	}

	//fill the state digest
	for(size_t i = 0; i < balls.size(); ++i)
	{
		m_stateDigest[i] = Position(balls[i].row, balls[i].column);
	}

	//partition walls to horizontal and vertical
	std::vector<std::pair<Position, Position>> horizontalWalls;
	std::vector<std::pair<Position, Position>> verticalWalls;
	std::partition_copy(walls.begin(), walls.end(), std::back_inserter(horizontalWalls), std::back_inserter(verticalWalls), [](const std::pair<Position, Position> &wall) {
		return wall.first.column == wall.second.column;
	});
	//make horizontal walls to be set through positions from top to bottom
	std::for_each(horizontalWalls.begin(), horizontalWalls.end(), [](std::pair<Position, Position> &wall) {
		if(wall.first.row > wall.second.row)
		{
			std::swap(wall.first, wall.second);
		}
	});
	//make vertical walls to be set through positions from left to right
	std::for_each(verticalWalls.begin(), verticalWalls.end(), [](std::pair<Position, Position> &wall) {
		if(wall.first.column > wall.second.column)
		{
			std::swap(wall.first, wall.second);
		}
	});
	//sort horizontal walls according to positions from left to right column and from top to bottom in each column
	std::sort(horizontalWalls.begin(), horizontalWalls.end(), [](const std::pair<Position, Position> &first, const std::pair<Position, Position> &second) -> bool {
		if(first.first.column == second.first.column)
		{
			return first.first.row < second.first.row;
		}
		return first.first.column < second.first.column;
	});
	//sort vertical walls according to positions from top to bottom row and from left to right in each row
	std::sort(verticalWalls.begin(), verticalWalls.end(), [](const std::pair<Position, Position> &first, const std::pair<Position, Position> &second) -> bool {
		if(first.first.row == second.first.row)
		{
			return first.first.column < second.first.column;
		}
		return first.first.row < second.first.row;
	});
	//create the list of sequences of the horizontally adjacent cells
	//(without any vertical walls there would be boardSize sequences of boardSize length each; a vertical wall splits corresponding sequence on two)
	m_horizontallyAdjacentCells.resize(boardSize + verticalWalls.size());
	for(size_t adjacentCellsSequenceNum = 0, adjacentCellsRow = 0, adjacentCellsStartColumn = 0; adjacentCellsSequenceNum < m_horizontallyAdjacentCells.size(); ++adjacentCellsSequenceNum)
	{
		//if current sequence should be split
		if(adjacentCellsSequenceNum - adjacentCellsRow < verticalWalls.size() && adjacentCellsRow == verticalWalls[adjacentCellsSequenceNum - adjacentCellsRow].second.row)
		{
			m_horizontallyAdjacentCells[adjacentCellsSequenceNum].resize(verticalWalls[adjacentCellsSequenceNum - adjacentCellsRow].second.column - adjacentCellsStartColumn);
			adjacentCellsStartColumn = verticalWalls[adjacentCellsSequenceNum - adjacentCellsRow].second.column;
		}
		//current sequence lasts till the end of the row
		else
		{
			m_horizontallyAdjacentCells[adjacentCellsSequenceNum].resize(boardSize - adjacentCellsStartColumn);
			adjacentCellsStartColumn = 0;
			++adjacentCellsRow;
		}
	}
	//create the list of sequences of the vertically adjacent cells 
	//(without any horizontal walls there would be boardSize sequences of boardSize length each; a horizontal wall splits corresponding sequence on two)
	m_verticallyAdjacentCells.resize(boardSize + horizontalWalls.size());
	for(size_t adjacentCellsSequenceNum = 0, adjacentCellsColumn = 0, adjacentCellsStartRow = 0; adjacentCellsSequenceNum < m_verticallyAdjacentCells.size(); ++adjacentCellsSequenceNum)
	{
		//if current sequence should be split
		if(adjacentCellsSequenceNum - adjacentCellsColumn < horizontalWalls.size() && adjacentCellsColumn == horizontalWalls[adjacentCellsSequenceNum - adjacentCellsColumn].second.column)
		{
			m_verticallyAdjacentCells[adjacentCellsSequenceNum].resize(horizontalWalls[adjacentCellsSequenceNum - adjacentCellsColumn].second.row - adjacentCellsStartRow);
			adjacentCellsStartRow = horizontalWalls[adjacentCellsSequenceNum - adjacentCellsColumn].second.row;
		}
		//current sequence lasts till the end of the row
		else
		{
			m_verticallyAdjacentCells[adjacentCellsSequenceNum].resize(boardSize - adjacentCellsStartRow);
			adjacentCellsStartRow = 0;
			++adjacentCellsColumn;
		}
	}
	FillAdjacentSequencesLists();
}

Board2::Board2(const Board2 &other)
	:m_isGameWon(other.m_isGameWon)
	,m_isGameLost(other.m_isGameLost)
	,m_tilts(other.m_tilts)
	,m_boardSize(other.m_boardSize)
	,m_cells(other.m_cells)
	,m_horizontallyAdjacentCells(other.m_horizontallyAdjacentCells)
	,m_verticallyAdjacentCells(other.m_verticallyAdjacentCells)
	,m_stateDigest(other.m_stateDigest)
{
	FillAdjacentSequencesLists();
}

void Board2::Tilt(TiltDirection direction)
{
	if(m_isGameLost)
	{
		throw std::logic_error("the board is not valid anymore");
	}
	
	if(m_isGameWon)
	{
		return;
	}

	std::vector<std::vector<CellsIterator>>::const_iterator adjacentCellsSequencesListBegin;
	std::vector<std::vector<CellsIterator>>::const_iterator adjacentCellsSequencesListEnd;
	if(West == direction || East == direction)
	{
		adjacentCellsSequencesListBegin = m_horizontallyAdjacentCells.cbegin();
		adjacentCellsSequencesListEnd = m_horizontallyAdjacentCells.cend();
	}
	else
	{
		adjacentCellsSequencesListBegin = m_verticallyAdjacentCells.cbegin();
		adjacentCellsSequencesListEnd = m_verticallyAdjacentCells.cend();
	}
	if(West == direction || North == direction)
	{
		for(auto adjacentCellsSequenceIt = adjacentCellsSequencesListBegin; adjacentCellsSequenceIt != adjacentCellsSequencesListEnd; ++adjacentCellsSequenceIt)
		{
			MoveBallsToTheStartingAdjacentCell(adjacentCellsSequenceIt->cbegin(), adjacentCellsSequenceIt->cend());
			if(m_isGameLost || m_isGameWon)
			{
				break;
			}
		}
	}
	else
	{
		for(auto adjacentCellsSequenceIt = adjacentCellsSequencesListBegin; adjacentCellsSequenceIt != adjacentCellsSequencesListEnd; ++adjacentCellsSequenceIt)
		{
			MoveBallsToTheStartingAdjacentCell(adjacentCellsSequenceIt->crbegin(), adjacentCellsSequenceIt->crend());
			if(m_isGameLost || m_isGameWon)
			{
				break;
			}
		}
	}
	
	m_tilts.push_back(direction);
}

bool Board2::IsGameWon() const
{
	return m_isGameWon;
}

bool Board2::IsGameLost() const
{
	return m_isGameLost;
}
const std::vector<TiltDirection>& Board2::GetTiltsSequence() const
{
	return m_tilts;
}

const BoardStateDigest& Board2::GetStateDigest() const
{
	if(m_isGameLost)
	{
		throw std::logic_error("the board is not valid anymore");
	}
	return m_stateDigest;
}

Board2::Cell::Cell(Value type, size_t attribute)
	:type(type)
	,attribute(attribute)
{
}

void Board2::FillAdjacentSequencesLists()
{
	//fill the horizontally adjacent cells
	for(size_t i = 0, adjacentCellsSequenceNum = 0, sequenceStartLinearNumber = 0; i < m_cells.size(); ++i)
	{
		if(i == sequenceStartLinearNumber + m_horizontallyAdjacentCells[adjacentCellsSequenceNum].size())
		{
			sequenceStartLinearNumber += m_horizontallyAdjacentCells[adjacentCellsSequenceNum].size();
			++adjacentCellsSequenceNum;
		}
		//size_t row = i / m_boardSize;
		//size_t column = i % m_boardSize;
		//(row * boardSize + column == i)
		m_horizontallyAdjacentCells[adjacentCellsSequenceNum][i - sequenceStartLinearNumber] = m_cells.begin() + i;
	}
	//fill the vertically adjacent cells
	for(size_t i = 0, adjacentCellsSequenceNum = 0, sequenceStartLinearNumber = 0; i < m_cells.size(); ++i)
	{
		if(i == sequenceStartLinearNumber + m_verticallyAdjacentCells[adjacentCellsSequenceNum].size())
		{
			sequenceStartLinearNumber += m_verticallyAdjacentCells[adjacentCellsSequenceNum].size();
			++adjacentCellsSequenceNum;
		}
		size_t row = i % m_boardSize;
		size_t column = i / m_boardSize;
		m_verticallyAdjacentCells[adjacentCellsSequenceNum][i - sequenceStartLinearNumber] = m_cells.begin() + row * m_boardSize + column;
	}
}