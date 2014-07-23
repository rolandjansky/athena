/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ClusterThresholdValue
#define TrigConf_ClusterThresholdValue

#include "TrigConfL1Data/TriggerThresholdValue.h"

namespace TrigConf {
  
   class ClusterThresholdValue : public TriggerThresholdValue {
   public:
      ClusterThresholdValue();
      ~ClusterThresholdValue();

      virtual TriggerThresholdValue* createCopy() const;
      
      // getters
      float emIsolation() const  { return m_EmIsolation; }
      float hadIsolation() const { return m_HadIsolation; }
      float hadVeto() const      { return m_HadVeto; }

      uint16_t isolationMask() { return m_IsolationMask; }

      int emIsolationCount() const;
      int hadIsolationCount() const;
      int hadVetoCount() const;

      virtual int thresholdValueCount() const;

      // setters
      void setEmIsolation(float v)  { m_EmIsolation = v; }
      void setHadIsolation(float v) { m_HadIsolation = v; }
      void setHadVeto(float v)      { m_HadVeto = v; }

      void setIsolationMask(uint16_t mask) { m_IsolationMask = mask; }

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
      
   private:
      float m_EmIsolation;
      float m_HadIsolation;
      float m_HadVeto;      

      /**
       * new isolation for run 2: bit mask to select the isolation parameterizations to be applied
       * 
       * The mask should be only setting the 5 lowests bits 0 to 4
       * A bit mask is prefered to a bit index as we might want to apply more than one parameterizations (Alan)
      ***/
      uint16_t  m_IsolationMask;

   };

} 

#endif   
