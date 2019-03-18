// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_CaloInfo
#define TrigConf_CaloInfo

#include <string>
#include <vector>
#include "TrigConfL1Data/L1DataBaseclass.h"
#include "TrigConfL1Data/CaloSinCos.h"
#include "TrigConfL1Data/METSigParam.h"
#include "TrigConfL1Data/IsolationParam.h"

namespace TrigConf {

   struct MinTOBPt {
      MinTOBPt() {}
      MinTOBPt( unsigned int ptmin,
                int32_t etamin,
                int32_t etamax,
                unsigned int priority) : 
         ptmin( ptmin ), etamin( etamin ), etamax( etamax ), priority( priority )
      {}

      unsigned int ptmin {0};
      int32_t etamin {-49};
      int32_t etamax {49};
      unsigned int priority {0};
   };


   class CaloInfo : public L1DataBaseclass {
      
   public:
      CaloInfo();
      virtual ~CaloInfo() = default;

      // getters
      double                             globalScale() const { return m_GlobalScale; }
      float                              globalEmScale() const { return m_GlobalEmScale; }
      float                              globalJetScale() const { return m_GlobalJetScale; }

      const std::vector<int>&            jetWeights() const { return m_JetWeights; }

      const std::vector<CaloSinCos>&     caloSinCos() const { return m_CaloSinCos; }

      METSigParam&                       metSigParam() { return m_METSigParam; }
      const METSigParam&                 metSigParam() const { return m_METSigParam; }

      const std::vector<IsolationParam>& isolationHAIsoForEMthr()  const { return m_IsolationHAIsoForEMthr;; }
      const std::vector<IsolationParam>& isolationEMIsoForEMthr()  const { return m_IsolationEMIsoForEMthr;; }
      const std::vector<IsolationParam>& isolationEMIsoForTAUthr() const { return m_IsolationEMIsoForTAUthr; }

      unsigned int                       jetWindowSizeSmall() const { return m_JetWindowSizeSmall; }
      unsigned int                       jetWindowSizeLarge() const { return m_JetWindowSizeLarge; }

      const MinTOBPt &                   minTobEM() const { return m_tobEM; }
      const MinTOBPt &                   minTobTau() const { return m_tobTau; }
      const MinTOBPt &                   minTobJetSmall() const { return m_tobJetSmall; }
      const MinTOBPt &                   minTobJetLarge() const { return m_tobJetLarge; }

      // setters
      void setGlobalScale(double val) { m_GlobalScale = val; }
      void setGlobalEmScale(float val) { m_GlobalEmScale = val; }
      void setGlobalJetScale(float val) { m_GlobalJetScale = val; }
      void addJetWeight(int);
      void addCaloSinCos(const CaloSinCos&);

      void setIsolation(const std::string & isothrtype, unsigned int bit, const IsolationParam & isopar);

      void setMinTobEM(const MinTOBPt & m) { m_tobEM = m; }
      void setMinTobTau(const MinTOBPt & m) { m_tobTau = m; }
      void setMinTobJetSmall(const MinTOBPt & m) { m_tobJetSmall = m; }
      void setMinTobJetLarge(const MinTOBPt & m) { m_tobJetLarge = m; }

      void setJetWindowSizeSmall(unsigned int window) { m_JetWindowSizeSmall = window; }
      void setJetWindowSizeLarge(unsigned int window) { m_JetWindowSizeLarge = window; }

      
      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

      void clear();
      
   protected:
      float                   m_GlobalScale {1};
      float                   m_GlobalEmScale {1};
      float                   m_GlobalJetScale {1};
      std::vector<int>        m_JetWeights;
      std::vector<CaloSinCos> m_CaloSinCos;
      METSigParam             m_METSigParam;

      std::vector<IsolationParam>  m_IsolationHAIsoForEMthr;
      std::vector<IsolationParam>  m_IsolationEMIsoForEMthr;
      std::vector<IsolationParam>  m_IsolationEMIsoForTAUthr;

      MinTOBPt                m_tobEM;
      MinTOBPt                m_tobTau;
      MinTOBPt                m_tobJetSmall;
      MinTOBPt                m_tobJetLarge;

      unsigned int            m_JetWindowSizeSmall {4};
      unsigned int            m_JetWindowSizeLarge {8};

   };

}

#endif 
  
