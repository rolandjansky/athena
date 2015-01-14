/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
// NAME:     HLTPrescaleSetCollectionLoader.h
// PACKAGE:  TrigConfStorage
//                                                     
// AUTHOR:   Andreas Hoecker (CERN) [Andreas.Hocker@cern.ch]
// CREATED:  10-Jan-2005   
//                                                     
// PURPOSE:
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_HLTPrescaleSetCollectionLoader
#define TrigConf_HLTPrescaleSetCollectionLoader

#include "TrigConfStorage/IHLTPrescaleSetCollectionLoader.h"
#include "TrigConfStorage/DBLoader.h"

#include <sys/types.h>

namespace TrigConf {

   class HLTPrescaleSetCollection;
  
   class HLTPrescaleSetCollectionLoader : virtual public IHLTPrescaleSetCollectionLoader, public DBLoader {
   public:

      HLTPrescaleSetCollectionLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader("HLTPrescaleSetCollectionLoader", sm, session) {}
      virtual ~HLTPrescaleSetCollectionLoader(){};

      virtual bool load( HLTPrescaleSetCollection& psc );

      virtual bool load( HLTPrescaleSetCollection& hltPS, unsigned int requestcounter, const std::string& partition);
   };
}

#endif
