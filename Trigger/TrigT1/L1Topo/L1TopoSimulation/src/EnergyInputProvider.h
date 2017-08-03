/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EnergyInputProvider
#define L1TopoSimulation_EnergyInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrigT1CaloEvent/EnergyTopoData.h"

class TH1I;

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

      TH1I * m_hPt {nullptr};
      TH1I * m_hPhi {nullptr};

   };
}

#endif
