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

#ifndef TrigConf_IJobOptionTableLoader
#define TrigConf_IJobOptionTableLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class JobOptionTable;
  
   class IJobOptionTableLoader : virtual public ILoader {
   public:

      virtual ~IJobOptionTableLoader() {};

      virtual bool load( JobOptionTable& data ) = 0;
   };

}

#endif
