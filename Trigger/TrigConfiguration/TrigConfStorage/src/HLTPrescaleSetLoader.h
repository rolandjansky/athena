/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
// NAME:     HLTPrescaleSetLoader.h
// PACKAGE:  TrigConfStorage
//                                                     
// AUTHOR:   Andreas Hoecker (CERN) [Andreas.Hocker@cern.ch]
// CREATED:  10-Jan-2005   
//                                                     
// PURPOSE:
//
//////////////////////////////////////////////////////////////////////

#ifndef TRIGCONF__HLTPRESCALESETLOADER
#define TRIGCONF__HLTPRESCALESETLOADER

#include "TrigConfStorage/IHLTPrescaleSetLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
   class HLTPrescaleSetLoader : public DBLoader, virtual public IHLTPrescaleSetLoader {
   public:

      HLTPrescaleSetLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader("HLTPrescaleSetLoader", sm, session) {}
      virtual ~HLTPrescaleSetLoader(){};

      virtual bool load( HLTPrescaleSet& hltpss);
   };
}

#endif
