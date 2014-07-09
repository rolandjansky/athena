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

#ifndef TrigConf_IPrescaledClockLoader
#define TrigConf_IPrescaledClockLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class PrescaledClock;
  
   class IPrescaledClockLoader : virtual public ILoader {
   public:

      virtual ~IPrescaledClockLoader() {};

      virtual bool load( PrescaledClock& data ) = 0;
   };

}

#endif
