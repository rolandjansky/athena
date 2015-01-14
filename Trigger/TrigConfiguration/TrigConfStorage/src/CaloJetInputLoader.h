/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CaloJetInputLoader.h   
//PACKAGE:  TrigConfStorage  
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_CaloJetInputLoader
#define TrigConf_CaloJetInputLoader 

#include "TrigConfStorage/ICaloJetInputLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
  /**@brief TriggerDB loader of the LVL1 calorimeter jet trigger configuration*/
  class CaloJetInputLoader : virtual public ICaloJetInputLoader, public DBLoader {
  public:

    CaloJetInputLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader(sm, session) {}
    virtual ~CaloJetInputLoader(){};

    virtual bool load( CaloJetInput& data);
  };
}

#endif
