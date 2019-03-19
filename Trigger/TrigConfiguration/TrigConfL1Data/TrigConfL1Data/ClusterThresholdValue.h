/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ClusterThresholdValue
#define TrigConf_ClusterThresholdValue

#include "TrigConfL1Data/TriggerThresholdValue.h"

namespace TrigConf {
  
   class ClusterThresholdValue : public TriggerThresholdValue {
   public:
      ClusterThresholdValue();
      ~ClusterThresholdValue() = default;

      virtual TriggerThresholdValue* createCopy() const override;
      
      // getters
      float emIsolation() const  { return m_EmIsolation; } // deprecated
      float hadIsolation() const { return m_HadIsolation; } // deprecated
      float hadVeto() const      { return m_HadVeto; } // deprecated

      int emIsolationCount() const;
      int hadIsolationCount() const;
      int hadVetoCount() const;

      // run 2 isolation
      bool     useIsolationMask() const { return m_useIsolationMask; }
      uint16_t emIsolationMask() const { return m_IsolationMask; } // deprecated
      uint16_t hadIsolationMask() const { return m_HadIsolationMask; } // deprecated
      uint16_t isolationMask() const { return m_IsolationMask; }

      virtual int thresholdValueCount() const override;

      // setters
      void setEmIsolation(float v)  { m_EmIsolation = v; }   // deprecated
      void setHadIsolation(float v) { m_HadIsolation = v; }  // deprecated
      void setHadVeto(float v)      { m_HadVeto = v; } // deprecated

      // run 2 isolation
      void setUseIsolationMask(bool use=true) { m_useIsolationMask = use; }
      void setEmIsolationMask(uint16_t mask) { m_IsolationMask = mask; }   // deprecated
      void setHadIsolationMask(uint16_t mask) { m_HadIsolationMask = mask; } // deprecated
      void setIsolationMask(uint16_t mask) { m_IsolationMask = mask; }

      virtual void print(const std::string& indent="", unsigned int detail=1) const override;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const override;
      
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
      uint16_t  m_HadIsolationMask; // deprecated

      bool  m_useIsolationMask;
   };

} 

#endif   
