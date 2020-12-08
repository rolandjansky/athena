/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EnergyInputProvider
#define L1TopoSimulation_EnergyInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/LockedHandle.h"
#include "TrigT1CaloEvent/EnergyTopoData.h"

#include "TH1.h"

class ITHistSvc;

namespace LVL1 {

   class EnergyInputProvider : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      EnergyInputProvider(const std::string& type, const std::string& name, 
                          const IInterface* parent);
      
      virtual ~EnergyInputProvider();

      virtual StatusCode initialize();

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const; 

      virtual void handle(const Incident&);

   private:

      ServiceHandle<ITHistSvc> m_histSvc;

      SG::ReadHandleKey< LVL1::EnergyTopoData > m_energyLocation;    //!<  EnergyROI SG key

      mutable LockedHandle<TH1> m_hPt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hPhi ATLAS_THREAD_SAFE;

   };
}

#endif
