/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IMenuLoader
#define TrigConf_IMenuLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

   class Menu;
  
   class IMenuLoader : virtual public ILoader {
   public:

      virtual ~IMenuLoader() {};

      virtual bool load( Menu& data ) = 0;
   };

}

#endif
