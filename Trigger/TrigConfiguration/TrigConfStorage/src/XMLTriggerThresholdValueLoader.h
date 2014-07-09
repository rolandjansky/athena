/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLTriggerThresholdValueLoader
#define TrigConf_XMLTriggerThresholdValueLoader

#include "TrigConfStorage/ITriggerThresholdValueLoader.h"
#include "TrigConfStorage/XMLLoader.h"

#include <string>

namespace TrigConf {

  /**@brief TriggerDB loader of the LVL1 trigger threshold values*/
  class XMLTriggerThresholdValueLoader : virtual public ITriggerThresholdValueLoader, public XMLLoader {
  public:

    XMLTriggerThresholdValueLoader(XMLStorageMgr& sm, value_type* pt = 0) :
       XMLLoader("XMLTriggerThresholdValueLoader", sm, pt) {};

    virtual ~XMLTriggerThresholdValueLoader(){};

    virtual bool load( TriggerThresholdValue& data);

  };

}

#endif
