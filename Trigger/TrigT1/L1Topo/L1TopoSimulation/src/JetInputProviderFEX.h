/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JETINPUTPROVIDERFEX_H
#define JETINPUTPROVIDERFEX_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/LockedHandle.h"

#include "xAODTrigger/jFexSRJetRoIContainer.h" //jets from gFEX

#include "TH1.h"
#include "TH2.h"

class ITHistSvc;

namespace LVL1 {

   class JetInputProviderFEX : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      JetInputProviderFEX(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~JetInputProviderFEX();

      virtual StatusCode initialize() override final;
      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const override final; 

      virtual void handle(const Incident&) override final;

   private:

      ServiceHandle<ITHistSvc> m_histSvc;

      StringProperty m_gFEXJetLoc {""};

      mutable LockedHandle<TH1> m_hPt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEtaPhi ATLAS_THREAD_SAFE;

      SG::ReadHandleKey<xAOD::jFexSRJetRoIContainer> m_jEDMKey {this, "L1_jFexSRJetRoI", "L1_jFexSRJetRoI", "jFEX EDM"};
   };
}

#endif
