/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CaloInfoLoader.h
//PACKAGE:  TrigConfStorage      
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  26. June. 2006                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_CaloInfoLoader
#define TrigConf_CaloInfoLoader 

#include "TrigConfStorage/ILoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
  /**@brief TriggerDB loader of the LVL1 calorimeter trigger configuration*/
  class CaloInfoLoader : virtual public ICaloInfoLoader, public DBLoader {
  public:

    /**@brief constructor
     *
     * @param sm reference to storage manager
     *
     * @param session reference to the database session
     */
    CaloInfoLoader( StorageMgr& sm,  coral::ISession& session)
      : ILoader(), DBLoader(sm, session) {}    

    /**@brief destructor*/       
    virtual ~CaloInfoLoader(){};

    virtual bool load( CaloInfo& data);
  };
}

#endif
