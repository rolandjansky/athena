/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IL1TopoMenuLoader
#define TrigConf_IL1TopoMenuLoader

#include "TrigConfStorage/ILoader.h"

namespace TXC {
   class L1TopoMenu;
}

namespace TrigConf {

   class IL1TopoMenuLoader : virtual public ILoader {
   public:
      virtual bool load( TXC::L1TopoMenu& data ) = 0;
   };

}

#endif
