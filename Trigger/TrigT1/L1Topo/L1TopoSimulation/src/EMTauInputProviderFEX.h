/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EMTauInputProviderFEX
#define L1TopoSimulation_EMTauInputProviderFEX

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"

//EM/Tau EDMs
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"

class TH1I;
class TH2I;

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

      const DataHandle< xAOD::TrigEMClusterContainer > m_eFEXCluster; //!< cluster from eFEX
      StringProperty m_eFEXClusterLoc;

      TH1I * m_hEMEt {nullptr};
      TH2I * m_hEMEtaPhi {nullptr};
      TH1I * m_hTauEt {nullptr};
      TH2I * m_hTauEtaPhi {nullptr};

   };
}

#endif
