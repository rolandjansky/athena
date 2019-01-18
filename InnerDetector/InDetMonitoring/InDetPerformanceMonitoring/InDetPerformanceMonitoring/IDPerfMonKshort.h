/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IDPerfMonKshort_H
#define IDPerfMonKshort_H

// **********************************************************************
// IDPerfMonKshort.cxx
// AUTHORS: Beate Heinemann, Tobias Golling
// **********************************************************************
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "ITrackToVertex/ITrackToVertex.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

class TH1;
class TProfile;
class TGraph;


class IDPerfMonKshort : public ManagedMonitorToolBase
{

public:

	IDPerfMonKshort( const std::string & type, const std::string & name, const IInterface* parent );

	virtual ~IDPerfMonKshort();

	virtual StatusCode initialize();
	virtual StatusCode bookHistograms();
	virtual StatusCode fillHistograms();

	void RegisterHisto(MonGroup& mon, TH1* histo);
	void RegisterHisto(MonGroup& mon, TProfile* histo);
	void RegisterHisto(MonGroup& mon, TGraph* graph);
	void RegisterTree(MonGroup& mon, TTree* tree);

protected:

	int m_histosBooked;

	TH1F* m_mass;
	TH1F* m_radius;
	TH1F* m_radius_secVertices;
	TH1F* m_radius_secVertices_sel;
	TH2F* m_radiusVsZ_secVertex;
	TH2F* m_radiusVsZ_secVertex_sel;
	TH2F* m_YVsX_secVertex;
	TH2F* m_YVsX_secVertex_sel;
	TH2F* m_radiusVsZ_secVertex_Ks;
	TH2F* m_YVsX_secVertex_Ks;

	TH2F* m_YVsX_primVertex;
	TH2F* m_XVsZ_primVertex;
	TH2F* m_YVsZ_primVertex;
	TH2F* m_YVsX_primVertex_Ks;
	TH2F* m_XVsZ_primVertex_Ks;
	TH2F* m_YVsZ_primVertex_Ks;
	TH1F* m_pt;
	TH1F* m_eta;
	TH1F* m_phi;
	TH1F* m_massVersusPt;
	TH1F* m_widthVersusPt;
	TH1F* m_massVersusRadius;
	TH1F* m_widthVersusRadius;
	TH1F* m_massVersusEta;
	TH1F* m_widthVersusEta;
	TH1F* m_massVersusPhi;
	TH1F* m_widthVersusPhi;
	TH1F* m_massVersusCurvatureDiff;
	TH1F* m_widthVersusCurvatureDiff;

	TH1F* m_Nevents;
	TH1F* m_Nevents_sel;

 private:

	//	StoreGateSvc* m_storeGate;
	std::string m_stream;
	std::string m_tracksName;
	std::string m_triggerChainName;
	int m_checkrate;
	std::string m_VxContainerName;
	std::string m_VxPrimContainerName;

  	std::string m_defaultTreeName;       //Default ID Tracks
	std::string m_ValidationTreeDescription;
  
  	TTree* m_defaultTree;

  	ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

  	mutable unsigned int m_runNumber;
  	mutable unsigned int m_evtNumber;
  	mutable unsigned int m_lumi_block;

  	double m_Ks_x;
  	double m_Ks_y;
  	double m_Ks_z;

  	double m_pv_x;
  	double m_pv_y;
  	double m_pv_z;

  	double m_positive_px;
  	double m_positive_py;
  	double m_positive_pz;
  	double m_positive_z0;
  	double m_positive_d0;
  	double m_positive_z0_err;
  	double m_positive_d0_err;


  	double m_negative_px;
  	double m_negative_py;
  	double m_negative_pz;
  	double m_negative_z0;
  	double m_negative_d0;
  	double m_negative_z0_err;
  	double m_negative_d0_err;

};

#endif
