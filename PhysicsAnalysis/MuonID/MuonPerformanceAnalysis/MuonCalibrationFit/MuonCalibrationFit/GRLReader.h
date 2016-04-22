/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GRLReader_h
#define GRLReader_h

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <boost/regex.hpp>

using namespace std;

typedef vector< pair< int, int > > grl_vec;
typedef map< int, grl_vec > grl_map; 

class GRLReader { 

private:
  grl_map m_map;

public:
  GRLReader( string grl_name );
  ~GRLReader();
  //:::
  void Print();
  bool PassGRL( int run, int lumiblock );
};
#endif
