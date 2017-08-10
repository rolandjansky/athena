/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_JetInputProvider
#define L1TopoSimulation_JetInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrigT1CaloEvent/JetCMXTopoDataCollection.h"

class TH1I;
class TH2I;

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

      TH1I * m_hPt1 {nullptr};
      TH1I * m_hPt2 {nullptr};
      TH2I * m_hEtaPhi {nullptr};
   };
}

#endif
