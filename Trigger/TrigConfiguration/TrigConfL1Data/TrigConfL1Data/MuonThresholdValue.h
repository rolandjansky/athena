/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MuonThresholdValue
#define TrigConf_MuonThresholdValue

#include "TrigConfL1Data/TriggerThresholdValue.h"

namespace TrigConf {
   class MuonThresholdValue : public TriggerThresholdValue {
   public:
      MuonThresholdValue();
      ~MuonThresholdValue() = default;

      virtual TriggerThresholdValue* createCopy() const;

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
   };
}
#endif
  
