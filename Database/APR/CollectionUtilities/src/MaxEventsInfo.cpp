/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CollectionUtilities/MaxEventsInfo.h"

#include <iostream>

#define corENDL coral::MessageStream::endmsg

using namespace std;
using namespace pool;


   
MaxEventsInfo::MaxEventsInfo( ) :
      CmdLineArgs2("MaxEventsInfo"),
      m_specified(false),
      m_maxEvents(-1)
{
   getArgQual("-nevents").desc << "finish after processing N events";
}


bool
MaxEventsInfo::evalArgs(std::vector<std::string>& argv)
{
   if( !CmdLineArgs2::evalArgs(argv) ) return false;

   string       nevents;
   if( getOpt("-nevents", nevents) ) {
      m_specified = true;
      istringstream  ne_str( nevents );
      ne_str >> m_maxEvents;
   }
   return true;
}

