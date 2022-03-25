/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_gFexInputProvider
#define L1TopoSimulation_gFexInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/LockedHandle.h"

// gFEX EDMs
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/gFexGlobalRoIContainer.h"

#include "TH1.h"
#include "TH2.h"

class ITHistSvc;

namespace LVL1 {

   class gFexInputProvider : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      gFexInputProvider(const std::string& type, const std::string& name, const IInterface* parent);
      
      virtual ~gFexInputProvider() = default;

      virtual StatusCode initialize() override final;
      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const override final;

      virtual void handle(const Incident&) override final;

   private:

      StatusCode fillSRJet(TCS::TopoInputEvent& inputEvent) const;
      StatusCode fillLRJet(TCS::TopoInputEvent& inputEvent) const;

      ServiceHandle<ITHistSvc> m_histSvc;

      SG::ReadHandleKey<xAOD::gFexJetRoIContainer> m_gJet_EDMKey {this, "gFexSRJetRoIKey", "L1_gFexSRJetRoI", "gFEX Jet EDM"};
      SG::ReadHandleKey<xAOD::gFexJetRoIContainer> m_gLJet_EDMKey {this, "gFexLRJetRoIKey", "L1_gFexLRJetRoI", "gFEX LJet EDM"};

      mutable LockedHandle<TH1> m_h_gJet_Pt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_gJet_PhiEta ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_gLJet_Pt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_gLJet_PhiEta ATLAS_THREAD_SAFE;

      // gFex to L1Topo conversion factors
      static const int m_Et_conversion;
      static const int m_phi_conversion;
      static const int m_eta_conversion;
 
      static const float m_EtDouble_conversion;
      static const float m_phiDouble_conversion;
      static const float m_etaDouble_conversion;

   };
}

#endif
