/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @author Frank Winklmeier
 * @date Aug 2021
 * @brief Regression tests for dbline
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_dbline

#include "MuonCablingTools/dbline.h"

#include <string>
#include <sstream>
#include <vector>

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( test_basic )
{
  std::stringstream s("token T : A 1 B 2 3 NAME atlas\n");
  DBline data(s);
  int a{0}, b1{0}, b2{0};
  std::string name;
  ++data;
  data("token T") >> ": " >> "A" >> a;
  data("B") >> b1 >> b2 >> "NAME" >> name;
  BOOST_TEST( a==1 );
  BOOST_TEST( b1==2 );
  BOOST_TEST( b2==3 );
  BOOST_TEST( name=="atlas" );
}

BOOST_AUTO_TEST_CASE( test_multi_token )
{
  std::stringstream s("cham 0 10 cham 1 11 cham 2 12\n");
  DBline data(s);
  std::vector<int> v{0,0,0}, exp{10,11,12};
  ++data;
  for (size_t i=0; i<3; i++) {
    data("cham #",i) >> v[i];
  }
  BOOST_TEST( v==exp, boost::test_tools::per_element() );
}

BOOST_AUTO_TEST_CASE( test_comment )
{
  std::stringstream s("# Full line comment\n"
                      "######## more comments\n"
                      "token T : A 1 # B 2\n");  // partial line comment
  DBline data(s);
  int a{0};
  while (data++) {
    if ( data("token T") ) data  >> ": " >> "A" >> a;
  }

  BOOST_TEST( a==1 );
}

BOOST_AUTO_TEST_CASE( test_until )
{
  std::stringstream s("tokens to be ignored until HERE A 1 B 2\n");
  DBline data(s);
  int a{0}, b{0};
  data.go_until("HERE");
  data >> "A" >> a >> "B" >> b;
  BOOST_TEST( a==1 );
  BOOST_TEST( b==2 );
}

BOOST_AUTO_TEST_CASE( test_bool )
{
  std::stringstream s("A 1\n");
  {
    DBline data(s);
    ++data;
    BOOST_TEST( static_cast<bool>(data("A")) == true );
  }
  {
    DBline data(s);
    ++data;
    BOOST_TEST( static_cast<bool>(data("C")) == false );
  }
}

BOOST_AUTO_TEST_CASE( test_hex_dec_oct )
{
  std::stringstream s("token 0xf1234 f1234 42 010 10\n");  // hex/dec prefix optional
  DBline data(s);
  int h1{0}, h2{0}, i{0}, o1{0}, o2{0};
  ++data;
  data("token") >> data.dbhex() >> h1 >> h2 >> data.dbdec() >> i >> data.dboct() >> o1 >> o2;
  BOOST_TEST( h1==0xf1234 );
  BOOST_TEST( h2==0xf1234 );
  BOOST_TEST( i==42 );
  BOOST_TEST( o1==010 );  // octal 8
  BOOST_TEST( o2==010 );
}

BOOST_AUTO_TEST_CASE( test_zero )
{
  std::stringstream s("0\n");
  DBline data(s);
  int i{-1};
  ++data;
  data >> data.dbhex() >> i;
  BOOST_TEST( i==0 );
}

BOOST_AUTO_TEST_CASE( test_reset_fmt )
{
  std::stringstream s("token 0x10 10\n");
  DBline data(s);
  int h{0}, i{0};
  ++data;
  data("token") >> data.dbhex() >> h;
  data.reset_fmt();
  data >> i;
  BOOST_TEST( h==0x10 );
  BOOST_TEST( i==10 );
}

BOOST_AUTO_TEST_CASE( test_struct )
{
  std::stringstream s("START: \"my data\" { \n"
                      "Token 1  12 \n"
                      "Token 2  13 \n"
                      "}\n");
  DBline data(s);
  std::string name;
  int a{0}, b{0};
  ++data;
  if (data("START:") >> name >> "{") {
    do {
      (++data)("Token 1") >> a;
      (++data)("Token 2") >> b;
      ++data;
    } while (!data("}"));
  }
   BOOST_TEST( name=="my data" );
   BOOST_TEST( a==12 );
   BOOST_TEST( b==13 );
}

