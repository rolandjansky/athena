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

#ifndef TrigConf_IBunchGroupSetLoader
#define TrigConf_IBunchGroupSetLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class BunchGroupSet;
  
   class IBunchGroupSetLoader : virtual public ILoader {
   public:

      virtual ~IBunchGroupSetLoader() {};

      virtual bool load( BunchGroupSet& data ) = 0;
   };

}

#endif
