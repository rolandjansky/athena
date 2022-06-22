/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOSIMULATION_GFEXINPUTPROVIDER_H
#define L1TOPOSIMULATION_GFEXINPUTPROVIDER_H

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

      StatusCode fillXEJWOJ(TCS::TopoInputEvent& inputEvent) const;
      StatusCode fillMHT(TCS::TopoInputEvent& inputEvent) const;
      StatusCode fillXENC(TCS::TopoInputEvent& inputEvent) const;
      StatusCode fillXERHO(TCS::TopoInputEvent& inputEvent) const;

      StatusCode fillTE(TCS::TopoInputEvent& inputEvent) const;

      ServiceHandle<ITHistSvc> m_histSvc;

      SG::ReadHandleKey<xAOD::gFexJetRoIContainer> m_gJet_EDMKey {this, "gFexSRJetRoIKey", "L1_gFexSRJetRoI", "gFEX Jet EDM"};
      SG::ReadHandleKey<xAOD::gFexJetRoIContainer> m_gLJet_EDMKey {this, "gFexLRJetRoIKey", "L1_gFexLRJetRoI", "gFEX LJet EDM"};

      SG::ReadHandleKey<xAOD::gFexGlobalRoIContainer> m_gXEJWOJ_EDMKey {this, "gFexXEJWOJRoIKey", "L1_gMETComponentsJwoj", "gFEX XEJWOJ EDM"};
      SG::ReadHandleKey<xAOD::gFexGlobalRoIContainer> m_gMHT_EDMKey {this, "gFexMHTRoIKey", "L1_gMHTComponentsJwoj", "gFEX MHT EDM"};
      SG::ReadHandleKey<xAOD::gFexGlobalRoIContainer> m_gXENC_EDMKey {this, "gFexXENCRoIKey", "L1_gMETComponentsNoiseCut", "gFEX XENC EDM"};
      SG::ReadHandleKey<xAOD::gFexGlobalRoIContainer> m_gXERHO_EDMKey {this, "gFexXERHORoIKey", "L1_gMETComponentsRms", "gFEX XERHO EDM"};

      SG::ReadHandleKey<xAOD::gFexGlobalRoIContainer> m_gTE_EDMKey {this, "gFexTERoIKey", "L1_gScalarEJwoj", "gFEX TE EDM"};

      mutable LockedHandle<TH1> m_h_gJet_Pt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_gJet_PhiEta ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_gLJet_Pt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_h_gLJet_PhiEta ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_gXEJWOJ_Pt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_gXEJWOJ_Phi ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_gMHT_Pt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_h_gMHT_Phi ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_gXENC_Pt ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_gXERHO_Pt ATLAS_THREAD_SAFE;

      mutable LockedHandle<TH1> m_h_gTE_sumEt ATLAS_THREAD_SAFE;

      // gFex to L1Topo conversion factors
      static const int m_EtJet_conversion;
      static const double m_EtGlobal_conversion;
      static const int m_phi_conversion;
      static const int m_eta_conversion;
 
      static const double m_EtDoubleJet_conversion;
      static const double m_EtDoubleGlobal_conversion;
      static const double m_phiDouble_conversion;
      static const double m_etaDouble_conversion;

   };
}

#endif
