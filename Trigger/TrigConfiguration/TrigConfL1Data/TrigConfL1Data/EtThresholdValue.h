/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_EtThresholdValue
#define TrigConf_EtThresholdValue

#include "TrigConfL1Data/TriggerThresholdValue.h"

namespace TrigConf {
   class EtThresholdValue : public TriggerThresholdValue {
   public:
      EtThresholdValue();
      ~EtThresholdValue();
      
      virtual TriggerThresholdValue* createCopy() const;

      virtual int thresholdValueCount() const;
      
      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
   };
}

#endif
  
