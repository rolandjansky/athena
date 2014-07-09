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

#ifndef TrigConf_IRandomLoader
#define TrigConf_IRandomLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class Random;
  
   class IRandomLoader : virtual public ILoader {
   public:

      virtual ~IRandomLoader() {};

      virtual bool load( Random& data ) = 0;
   };

}

#endif
