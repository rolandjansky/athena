/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_JetThresholdValue
#define TrigConf_JetThresholdValue

#include "TrigConfL1Data/TriggerThresholdValue.h"

namespace TrigConf {
  
   class JetThresholdValue : public TriggerThresholdValue {
   public:

      JetThresholdValue();
      ~JetThresholdValue() = default;
      
      virtual TriggerThresholdValue* createCopy() const override;

      virtual int thresholdValueCount() const override;

      virtual JetWindowSize::Size windowSize() const override;

      std::string windowSizeAsString() const;

      virtual void print(const std::string& indent="", unsigned int detail=1) const override;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const override;

   private:

   };

}

#endif 
  
