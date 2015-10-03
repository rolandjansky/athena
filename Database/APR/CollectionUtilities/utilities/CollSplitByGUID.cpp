/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollSplitByGUID.cpp
 * @brief Utility to list the file GUIDs used by a POOL collection and split the
 * collection into sub-collections by GUID
 * @author  Marcin.Nowak@cern.ch
 * $Id: CollSplitByGUID.cpp 458059 2011-09-09 13:52:47Z mnowak $
 */

#include "CollectionUtilities/CollSplitByGUIDBase.h"

#include <iostream>
#include <stdexcept>

using namespace std;
using namespace pool;

#define corENDL coral::MessageStream::endmsg


int main(int argc, const char *argv[])
{
   // Convert argv to vector of strings
   vector<string> argv_v;
   for( int i=0; i<argc; ++ i )
      argv_v.push_back( argv[i] );

   CollSplitByGUIDBase worker;
   return worker.execute( argv_v );
}

