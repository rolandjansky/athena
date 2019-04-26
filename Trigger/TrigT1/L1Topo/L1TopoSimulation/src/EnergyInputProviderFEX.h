/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EnergyInputProviderFEX
#define L1TopoSimulation_EnergyInputProviderFEX

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "xAODTrigger/EnergySumRoI.h"

class TH1I;

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

      const DataHandle< xAOD::EnergySumRoI >    m_gFEXMET;        //!< MET from gFEX (will make it an array later)
      StringProperty m_gFEXMETLoc;    //!<  EnergyROI SG key

      TH1I * m_hPt {nullptr};
      TH1I * m_hPhi {nullptr};

   };
}

#endif
