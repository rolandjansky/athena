/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EMTauInputProviderFEX
#define L1TopoSimulation_EMTauInputProviderFEX

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/LockedHandle.h"

//EM/Tau EDMs
#include "xAODTrigger/eFexEMRoIContainer.h"


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
      int ConvertEta(const int val) const;
     
      ServiceHandle<ITHistSvc> m_histSvc;

      mutable LockedHandle<TH1> m_hEMEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEMEtaPhi ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hTauEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauEtaPhi ATLAS_THREAD_SAFE;

     SG::ReadHandleKey<xAOD::eFexEMRoIContainer> m_eEDMKey {this, "L1_eEMRoI", "L1_eEMRoI", "eFEX EDM"};
   };
}

#endif
