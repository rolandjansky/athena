/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     BunchGroupSetLoader.h
//PACKAGE:  TrigConfStorage      
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  26. Jan. 2006                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_BunchGroupSetLoader
#define TrigConf_BunchGroupSetLoader

#include "TrigConfStorage/IBunchGroupSetLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {

   class BunchGroupSet;
  
   class BunchGroupSetLoader : public DBLoader, virtual public IBunchGroupSetLoader {
   public:

      BunchGroupSetLoader( StorageMgr& sm,  coral::ISessionProxy& session) :
         DBLoader("BunchGroupSetLoader", sm, session) {}

      virtual ~BunchGroupSetLoader(){};

      virtual bool load( BunchGroupSet& data);
   };
}

#endif
