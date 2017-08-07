/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EMTauInputProvider
#define L1TopoSimulation_EMTauInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"
class TH1I;
class TH2I;

class ITHistSvc;

namespace LVL1 {

   class EMTauInputProvider : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      EMTauInputProvider(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~EMTauInputProvider();

      virtual StatusCode initialize();

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const; 

      virtual void handle(const Incident&);

   private:

      /** \brief calculates eta and phi from roiWord*/
      void CalculateCoordinates(int32_t roiWord, double & eta, double & phi) const;

      ServiceHandle<ITHistSvc> m_histSvc;

      SG::ReadHandleKey<DataVector<LVL1::CPCMXTopoData>> m_emTauLocation;    //!<  EMTAU ROI SG key

      TH1I * m_hEMEt {nullptr};
      TH2I * m_hEMEtaPhi {nullptr};
      TH1I * m_hTauEt {nullptr};
      TH2I * m_hTauEtaPhi {nullptr};

   };
}

#endif
