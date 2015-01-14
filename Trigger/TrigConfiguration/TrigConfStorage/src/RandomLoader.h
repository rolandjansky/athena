/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     RandomLoader.h
//PACKAGE:  TrigConfStorage      
//
//AUTHOR:   Johannes Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_RandomLoader
#define TrigConf_RandomLoader 

#include "TrigConfStorage/IRandomLoader.h"
#include "TrigConfStorage/DBLoader.h"


namespace TrigConf {
  
   class RandomLoader : virtual public IRandomLoader, public DBLoader {
   public:

      RandomLoader( StorageMgr& sm, coral::ISessionProxy& session) : DBLoader("RandomLoader", sm, session) {}
      virtual ~RandomLoader(){};

      virtual bool load( Random& data);
   };
}

#endif
