/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_NEVENTSINFO
#define UTILITIES_COLLECTION_NEVENTSINFO

#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"

/**********************************************************

MaxEventsInfo is an concretization of a CmdLineArgs2
   to contain the max number of events to process

Qualifiers: -nevents

**********************************************************/

namespace pool
{
   
   class MaxEventsInfo : public CmdLineArgs2
   {
  public:

     /// Constructors
     MaxEventsInfo( );

     virtual ~MaxEventsInfo() {}

     ///
     bool evalArgs(std::vector<std::string>& argv);

     int                get()           { return m_maxEvents; }
     bool               specified()     { return m_specified; }

  protected:
     bool       m_specified;
     int        m_maxEvents;
   };


   
} // end pool namespace

#endif 

