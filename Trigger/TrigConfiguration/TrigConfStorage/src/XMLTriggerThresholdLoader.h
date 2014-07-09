/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLTriggerThresholdLoader
#define TrigConf_XMLTriggerThresholdLoader

#include "TrigConfStorage/ITriggerThresholdLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   class XMLTriggerThresholdLoader : virtual public ITriggerThresholdLoader, public XMLLoader {
   public:
      
      XMLTriggerThresholdLoader(XMLStorageMgr& sm, value_type* pt = 0) : 
         XMLLoader("XMLTriggerThresholdLoader", sm, pt)
      {};

      virtual ~XMLTriggerThresholdLoader(){};
      
      virtual bool load( TriggerThreshold& data);
   };
}

#endif

