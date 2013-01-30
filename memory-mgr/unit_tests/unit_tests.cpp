/* 
Generic Memory Manager (memory-mgr)
http://memory-mgr.sourceforge.net/
Copyright (c) 2007-2008 Anton (shikin) Matosov

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3, 29 June 2007 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA <http://fsf.org/>


Please feel free to contact me via e-mail: shikin@users.sourceforge.net
*/

#include "stdafx.h"
#include <memory-mgr/config/config.h>
#include <memory-mgr/memory_manager.h>
#include <memory-mgr/singleton_manager.h>
#include <memory-mgr/size_tracking.h>
#include <memory-mgr/heap_segment.h>
#include <memory-mgr/allocator.h>


#define BOOST_TEST_MODULE memory_mgr_unit_tests
#include <boost/test/auto_unit_test.hpp>


boost::unit_test::test_suite* init_unit_test_suite( int, char* [] )
{
	return 0;
}