/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetVertexMonitoring_H
#define InDetVertexMonitoring_H

#include <string>
#include <map>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

class TH1F_LW;
class TF2F_LW;
 
class InDetVertexMonitoring : public ManagedMonitorToolBase  {

 public:
   InDetVertexMonitoring( const std::string & type, const std::string & name, const IInterface* parent );
   ~InDetVertexMonitoring();

   virtual StatusCode initialize();
   virtual StatusCode bookHistograms();
   virtual StatusCode fillHistograms();

 protected:
 
  TH1F_LW* m_h_vxdist;
  TH1F_LW* m_h_Ntracks;
  TH1F_LW* m_h_sumpt;
  TH1F_LW* m_h_pvChiSq;
  TH1F_LW* m_h_pvDof;
  TH1F_LW* m_h_pvChiSqovDof;
  TH1F_LW* m_h_pvChiSqProb;
  TH1F_LW* m_h_cumulVx;
  TH2F_LW* m_h_deltaZpairs;

  private:

   Gaudi::Property<bool> m_doControlPlots { this, "DoControlPlots", false, "Enable control plots" };
   
   Gaudi::Property<std::string> m_histFolder 
   { this, "histFolder", "InDetGlobal/PrimaryVertexMultiplicity", "ROOT histogram path" };

   SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo { this, "EvtInfo", "EventInfo", "EventInfo name" };
   
   SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer 
   { this, "VertexContainer", "PrimaryVertices", "Vertex container key" };

};

#endif // InDetVertexMonitoring_H

