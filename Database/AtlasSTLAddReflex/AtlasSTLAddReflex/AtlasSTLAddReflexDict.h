// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Some common classes for general use in ATLAS

#include <utility> // for std::pair
#include <vector>
#include <map>
#include <set>
#include <string>

#include <stdint.h>

#include <boost/array.hpp>

namespace AtlasSTLAddReflexDict {

struct dict
{
  std::pair<int, float>                 m_pair_int_float;
  std::pair<unsigned int, unsigned int> m_pair_uint_uint;
  std::pair<std::string, unsigned int>  m_pair_std_uint;
  std::pair<std::string, double>        m_pair_str_double;
  std::pair<std::string, std::vector<int> >    m_pair_str_vec_of_ints;
  std::pair<std::string, std::vector<double> > m_pair_str_vec_of_doubles;
  
  std::pair< 
    std::pair<std::string, unsigned int>,
    std::pair<std::string, unsigned int> > m_pair_pair_str_uint_pair_str_uint;
  std::pair<unsigned int, std::vector<float> > m_pair_uint_vec_of_floats;
  std::pair<unsigned int, std::vector<unsigned int> > m_pair_uint_vec_of_uints;
  
  std::vector<long long>             m_vecLongLong;
  std::vector<unsigned long long>    m_vecULongLong;

  std::vector<std::vector<float> >   m_vecVecFloat;
  std::vector<std::vector<double> >  m_vecVecDouble;
  std::vector<std::vector<char> >    m_vecVecChar;
  std::vector<std::vector<short> >   m_vecVecShort;
  std::vector<std::vector<int> >     m_vecVecInt;
  std::vector<std::vector<long> >    m_vecVecLong;
  std::vector<std::vector<uint8_t> >          m_vecVecUInt8;
  std::vector<std::vector<unsigned short> >   m_vecVecUshort;
  std::vector<std::vector<unsigned int> >     m_vecVecUint;
  std::vector<std::vector<unsigned long> >    m_vecVecULong;
  std::vector<std::vector<unsigned long long> > m_vecVecULongLong;
  std::vector<std::vector<std::string> >      m_vecVecString;

  std::vector<std::vector<std::vector<float> > > m_vecVecVecFloat;
  std::vector<std::vector<std::vector<int> > >   m_vecVecVecInt;
  std::vector<std::vector<std::vector<unsigned char> > >   m_vecVecVecUChar;
  std::vector<std::vector<std::vector<unsigned long> > >   m_vecVecVecULong;

  std::vector<std::pair<int, float> > m_vec_pair_int_float;
  std::vector<std::pair<unsigned int, 
			std::vector<float> > > m_vec_pair_uint_vec_of_floats;

  std::vector<std::pair<std::string,double> > m_vec_pair_str_double;
  std::vector<std::pair<std::string,
			std::vector<int> > >  m_vec_pair_str_vec_of_ints;
  std::vector<std::pair<std::string,
			std::vector<double> > > m_vec_pair_str_vec_of_doubles;
  std::vector<std::pair<std::pair<std::string,unsigned int>,
			std::pair<std::string,unsigned int> > > m_vec_pair_pair_str_uint_pair_str_uint;

  std::map<int, unsigned int>        m_mapIntUint;
  std::map<unsigned int, unsigned int> m_map_uint_uint;
  std::map<unsigned int, std::string> m_map_uint_str;
  std::map<const std::string, unsigned int> m_mapStrUint;
  std::map<std::string, bool>        m_map_str_bool;
  std::map<std::string, int>         m_map_str_int;
  std::map<std::string, int>::iterator         m_map_str_int_itr;
  std::map<std::string, unsigned short> m_map_str_sshort;
  std::map<std::string, unsigned long> m_map_str_ulong;
  std::map<std::string, float>       m_map_str_float;
  std::map<std::string, double>      m_map_str_double;
  std::map<std::string, std::string> m_map_str_str;
  std::map<std::string,
	   std::vector<bool> >       m_map_str_vec_of_bools;
  std::map<std::string,
	   std::vector<double> >     m_map_str_vec_of_doubles;
  std::map<std::string,
	   std::vector<float> >      m_map_str_vec_of_floats;
  std::map<std::string,
	   std::vector<int> >        m_map_str_vec_of_ints;
  std::map<unsigned int,
	   std::vector<float> >      m_map_uint_vec_of_floats;
  std::map<unsigned int,std::set<std::pair<unsigned int, std::string> > >
    m_map_uint_set_pair_uint_string;

  std::multimap<std::string, int>    m_mmap_std_int;

  boost::array<float,2>              m_barrayFloat2;
  boost::array<float,3>              m_barrayFloat3;
  boost::array<float,4>              m_barrayFloat4;
  boost::array<float,5>              m_barrayFloat5;
  boost::array<float,6>              m_barrayFloat6;
  boost::array<float,7>              m_barrayFloat7;
  boost::array<float,8>              m_barrayFloat8;
  boost::array<float,9>              m_barrayFloat9;
  boost::array<float,10>             m_barrayFloat10;
  boost::array<float,16>             m_barrayFloat16;
  boost::array<double,2>             m_barrayDouble2;
  boost::array<double,3>             m_barrayDouble3;
  boost::array<double,4>             m_barrayDouble4;
  boost::array<double,5>             m_barrayDouble5;
  boost::array<double,6>             m_barrayDouble6;
  boost::array<double,7>             m_barrayDouble7;
  boost::array<double,8>             m_barrayDouble8;
  boost::array<double,9>             m_barrayDouble9;
  boost::array<double,10>            m_barrayDouble10;
  boost::array<double,16>            m_barrayDouble16;
  boost::array<int,2>                m_barrayInt2;
  boost::array<int,3>                m_barrayInt3;
  boost::array<int,4>                m_barrayInt4;
  boost::array<int,5>                m_barrayInt5;
  boost::array<int,6>                m_barrayInt6;
  boost::array<int,7>                m_barrayInt7;
  boost::array<int,8>                m_barrayInt8;
  boost::array<int,9>                m_barrayInt9;
  boost::array<int,10>               m_barrayInt10;
  boost::array<long,2>               m_barrayLong2;
  boost::array<long,3>               m_barrayLong3;
  boost::array<long,4>               m_barrayLong4;
  boost::array<long,5>               m_barrayLong5;
  boost::array<long,6>               m_barrayLong6;
  boost::array<long,7>               m_barrayLong7;
  boost::array<long,8>               m_barrayLong8;
  boost::array<long,9>               m_barrayLong9;
  boost::array<long,10>              m_barrayLong10;
};
} //> namespace AtlasSTLAddReflexDict


