#include <vector>
#include <stdexcept>
#include <cstdint>

using Area = std::vector<std::vector<int>>;

enum class MoveDirection
{
	Up,
	Down,
	Left,
	Right
};

class MazeWalk
{
public:
	MazeWalk(const Area& a);

	bool SolveMaze();
	bool Move(MoveDirection direction);
	bool IsFinishReached() const;

private:
	MazeWalk(const MazeWalk& walk);
	MazeWalk& operator=(const MazeWalk& other);
	bool AdvancePosition(size_t &pos, int distance);

	const Area& m_area;
	size_t m_posX;
	size_t m_posY;
	std::vector<bool> m_isVisited;
};

MazeWalk::MazeWalk(const Area& a)
	: m_area(a)
	, m_posX(0)
	, m_posY(0)
	, m_isVisited(m_area.size()* m_area.size(), false)
{
	for (size_t i = 0; i < m_area.size(); ++i)
	{
		if (m_area[i].size() != m_area.size())
			throw std::logic_error("Not square mazes are not supported");
	}
}

MazeWalk::MazeWalk(const MazeWalk& walk)
	: m_area(walk.m_area)
	, m_posX(walk.m_posX)
	, m_posY(walk.m_posY)
	, m_isVisited(walk.m_isVisited)
{
}

MazeWalk& MazeWalk::operator=(const MazeWalk& other)
{
	if (&m_area[0] != &(other.m_area[0]))
		throw std::logic_error("Only walks though the same maze can be assigned");
	
	if (this != &other)
	{
		m_posX = other.m_posX;
		m_posY = other.m_posY;
		m_isVisited = other.m_isVisited;
	}

	return *this;
}

bool MazeWalk::SolveMaze()
{
	if (IsFinishReached())
		return true;

	auto arrMoves = { MoveDirection::Down, MoveDirection::Left, MoveDirection::Right, MoveDirection::Up };
	for (auto it = arrMoves.begin(); it != arrMoves.end(); ++it)
	{
		MazeWalk newWalk = *this;
		if (!newWalk.Move(*it))
			continue;
		if (newWalk.SolveMaze())
		{
			*this = newWalk;
			return true;
		}
	}
	return false;
}

bool MazeWalk::Move(MoveDirection direction)
{
	size_t newPosX = m_posX;
	size_t newPosY = m_posY;
	switch (direction)
	{
	case MoveDirection::Down:
		if(!AdvancePosition(newPosY, 1))
			return false;
		break;
	case MoveDirection::Up:
		if (!AdvancePosition(newPosY, -1))
			return false;
		break;
	case MoveDirection::Left:
		if (!AdvancePosition(newPosX, -1))
			return false;
		break;
	case MoveDirection::Right:
		if (!AdvancePosition(newPosX, 1))
			return false;
		break;
	}
	
	//check for a wall.
	if (m_area[newPosY][newPosX] != 0)
		return false;

	//check visited
	if (m_isVisited[newPosY * m_area.size() + newPosX])
		return false;
	
	m_isVisited[newPosY * m_area.size() + newPosX] = true;
	m_posY = newPosY;
	m_posX = newPosX;
	return true;
}

bool MazeWalk::IsFinishReached() const
{
	if (m_posY != m_area.size() - 1)
		return false;
	if (m_posX != m_area.size() - 1)
		return false;
	return true;
}

bool MazeWalk::AdvancePosition(size_t& pos, int distance)
{
	int64_t newPos = (int64_t)pos + (int64_t)distance;
	if (newPos >= (int64_t)m_area.size())
		return false;
	if (newPos < 0)
		return false;
	pos = pos + distance;
	return true;
}

bool canPass(const Area& a)
{
	MazeWalk walk(a);
	return walk.SolveMaze();
}
