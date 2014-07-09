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

#ifndef TrigConf_IThresholdConfigLoader
#define TrigConf_IThresholdConfigLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class ThresholdConfig;
  
   class IThresholdConfigLoader : virtual public ILoader {
   public:

      virtual ~IThresholdConfigLoader() {};

      virtual bool load( ThresholdConfig& data ) = 0;
   };

}

#endif
