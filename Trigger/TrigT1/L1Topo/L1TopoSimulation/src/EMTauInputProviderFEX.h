// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoSimulation_EMTauInputProviderFEX
#define L1TopoSimulation_EMTauInputProviderFEX

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/LockedHandle.h"

//EM/Tau EDMs
#include "xAODTrigger/eFexEMRoIContainer.h"
#include "xAODTrigger/eFexTauRoIContainer.h"


#include "TH1.h"
#include "TH2.h"

class ITHistSvc;

namespace LVL1 {

   class EMTauInputProviderFEX : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      EMTauInputProviderFEX(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~EMTauInputProviderFEX();

      virtual StatusCode initialize() override final;

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const override final; 

      virtual void handle(const Incident&) override final;

   private:

      /** \brief calculates eta and phi from roiWord*/
      void CalculateCoordinates(int32_t roiWord, double & eta, double & phi) const;
     
      StatusCode fillEM(TCS::TopoInputEvent& inputEvent) const;
      StatusCode fillTau(TCS::TopoInputEvent& inputEvent) const;

      ServiceHandle<ITHistSvc> m_histSvc;

      mutable LockedHandle<TH1> m_hEmEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hEmREta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hEmRHad ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hEmWsTot ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEmPhiEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEmPhiEta_local ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEmEtEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEmEtPhi ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEmEtREta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEmEtRHad ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEmEtWsTot ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hTauEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hTauRCore ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hTauRHad ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauPhiEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauEtEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauEtPhi ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauEtRCore ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauEtRHad ATLAS_THREAD_SAFE;

     SG::ReadHandleKey<xAOD::eFexEMRoIContainer> m_eEM_EDMKey {this, "eFexEMRoIKey", "L1_eEMRoI", "eFEXEM EDM"};
     SG::ReadHandleKey<xAOD::eFexTauRoIContainer> m_eTau_EDMKey {this, "eFexTauRoIKey", "L1_eTauRoI", "eFEXTau EDM"};
   };
}

#endif
