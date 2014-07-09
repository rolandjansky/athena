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

#ifndef TrigConf_IDeadTimeLoader
#define TrigConf_IDeadTimeLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class DeadTime;
  
   class IDeadTimeLoader : virtual public ILoader {
   public:

      virtual ~IDeadTimeLoader() {};

      virtual bool load( DeadTime& data ) = 0;
   };

}

#endif
