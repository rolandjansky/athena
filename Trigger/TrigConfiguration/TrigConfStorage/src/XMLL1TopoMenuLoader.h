/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLL1TopoMenuLoader
#define TrigConf_XMLL1TopoMenuLoader

#include "TrigConfStorage/IL1TopoMenuLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {
   class XMLL1TopoMenuLoader : virtual public IL1TopoMenuLoader, public XMLLoader {
   public:
      XMLL1TopoMenuLoader( XMLStorageMgr& sm, const ptree::value_type* pt ) :
         XMLLoader("XMLL1TopoMenuLoader", sm, pt)
      {};

      virtual ~XMLL1TopoMenuLoader() {};

      virtual bool load( TXC::L1TopoMenu& menu );

   };
}

#endif
