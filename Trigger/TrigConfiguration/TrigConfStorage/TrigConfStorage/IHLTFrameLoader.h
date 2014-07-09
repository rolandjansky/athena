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

#ifndef TrigConf_IHLTFrameLoader
#define TrigConf_IHLTFrameLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class HLTFrame;
  
   class IHLTFrameLoader : virtual public ILoader {
   public:

      virtual ~IHLTFrameLoader() {};

      virtual bool load( HLTFrame& data ) = 0;
   };

}

#endif
