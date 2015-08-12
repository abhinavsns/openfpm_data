#define BOOST_DISABLE_ASSERTS

#include "config.h"
#define BOOST_TEST_MODULE "C++ test module for OpenFPM_data project"
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <boost/mpl/int.hpp>
#include <typeinfo>

// Include tests

#include "Space/Shape/Point_unit_test.hpp"
#include "timer_util_test.hpp"
#include "Grid/grid_key_dx_expression_unit_tests.hpp"
#include "Point_test_unit_tests.hpp"
#include "util/util_test.hpp"
#include "Space/SpaceBox_unit_tests.hpp"
#include "Space/Shape/Box_unit_tests.hpp"
#include "NN/CellList/CellList_test.hpp"
#include "Vector/vector_unit_tests.hpp"
#include "hypercube_unit_test.hpp"
#include "Graph/graph_unit_tests.hpp"
#include "Grid/grid_unit_tests.hpp"
