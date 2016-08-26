/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDAlignMonPVBiases_H
#define IDAlignMonPVBiases_H

// **********************************************************************
// IDAlignMonPVBIases.cxx
// AUTHORS: Ambrosius  Vermeulen, Pierfrancesco Butti
// **********************************************************************

#include <vector>

#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TH3F.h"
#include "TH2F.h"
#include "TFile.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include <map>

namespace Trk  { 
  class ITrackToVertexIPEstimator;
  class VxCandidate;
  class Track;
  class VxTrackAtVertex;
}

namespace InDetAlignMon{
  class TrackSelectionTool;
}

class IDAlignMonPVBiases : public ManagedMonitorToolBase
{

public:

  IDAlignMonPVBiases( const std::string & type, const std::string & name, const IInterface* parent ); 

  virtual ~IDAlignMonPVBiases();
  virtual StatusCode initialize();
  virtual StatusCode fillHistograms();
  virtual StatusCode finalize();
  virtual StatusCode bookHistograms();
  virtual StatusCode procHistograms();

private:
  bool fillVertexInformation() const;
  const xAOD::Vertex* findAssociatedVertexTP(const xAOD::TrackParticle *) const;
  
  mutable std::map<const xAOD::TrackParticle*, const xAOD::Vertex* > m_trackVertexMapTP;

  int m_checkrate;
  int m_events; 
  std::string m_tracksName;
  std::string m_triggerChainName;
  std::string m_VxPrimContainerName;
  ToolHandle< Trk::ITrackToVertexIPEstimator >  m_trackToVertexIPEstimator;
  const xAOD::VertexContainer* m_vertices;
  std::string m_TreeFolder;
  TTree* m_Tree;
  std::string m_TreeName;

  mutable unsigned int            m_runNumber;
  mutable unsigned int            m_evtNumber;
  mutable unsigned int            m_lumi_block;

  double m_charge;
  double m_pt;
  double m_eta;
  double m_phi;
  double m_z0;
  double m_d0;
  double m_z0_err;
  double m_d0_err;
  double m_vertex_x;
  double m_vertex_y;
  double m_vertex_z;

  //Daiki stuf
  double bsXerror;
  double bsYerror;
  double bsXYerror;
  double bsZerror;
	
  ToolHandle< InDetAlignMon::TrackSelectionTool > m_trackSelection; 
};

#endif
