/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_TriggerThresholdValue
#define TrigConf_TriggerThresholdValue

#include "TrigConfL1Data/L1DataBaseclass.h"

#include <string>

namespace TrigConf {
  
   namespace JetWindowSize {
      enum Size { NONE, SMALL, LARGE, UNDEF };
   }

   class CaloInfo;

   class TriggerThresholdValue : public L1DataBaseclass  {
   public:
      static const int fgCaloClusterOff   = 0xff; // 255
      static const int fgCaloJetOff       = 0x3ff; // 1023
      static const int fgCaloEtSumOff     = 0x3fff; // 16383
      static const int fgCaloEtMissOff    = 0xb4f; // 2895
      static const int fgCaloJetEtOff     = 0x33e6; // 13286
      static const int fgCaloIsolationOff = 0x3f; // 63

   public:
      static void setCaloInfo(const CaloInfo& ci);
      static float caloGlobalScale();
      static float caloGlobalScaleGeVToCount();
      static float caloGlobalScaleCountToGeV();
      
   public:
      TriggerThresholdValue();
      virtual ~TriggerThresholdValue();

      virtual TriggerThresholdValue* createCopy() const = 0;

      // getters
      const std::string& type() const { return m_Type; }
      float        ptcut() const { return m_Ptcut; }
      float        priority() const { return m_Priority; }
      int          ptCutCount() const { return thresholdValueCount(); }
      int          thresholdValueCount() const;
      unsigned int window() const { return m_Window; }
      virtual JetWindowSize::Size windowSize() const { return m_WindowSize; }
      int        phimin() const { return m_PhiMin; }
      int        phimax() const { return m_PhiMax; }
      int        etamin() const { return m_EtaMin; }
      int        etamax() const { return m_EtaMax; }

      // setters
      void setType(const std::string& type) { m_Type = type; }
      void setPtcut(float pt) { m_Ptcut = pt; }
      void setPriority(float prio) { m_Priority = prio; }
      void setWindow( int value ) { m_Window = value; }
      void setPhiMin( int value ) { m_PhiMin = value; }
      void setPhiMax( int value ) { m_PhiMax = value; }
      void setEtaMin( int value ) { m_EtaMin = value; }
      void setEtaMax( int value ) { m_EtaMax = value; }

      // geometrical check
      bool contains(float eta, float phi) const;

      // access to global Calo configuration
      const CaloInfo & caloInfo() const;

      virtual void print(const std::string& indent="", unsigned int detail=1) const = 0;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
    
   protected:
      std::string  m_Type {""};
      float        m_Ptcut {0};
      unsigned int m_Priority {0};
      unsigned int m_Window {0};
      JetWindowSize::Size m_WindowSize { JetWindowSize::NONE }; // mutable as long as we calculate it on the fly
      int          m_PhiMin {0};
      int          m_PhiMax {64};
      int          m_EtaMin {-49};
      int          m_EtaMax {49};
   };

}

#endif 
  
