#include "stdafx.h"
#include "board.h"
#include "board2.h"

//example 2 from the task
//	-------------
//	| (1) | >2< |
//	=======------
//	| >1< | (2) |
//	-------------
template <class BoardType>
void Test0()
{
	//Input 2	 2	 1	 1,1	 2,2	 1,2	 2,1	 1,1:1,2

	size_t boardSize = 2;
	std::vector<Position> balls(2);
	std::vector<Position> holes(2);
	std::vector<std::pair<Position, Position>> walls(1);
	balls[0].column = 0;
	balls[0].row = 0;
	balls[1].column = 1;
	balls[1].row = 1;
	holes[0].column = 0;
	holes[0].row = 1;
	holes[1].column = 1;
	holes[1].row = 0;
	walls[0].first.column = 0;
	walls[0].first.row = 0;
	walls[0].second.column = 0;
	walls[0].second.row = 1;
	
	BoardType b(boardSize, balls, holes, walls);
	std::array<TiltDirection, 4> solutionStandard = {North, East, South, West};
	std::vector<TiltDirection> solution;
	if(BoardType::Solve(b, solution))
	{
		if(solutionStandard.size() == solution.size())
		{
			if(!std::equal(solutionStandard.begin(), solutionStandard.end(), solution.begin()))
			{
				std::cout << "Test0 warning: solution is different from the standard!" << std::endl;
			}
		}
		else
		{
			std::cerr << "Test0 failed: wrong solution found!" << std::endl;
		}
	}
	else
	{
		std::cerr << "Test0 failed: solution is not found!" << std::endl;
	}
	std::for_each(solutionStandard.begin(), solutionStandard.end(), std::bind(&BoardType::Tilt, std::ref(b), std::placeholders::_1));
	if(b.IsGameLost())
	{
		std::cerr << "Test0 failed: game is lost!" << std::endl;
	}
	else
	{
		if(!b.IsGameWon())
		{
			std::cerr << "Test0 failed: game is not won!" << std::endl;
		}
		if(solutionStandard.size() == b.GetTiltsSequence().size())
		{
			if(!std::equal(solutionStandard.begin(), solutionStandard.end(), b.GetTiltsSequence().begin()))
			{
				std::cout << "Test0 warning: solution is different from the standard!" << std::endl;
			}
		}
		else
		{
			std::cerr << "Test0 failed: wrong solution found!" << std::endl;
		}
	}
	if(!b.GetStateDigest().empty())
	{
		std::cerr << "Test0 failed: wrong state!" << std::endl;
	}
}

//example 1 from the task
//	-------------------------
//	| (1) |     ||    | (3) |
//	-------------------------
//	| (2) | >2< |     | >3< |
//	-------------------------
//	|     |     |     |     |
//	------------=======------
//	|     | >1< |     |     |
//	-------------------------
template <class BoardType>
void Test1()
{
	//Read input 4	 3	 2	 |	 1,1	 1,2	 4,1	 |	 2,4	 2,2	 4,2	 |	 2,1:3,1	 3,3:3,4

	size_t boardSize = 4;
	std::vector<Position> balls(3);
	std::vector<Position> holes(3);
	std::vector<std::pair<Position, Position>> walls(2);
	balls[0].column = 0;
	balls[0].row = 0;
	balls[1].column = 0;
	balls[1].row = 1;
	balls[2].column = 3;
	balls[2].row = 0;
	holes[0].column = 1;
	holes[0].row = 2;
	holes[1].column = 1;
	holes[1].row = 1;
	holes[2].column = 3;
	holes[2].row = 1;
	walls[0].first.column = 1;
	walls[0].first.row = 0;
	walls[0].second.column = 2;
	walls[0].second.row = 0;
	walls[1].first.column = 2;
	walls[1].first.row = 2;
	walls[1].second.column = 2;
	walls[1].second.row = 3;
	
	BoardType b(boardSize, balls, holes, walls);
	std::array<TiltDirection, 2> solutionStandard = {East, South};
	std::vector<TiltDirection> solution;
	if(BoardType::Solve(b, solution))
	{
		if(solutionStandard.size() == solution.size())
		{
			if(!std::equal(solutionStandard.begin(), solutionStandard.end(), solution.begin()))
			{
				std::cout << "Test1 warning: solution is different from the standard!" << std::endl;
			}
		}
		else
		{
			std::cerr << "Test1 failed: wrong solution found!" << std::endl;
		}
	}
	else
	{
		std::cerr << "Test1 failed: solution is not found!" << std::endl;
	}
	std::for_each(solutionStandard.begin(), solutionStandard.end(), std::bind(&BoardType::Tilt, std::ref(b), std::placeholders::_1));
	if(b.IsGameLost())
	{
		std::cerr << "Test1 failed: game is lost!" << std::endl;
	}
	else
	{
		if(!b.IsGameWon())
		{
			std::cerr << "Test1 failed: game is not won!" << std::endl;
		}
		if(solutionStandard.size() == b.GetTiltsSequence().size())
		{
			if(!std::equal(solutionStandard.begin(), solutionStandard.end(), b.GetTiltsSequence().begin()))
			{
				std::cout << "Test1 warning: solution is different from the standard!" << std::endl;
			}
		}
		else
		{
			std::cerr << "Test1 failed: wrong solution found!" << std::endl;
		}
	}
	if(!b.GetStateDigest().empty())
	{
		std::cerr << "Test1 failed: wrong state!" << std::endl;
	}
}

//Test for the cyclic game:
//	-------------------
//	|     | (1) |     |
//	=======------------
//	| >1< |     |     |
//	=======------------
//	|     |     |     |
//	-------------------
template <class BoardType>
void Test2()
{
	size_t boardSize;
	std::vector<Position> balls;
	std::vector<Position> holes;
	std::vector<std::pair<Position, Position>> walls;

	std::string inputData("3  1  2   2 1   1 2   1 1 1 2  1 2 1 3");
	std::stringstream ss(inputData);
	size_t readNumber;
	ss >> readNumber;
	boardSize = readNumber;
	ss >> readNumber;
	balls.resize(readNumber);
	holes.resize(readNumber);
	ss >> readNumber;
	walls.resize(readNumber);
	for(size_t i = 0; i < balls.size(); ++i)
	{
		ss >> readNumber;
		balls[i].column = readNumber - 1;
		ss >> readNumber;
		balls[i].row = readNumber - 1;
	}
	for(size_t i = 0; i < holes.size(); ++i)
	{
		ss >> readNumber;
		holes[i].column = readNumber - 1;
		ss >> readNumber;
		holes[i].row = readNumber - 1;
	}
	for(size_t i = 0; i < walls.size(); ++i)
	{
		ss >> readNumber;
		walls[i].first.column = readNumber - 1;
		ss >> readNumber;
		walls[i].first.row = readNumber - 1;
		ss >> readNumber;
		walls[i].second.column = readNumber - 1;
		ss >> readNumber;
		walls[i].second.row = readNumber - 1;
	}

	BoardType b(boardSize, balls, holes, walls);
	std::vector<TiltDirection> solution;
	if(BoardType::Solve(b, solution))
	{
		std::cerr << "Test2 failed: solution found!" << std::endl;
	}
}


template <typename T>
void f(T val)
{
	std::cout << val << std::endl;
}

template <typename T>
void g(T &val)
{
	++val;
	std::cout << val << std::endl;
}

template <typename T1, typename T2>
void h(T1 &val1, T2 &val2)
{
	++val1;
	++val2;
	std::cout << val1 << " " << val2 << std::endl;
}

template <typename T1, typename T2>
void j(T1 &val1, T2 &val2)
{
	++val1;
	++val2;
	std::cout << val1 << " " << val2 << std::endl;
}

template <class InputIterator, class Distance>
void advance (InputIterator &it, Distance n)
{
}

int main()
try
{
	std::cout << "Test0()" << std::endl;
	Test0<Board2>();
	std::cout << "Test1()" << std::endl;
	Test1<Board2>();
	std::cout << "Test2()" << std::endl;
	Test2<Board2>();
}
catch (const std::exception &e)
{
	std::cout << "Caught " << typeid(e).name() << ": " << e.what() << std::endl;
}