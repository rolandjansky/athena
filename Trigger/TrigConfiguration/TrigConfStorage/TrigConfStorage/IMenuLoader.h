/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IMenuLoader
#define TrigConf_IMenuLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class Menu;
  
   class IMenuLoader : virtual public ILoader {
   public:
      typedef enum  {ALL, CTP, CTPOnl, HLT, COOLL1, COOLHLT, L1Simu} ENV;

      virtual ~IMenuLoader() {};

      virtual void setEnv(ENV env) = 0;
      virtual bool load( Menu& data ) = 0;
   };

}

#endif
