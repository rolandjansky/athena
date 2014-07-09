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

#ifndef TrigConf_ICaloSinCosLoader
#define TrigConf_ICaloSinCosLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class CaloSinCos;
  
   class ICaloSinCosLoader : virtual public ILoader {
   public:

      virtual ~ICaloSinCosLoader() {};

      virtual bool load( CaloSinCos& data ) = 0;
   };

}

#endif
