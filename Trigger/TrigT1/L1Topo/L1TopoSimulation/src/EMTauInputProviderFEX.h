// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
     
      ServiceHandle<ITHistSvc> m_histSvc;

      mutable LockedHandle<TH1> m_hEMEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEMPhiEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEMPhiEta_local ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEMEtEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEMEtPhi ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hTauEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hTauIsolation ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauPhiEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauEtEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauEtPhi ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauEtIsolation ATLAS_THREAD_SAFE;

     SG::ReadHandleKey<xAOD::eFexEMRoIContainer> m_eEM_EDMKey {this, "L1_eEMRoI", "L1_eEMRoI", "eFEXEM EDM"};
     SG::ReadHandleKey<xAOD::eFexTauRoIContainer> m_eTau_EDMKey {this, "L1_eTauRoI", "L1_eTauRoI", "eFEXTau EDM"};
   };
}

#endif
