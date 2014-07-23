/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_JetThresholdValue
#define TrigConf_JetThresholdValue

#include "TrigConfL1Data/TriggerThresholdValue.h"

namespace TrigConf {
  
   class JetThresholdValue : public TriggerThresholdValue {
   public:

      JetThresholdValue();
      ~JetThresholdValue();
      
      virtual TriggerThresholdValue* createCopy() const;

      virtual int thresholdValueCount() const;

      virtual JetWindowSize::Size windowSize() const;

      std::string windowSizeAsString() const;

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

   private:

   };

}

#endif 
  
