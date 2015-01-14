/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     MuctpiLoader.h
//PACKAGE:  TrigConfStorage      
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_MuctpiLoader
#define TrigConf_MuctpiLoader 

#include "TrigConfStorage/IMuctpiLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
   class MuctpiLoader : public DBLoader, virtual public IMuctpiLoader {
   public:

      MuctpiLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader(sm, session) {}
      virtual ~MuctpiLoader(){};

      virtual bool load( Muctpi& data);

   };
}

#endif
