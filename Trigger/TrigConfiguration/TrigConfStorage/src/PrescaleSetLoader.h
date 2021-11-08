/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     PrescaleSetLoader.h
//PACKAGE:  TrigConfStorage
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  25. Jan. 2006                                                     
//                                                     
//PURPOSE:
// Loads a L1 Prescale from a XML file. 
// ** Loads using the 64bit classes.
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_PrescaleSetLoader
#define TrigConf_PrescaleSetLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfStorage/DBLoader.h"


namespace TrigConf {
  
   class PrescaleSetLoader : virtual public IPrescaleSetLoader, public DBLoader {
   public:

      PrescaleSetLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader("PrescaleSetLoader", sm, session) {}
      virtual ~PrescaleSetLoader(){};

      virtual bool load(unsigned int ctpVersion, PrescaleSet& data);
   };
}

#endif
