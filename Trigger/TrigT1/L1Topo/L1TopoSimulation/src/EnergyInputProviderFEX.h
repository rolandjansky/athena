/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EnergyInputProviderFEX
#define L1TopoSimulation_EnergyInputProviderFEX

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/LockedHandle.h"
#include "xAODTrigger/EnergySumRoI.h"

#include "TH1.h"

class ITHistSvc;

namespace LVL1 {

   class EnergyInputProviderFEX : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      EnergyInputProviderFEX(const std::string& type, const std::string& name, 
                          const IInterface* parent);
      
      virtual ~EnergyInputProviderFEX();

      virtual StatusCode initialize() override final;

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const override final; 

      virtual void handle(const Incident&) override final;

   private:

      ServiceHandle<ITHistSvc> m_histSvc;

      StringProperty m_gFEXMETLoc;    //!<  EnergyROI SG key

      mutable LockedHandle<TH1> m_hPt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hPhi ATLAS_THREAD_SAFE;

   };
}

#endif
