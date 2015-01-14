/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     TriggerThresholdLoader.h
//PACKAGE:  TrigConfStorage
//
//AUTHOR:   Johannes Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_TriggerThresholdLoader
#define TrigConf_TriggerThresholdLoader 

#include "TrigConfStorage/ITriggerThresholdLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
  /**@brief TriggerDB loader of the LVL1 trigger thresholds*/
  class TriggerThresholdLoader : virtual public ITriggerThresholdLoader, public DBLoader {
  public:

    /**@brief constructor
     *
     * @param sm reference to storage manager
     *
     * @param session reference to the database session
     */
    TriggerThresholdLoader( StorageMgr& sm,  coral::ISessionProxy& session) :
       DBLoader("TriggerThresholdLoader", sm, session), m_MenuId(0), m_LoadCableInfo(true) {}

    /**@brief destructor*/       
    virtual ~TriggerThresholdLoader(){};

    virtual bool load( TriggerThreshold& data);

    void setLoadCableInfo(bool b) { m_LoadCableInfo = b; }
    bool loadCableInfo() { return m_LoadCableInfo; }

    void setMenuId(const int& id);
    int menuId();

  private:
    int m_MenuId;
    bool m_LoadCableInfo;
  };
}

#endif
