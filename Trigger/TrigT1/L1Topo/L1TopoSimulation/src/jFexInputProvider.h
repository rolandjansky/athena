/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef L1TOPOSIMULATION_JFEXINPUTPROVIDER_H
#define L1TOPOSIMULATION_JFEXINPUTPROVIDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/LockedHandle.h"

// jFEX EDMs
#include "xAODTrigger/jFexSRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/jFexTauRoIContainer.h"
#include "xAODTrigger/jFexMETRoIContainer.h"
#include "xAODTrigger/jFexSumETRoIContainer.h"
#include "xAODTrigger/jFexFwdElRoIContainer.h"

#include "TH1.h"
#include "TH2.h"

class ITHistSvc;

namespace LVL1 {

   class jFexInputProvider : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      jFexInputProvider(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~jFexInputProvider();

      virtual StatusCode initialize() override final;
      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const override final; 

      virtual void handle(const Incident&) override final;

   private:
      StatusCode fillSRJet(TCS::TopoInputEvent& inputEvent) const;
      StatusCode fillLRJet(TCS::TopoInputEvent& inputEvent) const;

      StatusCode fillEM(TCS::TopoInputEvent& inputEvent) const;
      StatusCode fillTau(TCS::TopoInputEvent& inputEvent) const;

      StatusCode fillXE(TCS::TopoInputEvent& inputEvent) const;
      StatusCode fillTE(TCS::TopoInputEvent& inputEvent) const;

      ServiceHandle<ITHistSvc> m_histSvc;

      StringProperty m_gFEXJetLoc {""};

      mutable LockedHandle<TH1> m_h_jJetPt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_jJetPhiEta ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_jLJetPt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_jLJetPhiEta ATLAS_THREAD_SAFE;
     
      mutable LockedHandle<TH1> m_h_jTauPt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_jTauIsolation ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_jTauPhiEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_jTauIsolationEta ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_jEmPt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_jEmIsolation ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_jEmFrac1 ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_jEmFrac2 ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_jEmPhiEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_jEmIsolationEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_jEmFrac1Eta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_jEmFrac2Eta ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_jXE_Pt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_jXE_Phi ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_jTE_sumEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_jTEC_sumEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_jTEFWD_sumEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_jTEFWDA_sumEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_jTEFWDC_sumEt ATLAS_THREAD_SAFE;

      SG::ReadHandleKey<xAOD::jFexSRJetRoIContainer> m_jJet_EDMKey {this, "jFexSRJetRoIKey", "L1_jFexSRJetRoI", "jFEX Jet EDM"};
      SG::ReadHandleKey<xAOD::jFexLRJetRoIContainer> m_jLJet_EDMKey {this, "jFexLRJetRoIKey", "L1_jFexLRJetRoI", "jFEX LJet EDM"};
      SG::ReadHandleKey<xAOD::jFexFwdElRoIContainer> m_jEM_EDMKey {this, "jFexEMRoIKey", "L1_jFexFwdElRoI", "jFEX EM EDM"};     
      SG::ReadHandleKey<xAOD::jFexTauRoIContainer> m_jTau_EDMKey {this, "jFexTauRoIKey", "L1_jFexTauRoI", "jFEX Tau EDM"};
      SG::ReadHandleKey<xAOD::jFexMETRoIContainer> m_jXE_EDMKey {this, "jFexXERoIKey", "L1_jFexMETRoI", "jFEX XE EDM"};
      SG::ReadHandleKey<xAOD::jFexSumETRoIContainer> m_jTE_EDMKey {this, "jFexTERoIKey", "L1_jFexSumETRoI", "jFEX TE EDM"};
     
      // jFex to L1Topo conversion factors
      static const int m_Et_conversion;
      static const double m_sumEt_conversion;
      static const int m_phi_conversion;
      static const int m_eta_conversion;

      static const double m_EtDouble_conversion;
      static const double m_sumEtDouble_conversion;
      static const double m_phiDouble_conversion;
      static const double m_etaDouble_conversion;

  };
}

#endif
