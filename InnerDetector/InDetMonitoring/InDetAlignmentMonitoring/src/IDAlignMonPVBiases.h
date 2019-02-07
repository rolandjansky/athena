/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

  void InitializeHistograms();

  void RegisterHisto(MonGroup& mon, TH1* histo);
  void RegisterHisto(MonGroup& mon, TH2* histo);
  void RegisterHisto(MonGroup& mon, TProfile* histo);
  void RegisterHisto(MonGroup& mon, TProfile2D* histo);

 protected:

	/////////////////////////////////////////////////
    	///////Declare histo's 400MeV until 600MeV///////
    	/////////////////////////////////////////////////
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive;
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative;

	TH2F* m_trkd0_wrtPV_vs_phi_400MeV_600MeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_phi_400MeV_600MeV_negative;

	TH2F* m_trkd0_wrtPV_vs_eta_400MeV_600MeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_eta_400MeV_600MeV_negative;
 
	/////////////////////////////////////////////////
    	////////Declare histo's 600MeV until 1GeV////////
    	/////////////////////////////////////////////////
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive;
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative;

	TH2F* m_trkd0_wrtPV_vs_phi_600MeV_1GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_phi_600MeV_1GeV_negative;

	TH2F* m_trkd0_wrtPV_vs_eta_600MeV_1GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_eta_600MeV_1GeV_negative;
  
	/////////////////////////////////////////////////
    	/////////Declare histo's 1GeV until 2GeV/////////
    	/////////////////////////////////////////////////
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive;
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative;

	TH2F* m_trkd0_wrtPV_vs_phi_1GeV_2GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_phi_1GeV_2GeV_negative;

	TH2F* m_trkd0_wrtPV_vs_eta_1GeV_2GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_eta_1GeV_2GeV_negative;
  
	/////////////////////////////////////////////////
    	/////////Declare histo's 2GeV until 5GeV/////////
    	/////////////////////////////////////////////////
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive;
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative;

	TH2F* m_trkd0_wrtPV_vs_phi_2GeV_5GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_phi_2GeV_5GeV_negative;

	TH2F* m_trkd0_wrtPV_vs_eta_2GeV_5GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_eta_2GeV_5GeV_negative;
  
	/////////////////////////////////////////////////
    	////////Declare histo's 5GeV until 10GeV/////////
    	/////////////////////////////////////////////////
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive;
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative;

	TH2F* m_trkd0_wrtPV_vs_phi_5GeV_10GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_phi_5GeV_10GeV_negative;
  
	TH2F* m_trkd0_wrtPV_vs_eta_5GeV_10GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_eta_5GeV_10GeV_negative;
  
	/////////////////////////////////////////////////
    	/////////Declare histo's larger than 10GeV///////
    	/////////////////////////////////////////////////
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_positive;
	TH3F* m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_negative;

	TH2F* m_trkd0_wrtPV_vs_phi_10GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_phi_10GeV_negative;

	TH2F* m_trkd0_wrtPV_vs_eta_10GeV_positive;
  	TH2F* m_trkd0_wrtPV_vs_eta_10GeV_negative;

private:

  int m_checkrate;
  int m_events; 
  int m_histosBooked;
  std::string m_tracksName;
  std::string m_triggerChainName;
  std::string m_VxPrimContainerName;
  ToolHandle< Trk::ITrackToVertexIPEstimator >  m_trackToVertexIPEstimator;
  const xAOD::VertexContainer* m_vertices;
  bool m_do3dplots;  
  bool m_dolowpt;

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
	
  ToolHandle< InDetAlignMon::TrackSelectionTool > m_trackSelection; 
};

#endif
