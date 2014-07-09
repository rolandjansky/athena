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

#ifndef TrigConf_ICTPFilesLoader
#define TrigConf_ICTPFilesLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class CTPFiles;
  
   class ICTPFilesLoader : virtual public ILoader {
   public:

      virtual ~ICTPFilesLoader() {};

      virtual bool load( CTPFiles& data ) = 0;
   };

}

#endif
