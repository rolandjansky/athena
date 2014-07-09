/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

void
my_test_function()
{
   BOOST_CHECK( 1 == 2 );
}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
    test_suite* test = BOOST_TEST_SUITE( "Master test suite" );

    test->add( BOOST_TEST_CASE( &my_test_function ) );

    return test;
}


int main(int argc, char* argv[]) {

   test_suite* test = init_unit_test_suite(argc, argv);


   return 0;
}
