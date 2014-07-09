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

#ifndef TrigConf_ICaloInfoLoader
#define TrigConf_ICaloInfoLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class CaloInfo;
  
   class ICaloInfoLoader : virtual public ILoader {
   public:

      virtual ~ICaloInfoLoader() {};

      virtual bool load( CaloInfo& data ) = 0;
   };

}

#endif
