/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////
//  FileParse implementation file
////////////////////////////////////////////////////////////////////////

#include "FileParse.h"
using namespace std;


bool FileParse::expect( ifstream &f, string msg, string fname )
{
  string str;
  
  if( f.bad() ) 
  {
    cerr << "FileParse::expect: File " << fname << " is bad" << endl;
    return false;
  }
  
  f >> str;
  
  if( str == msg )
    return true;
  else
  {
    cerr << "FileParse::expect: Found \"" << str << "\" instead of \"" << 
      msg << "\" in file " << fname << endl;
    return false;
  }

}

bool FileParse::expectInt( ifstream &f, string msg, string fname, int &val )
{
  if( !expect( f, msg, fname ) || f.bad() )
    return false;
  else
  {
    f >> val;
    return true;
  }
}

bool FileParse::expectInt( ifstream &f, string msg, string fname, int &val, int val2 )
{
  if( !expectInt( f, msg, fname, val ) || f.bad() )
    return false;
 
  if( val == val2 )
    return true;
  else
  {
    cerr << "FileParse::expectInt: Found " << val << " instead of " << 
      val2 << " in file " << fname << endl;
    return false;
  }

}
