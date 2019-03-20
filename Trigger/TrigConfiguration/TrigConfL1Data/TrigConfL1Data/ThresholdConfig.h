// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ThresholdConfig
#define TrigConf_ThresholdConfig

#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/L1DataBaseclass.h"
#include "TrigConfL1Data/L1DataDef.h"

namespace TrigConf {

   class ThresholdConfig : public L1DataBaseclass {
   public:

      typedef std::vector<TriggerThreshold*> thrVec_t;
      typedef std::map< L1DataDef::TriggerType, thrVec_t> thrVecMap_t;

      ThresholdConfig();
      ~ThresholdConfig() = default;
      
      std::vector<TriggerThreshold*>& thresholdVector(L1DataDef::TriggerType);
      const std::vector<TriggerThreshold*>& getThresholdVector(L1DataDef::TriggerType) const;

      const std::vector<TriggerThreshold*>& getThresholdVector() const { return m_TriggerThresholdVector; }
      const std::vector<TriggerThreshold*>& thresholdVector()    const { return m_TriggerThresholdVector; } // backward compatible

      //access functions
      std::string getClusterThresholdType( int thresholdnumber ) const;

      // access to ptcut
      float getThreshold(L1DataDef::TriggerType type, int eta, int phi, int thresholdnumber) const;

      // access to window
      int   getWindow(L1DataDef::TriggerType type, int eta, int phi, int thresholdnumber) const;

      size_t size() const { return getThresholdVector().size(); }
      size_t size(L1DataDef::TriggerType type) const { return getThresholdVector(type).size(); }
 
      // and specialized (should be faced out)
      float getClusterThreshold(int eta, int phi, int thresholdnumber) const;
      float getJetThreshold(int eta, int phi, int thresholdnumber) const;
      float getJbThreshold(int eta, int phi, int thresholdnumber) const;
      float getJfThreshold(int eta, int phi, int thresholdnumber) const;
      float getMuonThreshold(int eta, int phi, int thresholdnumber) const;
      float getTotEtThreshold(int eta, int phi, int thresholdnumber) const;
      float getJetEtThreshold(int eta, int phi, int thresholdnumber) const;
      float getMissEtThreshold(int eta, int phi, int thresholdnumber) const;
      float getMissEtSigThreshold(int eta, int phi, int thresholdnumber) const;
      float getClusterEmIsolation(int eta, int phi, int thresholdnumber) const;
      float getClusterHadIsolation(int eta, int phi, int thresholdnumber) const;
      float getClusterHadVeto(int eta, int phi, int thresholdnumber) const;
      int   getJetWindow(int eta, int phi, int thresholdnumber) const;
      int   getJbWindow(int eta, int phi, int thresholdnumber) const;
      int   getJfWindow(int eta, int phi, int thresholdnumber) const;

      const CaloInfo& caloInfo() const { return m_CaloInfo; }
      void setCaloInfo (const CaloInfo& ci) { m_CaloInfo = ci; }

      bool addTriggerThreshold(TriggerThreshold* value); // add according to type

      virtual void print(const std::string& indent="", unsigned int detail=1) const;

      void printTriggerThresholdVector(const std::string& indent="") const;

      void printThresholdValueMap() const;
      void printTtvMap(const TriggerThreshold* thr) const;
      void printTtvSummary(const thrVec_t& vec, const std::string& name) const;

      void clear();

      void attributeThresholdNumbers(); 

      TriggerThreshold* findTriggerThreshold(unsigned int id);

      void setCtpVersion(unsigned int ctpVersion) { m_ctpVersion = ctpVersion; }
      unsigned int ctpVersion() const { return m_ctpVersion; }

      void setL1Version(unsigned int l1Version) { m_l1Version = l1Version; }
      unsigned int l1Version() const { return m_l1Version; }

   private:

      bool insertInPosition(std::vector<TriggerThreshold*>& thrVec, TriggerThreshold* tt, unsigned int pos);

      std::vector<TriggerThreshold*> getNimThresholdVectorByType(const std::string& type) const;

   private:
      friend class CTPConfig;

      unsigned int m_ctpVersion;
      unsigned int m_l1Version;
      thrVecMap_t  m_thresholdVectors;        // vectors of TriggerThresholds accessed by type (map)
      thrVec_t     m_TriggerThresholdVector;  // vector of all TriggerThresholds (owns all thresholds)
      thrVec_t     m_ClusterThresholdVector;  // vector of all EM TAU thresholds (for backward compatibility, where EM and TAU thresholds are in one vector)
      CaloInfo     m_CaloInfo;
      

   public:
      // these need to stay here temporarily to make the migration not too painfull
      const std::vector<TriggerThreshold*>& getClusterThresholdVector() const { return m_ClusterThresholdVector; }
      const std::vector<TriggerThreshold*>& getJetThresholdVector()     const { return getThresholdVector(L1DataDef::JET); }
      const std::vector<TriggerThreshold*>& getFJetThresholdVector()    const { return getThresholdVector(L1DataDef::FJ); }
      const std::vector<TriggerThreshold*>& getJbThresholdVector()      const { return getThresholdVector(L1DataDef::JB); }
      const std::vector<TriggerThreshold*>& getJfThresholdVector()      const { return getThresholdVector(L1DataDef::JF); }
      const std::vector<TriggerThreshold*>& getTotEtVector()            const { return getThresholdVector(L1DataDef::TE); }
      const std::vector<TriggerThreshold*>& getJetEtVector()            const { return getThresholdVector(L1DataDef::JE); }
      const std::vector<TriggerThreshold*>& getMissEtVector()           const { return getThresholdVector(L1DataDef::XE); }
      const std::vector<TriggerThreshold*>& getMissEtSignVector()       const { return getThresholdVector(L1DataDef::XS); }
      const std::vector<TriggerThreshold*>& getMuonThresholdVector()    const { return getThresholdVector(L1DataDef::MUON); }
      const std::vector<TriggerThreshold*>& getRndmThresholdVector()    const { return getThresholdVector(L1DataDef::RNDM); }
      const std::vector<TriggerThreshold*>& getBgrpThresholdVector()    const { return getThresholdVector(L1DataDef::BGRP); }
      const std::vector<TriggerThreshold*>& getPclkThresholdVector()    const { return getThresholdVector(L1DataDef::PCLK); }
      const std::vector<TriggerThreshold*>& getNimThresholdVector()     const { return getThresholdVector(L1DataDef::NIM); }
      const std::vector<TriggerThreshold*>& getZBThresholds()           const { return getThresholdVector(L1DataDef::ZB);}
      const std::vector<TriggerThreshold*>& getMbtsThresholdVector()    const { return getThresholdVector(L1DataDef::MBTS); }
      const std::vector<TriggerThreshold*>& getMbtssiThresholdVector()  const { return getThresholdVector(L1DataDef::MBTSSI); }
      const std::vector<TriggerThreshold*>& getTrtThresholdVector()     const { return getThresholdVector(L1DataDef::TRT); }
      const std::vector<TriggerThreshold*>& getZdcThresholdVector()     const { return getThresholdVector(L1DataDef::ZDC); }
      const std::vector<TriggerThreshold*>& getBcmThresholdVector()     const { return getThresholdVector(L1DataDef::BCM); }
      const std::vector<TriggerThreshold*>& getBcmcmbThresholdVector()  const { return getThresholdVector(L1DataDef::BCMCMB); }
      const std::vector<TriggerThreshold*>& getLucidThresholdVector()   const { return getThresholdVector(L1DataDef::LUCID); }
      const std::vector<TriggerThreshold*>& getBptxThresholdVector()    const { return getThresholdVector(L1DataDef::BPTX); }
 
      #ifndef __GCCXML__

      const std::vector<TriggerThreshold*>* getClusterThresholdPointer() const __attribute__ ((deprecated)) { return &m_ClusterThresholdVector;}
      const std::vector<TriggerThreshold*>* getJetThresholdPointer()     const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::JET);}
      const std::vector<TriggerThreshold*>* getFJetThresholdPointer()    const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::FJ);}
      const std::vector<TriggerThreshold*>* getJbThresholdPointer()      const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::JB);}
      const std::vector<TriggerThreshold*>* getJfThresholdPointer()      const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::JF);}
      const std::vector<TriggerThreshold*>* getTotEtPointer()            const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::TE);}
      const std::vector<TriggerThreshold*>* getJetEtPointer()            const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::JE);}
      const std::vector<TriggerThreshold*>* getMissEtPointer()           const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::XE);}
      const std::vector<TriggerThreshold*>* getMissEtSignPointer()       const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::XS);}
      const std::vector<TriggerThreshold*>* getMuonThresholdPointer()    const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::MUON);}
      const std::vector<TriggerThreshold*>* getRndmThresholdPointer()    const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::RNDM);}
      const std::vector<TriggerThreshold*>* getBgrpThresholdPointer()    const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::BGRP);}
      const std::vector<TriggerThreshold*>* getPclkThresholdPointer()    const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::PCLK);}
      const std::vector<TriggerThreshold*>* getNimThresholdPointer()     const __attribute__ ((deprecated)) { return &getThresholdVector(L1DataDef::NIM);}

      #endif
   };
}

#endif
