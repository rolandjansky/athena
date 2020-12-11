/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EMTauInputProvider
#define L1TopoSimulation_EMTauInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/LockedHandle.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"

#include "TH1.h"
#include "TH2.h"

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

      mutable LockedHandle<TH1> m_hEMEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEMEtaPhi ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hTauEt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hTauEtaPhi ATLAS_THREAD_SAFE;

   };
}

#endif
