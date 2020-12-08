/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_JetInputProvider
#define L1TopoSimulation_JetInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/LockedHandle.h"
#include "TrigT1CaloEvent/JetCMXTopoDataCollection.h"

#include "TH1.h"
#include "TH2.h"

class ITHistSvc;

namespace LVL1 {

   class JetInputProvider : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      JetInputProvider(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~JetInputProvider();

      virtual StatusCode initialize();

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const; 

      virtual void handle(const Incident&);

   private:

      ServiceHandle<ITHistSvc> m_histSvc;

      SG::ReadHandleKey< DataVector<JetCMXTopoData> >  m_jetLocation;    //!<  Jet ROIs SG key

      mutable LockedHandle<TH1> m_hPt1 ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hPt2 ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hEtaPhi ATLAS_THREAD_SAFE;
   };
}

#endif
