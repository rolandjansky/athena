/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     IHLTFrameLoader.h
//PACKAGE:  TrigConfStorage
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_IThresholdMonitorLoader
#define TrigConf_IThresholdMonitorLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class ThresholdMonitor;
  
   class IThresholdMonitorLoader : virtual public ILoader {
   public:

      virtual ~IThresholdMonitorLoader() {};

      virtual bool load( ThresholdMonitor& data ) = 0;
   };

}

#endif
