/*
SPlease provide one file called `labirinth_<yourname>.cpp`. 
Implement a method called `canPass(const Area& area)` that will return true if
one can reach the bottom-right corner starting from the top-left corner. Treat
`1` as a wall (you cannot go over `1` when finding the passage). 
Allowed moves are left, right, up, down.

The provided algorithm shall work for other 8x8 labirinths as well. 
You don’t have to find optimal way nor return the chosen path - only 
true/false is required as the output.

*/
#include <iostream>
#include <vector>

using Area = std::vector<std::vector<int>>;

// 1 - wall that we cannot pass, 0 - empty space that we can move to
Area labirinth = {

	//0  1  2  3  4  5  6  7

	{ 0, 0, 1, 0, 0, 0, 0, 0 },//0

	{ 0, 0, 1, 1, 0, 1, 1, 1 },//1

	{ 0, 0, 0, 0, 0, 0, 1, 0 },//2

	{ 0, 0, 0, 0, 1, 0, 0, 0 },//3

	{ 1, 1, 1, 1, 1, 0, 1, 1 },//4

	{ 1, 0, 0, 0, 0, 0, 0, 0 },//5

	{ 0, 1, 1, 1, 1, 1, 0, 0 },//6

	{ 0, 0, 0, 0, 0, 0, 0, 0 },//7

};

bool canPass(const Area& a);

int main() {

	std::cout << "Can I pass? : " << std::boolalpha << canPass(labirinth) << std::endl;
}
