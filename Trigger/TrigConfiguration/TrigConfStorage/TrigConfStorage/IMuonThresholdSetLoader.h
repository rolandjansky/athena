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

#ifndef TrigConf_IMuonThresholdSetLoader
#define TrigConf_IMuonThresholdSetLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class MuonThresholdSet;
  
   class IMuonThresholdSetLoader : virtual public ILoader {
   public:

      virtual ~IMuonThresholdSetLoader() {};

      virtual bool load( MuonThresholdSet& data ) = 0;
   };

}

#endif
