/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
//#include "InDetPerformanceMonitoring/IDPerfMonNtupleMaker.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

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
	virtual StatusCode procHistograms();

	void RegisterHisto(MonGroup& mon, TH1* histo);
	void RegisterHisto(MonGroup& mon, TProfile* histo);
	void RegisterHisto(MonGroup& mon, TGraph* graph);

protected:

	int m_histosBooked;

	TH1F* m_mass;
	TH1F* m_mass_scaled;
	TH2F* m_massVsPhi;
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

	TH1F* m_massVersusPt_merged;
	TH1F* m_widthVersusPt_merged;
	TH1F* m_massVersusRadius_merged;
	TH1F* m_widthVersusRadius_merged;
	TH1F* m_massVersusEta_merged;
	TH1F* m_widthVersusEta_merged;
	TH1F* m_massVersusPhi_merged;
	TH1F* m_widthVersusPhi_merged;
	TH1F* m_massVersusCurvatureDiff_merged;
	TH1F* m_widthVersusCurvatureDiff_merged;

	const static Int_t m_nFittedBinsPt = 5 ;
	const static Int_t m_nBinsPt = 46 ;
	const static Int_t m_nFittedBinsRadius = 7 ;
	const static Int_t m_nBinsRadius = 70 ;
	TH1F* m_massVPtBinHistos[m_nBinsPt];
	TH1F* m_massVEtaBinHistos[10];
	TH1F* m_massVPhiBinHistos[10];
	TH1F* m_massVCurvatureDiffBinHistos[6];
	TH1F* m_massVRadiusBinHistos[m_nBinsRadius];
	TH1F* m_massVPtBinFittedHistos[m_nFittedBinsPt];
	TH1F* m_massVRadiusBinFittedHistos[m_nFittedBinsRadius];

	TH1F* m_Nevents;

 private:

	//	StoreGateSvc* m_storeGate;
	std::string m_stream;
	std::string m_tracksName;
	std::string m_triggerChainName;
	int m_checkrate;
	std::string m_VxContainerName;
	std::string m_VxPrimContainerName;

	//IDPerfMonNtupleMaker NtupleMaker;
	//bool m_Ntuple;

};

#endif
