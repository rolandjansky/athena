/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XsThresholdValue
#define TrigConf_XsThresholdValue

#include "TrigConfL1Data/TriggerThresholdValue.h"

namespace TrigConf {
   class XsThresholdValue : public TriggerThresholdValue {
   public:
      XsThresholdValue();
      ~XsThresholdValue() = default;
      
      TriggerThresholdValue* createCopy() const;

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
   };
}

#endif
  
