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

#ifndef TrigConf_ITriggerThresholdLoader
#define TrigConf_ITriggerThresholdLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class TriggerThreshold;
  
   class ITriggerThresholdLoader : virtual public ILoader {
   public:

      virtual ~ITriggerThresholdLoader() {};

      virtual bool load( TriggerThreshold& data ) = 0;
   };

}

#endif
