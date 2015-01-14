/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     ThresholdMonitorLoader.h
//PACKAGE:  TrigConfStorage
//
//AUTHOR:   Gordon Fischer (CERN)	Gordon.Fischer@cern.ch 
//CREATED:  11. Oct. 2007                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_ThresholdMonitorLoader
#define TrigConf_ThresholdMonitorLoader 

#include "TrigConfStorage/IThresholdMonitorLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
   class ThresholdMonitorLoader : virtual public IThresholdMonitorLoader, public DBLoader {
   public:

      ThresholdMonitorLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader(sm, session), m_MenuId(0) {}
      virtual ~ThresholdMonitorLoader(){};

      virtual bool load( ThresholdMonitor& data);

      void setMenuId(const long& id);
      long menuId();
     
   private:
      long m_MenuId;
   };
}

#endif

