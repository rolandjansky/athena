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

#ifndef TrigConf_ICTPConfigLoader
#define TrigConf_ICTPConfigLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class CTPConfig;
  
   class ICTPConfigLoader : virtual public ILoader {
   public:

      virtual ~ICTPConfigLoader() {};

      virtual bool load( CTPConfig& data ) = 0;
   };

}

#endif
