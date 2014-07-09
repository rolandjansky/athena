/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ILutCamLoader
#define TrigConf_ILutCamLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfL1Data/LutCam.h"

namespace TrigConf {

   class ILutCamLoader : virtual public ILoader {
   public:
      virtual ~ILutCamLoader() {};
      virtual bool load( LutCam& data ) = 0;
   };

}

#endif
