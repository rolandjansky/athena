/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     ThresholdConfigLoader.h 
//PACKAGE:  TrigConfStorage  
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005
//                                  
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////
#ifndef TrigConf_ThresholdConfigLoader 
#define TrigConf_ThresholdConfigLoader 

#include "TrigConfStorage/IThresholdConfigLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {

  /**@brief TriggerDB loader of the LVL1 trigger threshold configuration*/
  class ThresholdConfigLoader : virtual public IThresholdConfigLoader, public DBLoader {
  public:

    /**@brief constructor
     *
     * @param sm reference to storage manager
     *
     * @param session reference to the database session
     */
    ThresholdConfigLoader( StorageMgr& sm,  coral::ISessionProxy& session)
       : DBLoader("ThresholdConfigLoader", sm, session) {}

    virtual ~ThresholdConfigLoader(){};

    virtual bool load( ThresholdConfig& data);

  private:

     void loadCaloInfo( ThresholdConfig& thrConfig, long caloInfoId );

  };
}

#endif
