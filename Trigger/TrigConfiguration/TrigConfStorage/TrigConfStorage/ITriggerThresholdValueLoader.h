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

#ifndef TrigConf_ITriggerThresholdValueLoader
#define TrigConf_ITriggerThresholdValueLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class TriggerThresholdValue;
  
   class ITriggerThresholdValueLoader : virtual public ILoader {
   public:

      virtual ~ITriggerThresholdValueLoader() {};

      virtual bool load( TriggerThresholdValue& data ) = 0;
   };

}

#endif
