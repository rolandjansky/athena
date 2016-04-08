/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollAppendXML.cpp
 * @brief Utility to generate XML for CollAppend argument list
 * @author K. Karr <Kristo.Karr@cern.ch>
 * @author Marcin.Nowak@cern.ch
 * $Id: CollAppendXML.cxx,v 1.31 2009-05-04 15:57:47 cranshaw Exp $ 
 */

#include "CollectionUtilities/CollAppendBase.h"
#include <iostream>

using namespace std;
using namespace pool;

int main(int argc, char *argv[])
{
   string thisProgram("CollAppend.exe");
   // Convert argv to vector of strings
   vector<string> argv_v;
   for( int i=0; i<argc; ++ i )
      argv_v.push_back( argv[i] );

   CollAppendBase worker(thisProgram);
   return worker.execute( argv_v );
}

