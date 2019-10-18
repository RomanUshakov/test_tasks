#include "stdafx.h"
#include "board.h"

int _tmain(int argc, _TCHAR* argv[])
try
{
	//parse command line parameters
	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()
    ("help", "produce help message")
	("input_file", po::value<std::string>(), "set path to the file with the input data")
	;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);    
	if(vm.count("help")) 
	{
		std::cout << desc << "\n";
		return 1;
	}
	if(!vm.count("input_file"))
	{
		std::cerr << "No input file was specified!" << std::endl;
		return 2;
	}

	//read input from the file
	std::string fileName(vm["input_file"].as<std::string>());
	std::ifstream inputFile(fileName);
	if(!inputFile.is_open())
	{
		std::cerr << "Error while openning the input file: " << fileName << std::endl;
		return 3;
	}
	std::string inputData;
	if(!std::getline(inputFile,	inputData))
	{
		std::cerr << "Error while reading the input file: " << fileName << std::endl;
		return 3;
	}

	size_t boardSize;
	std::vector<Position> balls;
	std::vector<Position> holes;
	std::vector<std::pair<Position, Position>> walls;

	//Parse input data
	//bellow we don't check for errors in the input data (according to the task)
	//Note: in the input we recieve positions starting from (1,1), so we fix them
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

	//Solve the game
	Board b(boardSize, balls, holes, walls);
	std::vector<TiltDirection> solution;
	bool solutionFound = Board::Solve(b, solution);
	if(solutionFound)
	{
		for(size_t i = 0; i < solution.size(); ++i)
		{
			switch(solution[i])
			{
				case East:
					std::cout << "E ";
					break;
				case West:
					std::cout << "W ";
					break;
				case North:
					std::cout << "N ";
					break;
				case South:
					std::cout << "S ";
					break;
				default:
					std::cerr << "Unexpected value in the solution!" << std::endl;
					return 4;
			}
		}
	}
	else
	{
		std::cout << "X";
	}
	std::cout << std::endl;

	return 0;
}
catch(const std::exception& e)
{
	std::cout << "Caught " << typeid(e).name() << ": " << e.what() << std::endl;
}