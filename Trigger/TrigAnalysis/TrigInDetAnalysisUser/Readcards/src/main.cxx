/**
 **     @file    main.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "ReadCards.h"


int main(int argc, char** argv)
{
  if (argc<3) return -1;
  
  ReadCards r(argv[1]);
  
  double buffer  = r.GetValue("buffer");
  double buffer2 = r.GetValue("buffer2");
  double buffer3 = r.GetValue("buffer3");
  string s       = r.GetString("s");

  vector<double> dvec = r.GetVector("dvec");
  vector<string> svec = r.GetStringVector("svec");


  cout << "buffer="  << buffer  << endl;
  cout << "buffer2=" << buffer2 << endl;
  cout << "buffer3=" << buffer3 << endl;
  cout << "s="       << s       << endl;

  for ( int i=0 ; i<dvec.size() ; i++ ) cout << "dvec[" << i << "]=" << dvec[i] << endl;
  for ( int i=0 ; i<svec.size() ; i++ ) cout << "svec[" << i << "]=" << svec[i] << endl;


  ReadCards r2(argv[2]);

  return 0;
}
