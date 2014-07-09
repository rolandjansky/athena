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

#ifndef TrigConf_IMuctpiLoader
#define TrigConf_IMuctpiLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfL1Data/Muctpi.h"

namespace TrigConf {

   class IMuctpiLoader : virtual public ILoader {
   public:

      virtual ~IMuctpiLoader() {};

      virtual bool load( Muctpi& data ) = 0;
   };

}

#endif
