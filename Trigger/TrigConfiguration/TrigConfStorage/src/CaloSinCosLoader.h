/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CaloSinCosLoader.h 
//PACKAGE:  TrigConfStorage         
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005         
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_CaloSinCosLoader
#define TrigConf_CaloSinCosLoader 

#include "TrigConfStorage/ICaloSinCosLoader.h"
#include "TrigConfStorage/DBLoader.h"



namespace TrigConf {
  
  /**@brief TriggerDB loader of the LVL1 calorimeter sin and cos configuration*/
  class CaloSinCosLoader : virtual public ICaloSinCosLoader, public DBLoader {
  public:

    /**@brief constructor
     *
     * @param sm reference to storage manager
     *
     * @param session reference to the database session
     */
    CaloSinCosLoader( StorageMgr& sm, coral::ISessionProxy& session) : 
       DBLoader(sm, session) {}

    /**@brief destructor*/       
    virtual ~CaloSinCosLoader(){};

    virtual bool load( CaloSinCos& data);
  };
}

#endif
