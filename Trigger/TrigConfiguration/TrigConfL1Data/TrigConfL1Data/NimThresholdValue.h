/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_NimThresholdValue
#define TrigConf_NimThresholdValue

#include "TrigConfL1Data/TriggerThresholdValue.h"

namespace TrigConf {
   class NimThresholdValue : public TriggerThresholdValue {
   public:
      NimThresholdValue();
      ~NimThresholdValue() = default;

      virtual TriggerThresholdValue* createCopy() const;

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
   };
}
#endif
  
