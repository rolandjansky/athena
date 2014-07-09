/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLDeadTimeLoader
#define TrigConf_XMLDeadTimeLoader

#include "TrigConfStorage/IDeadTimeLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   class XMLDeadTimeLoader : virtual public IDeadTimeLoader, public XMLLoader {
   public:

      XMLDeadTimeLoader(XMLStorageMgr& sm, const ptree::value_type* pt) : 
         XMLLoader("XMLDeadTimeLoader", sm, pt) {};

      virtual ~XMLDeadTimeLoader(){};

      virtual bool load( DeadTime& data);
   };

}

#endif

