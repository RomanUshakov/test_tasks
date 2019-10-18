// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#pragma warning( push,3 )
#pragma warning(disable : 4996)

#include <stdexcept>
#include <functional>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>
#include <deque>
#include <queue>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <iterator>

//#define BOOST_ALL_DYN_LINK
#include <boost/functional.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/program_options.hpp>

#pragma warning( pop )
