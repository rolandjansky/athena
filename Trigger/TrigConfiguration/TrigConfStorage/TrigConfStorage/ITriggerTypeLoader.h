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

#ifndef TrigConf_ITriggerTypeLoader
#define TrigConf_ITriggerTypeLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class TriggerType;
  
   class ITriggerTypeLoader : virtual public ILoader {
   public:

      virtual ~ITriggerTypeLoader() {};

      virtual bool load( TriggerType& data ) = 0;
   };

}

#endif
