// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
      static const int fgCaloClusterOff;
      static const int fgCaloJetOff;
      static const int fgCaloEtSumOff;
      static const int fgCaloEtMissOff;
      static const int fgCaloJetEtOff;
      static const int fgCaloIsolationOff;

   public:
      static void setCaloInfo(const CaloInfo& ci);
      static float caloGlobalScale();
      static float caloGlobalScaleGeVToCount();
      static float caloGlobalScaleCountToGeV();
      
   public:
      TriggerThresholdValue();
      virtual ~TriggerThresholdValue() = default;

      virtual TriggerThresholdValue* createCopy() const = 0;

      // getters
      const std::string& type() const { return m_Type; }
      float        ptcut() const { return m_Ptcut; }
      float        priority() const { return m_Priority; }
      int          ptCutCount() const { return thresholdValueCount(); }
      virtual int  thresholdValueCount() const;
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
      std::string  m_Type;
      float        m_Ptcut;
      unsigned int m_Priority;
      unsigned int m_Window;
      JetWindowSize::Size m_WindowSize; // mutable as long as we calculate it on the fly
      int          m_PhiMin;
      int          m_PhiMax;
      int          m_EtaMin;
      int          m_EtaMax;
   };

}

#endif 
  
