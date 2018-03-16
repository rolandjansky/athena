/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollAppend.cpp
 * @brief Utility to copy and merge POOL collections - uses CollAppendBase class
 * @author Marcin.Nowak@cern.ch
 */

#include "CollectionUtilities/CollAppendBase.h"

using namespace std;
using namespace pool;

int main(int argc, const char *argv[])
{
   // Convert argv to vector of strings
   vector<string> argv_v;
   for( int i=0; i<argc; ++ i )
      argv_v.push_back( argv[i] );
	
   CollAppendBase worker("CollAppend");
   return worker.execute( argv_v );
}

