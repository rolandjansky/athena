/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_ELECTRON_MONITORING_TOOL_H
#define TRT_ELECTRON_MONITORING_TOOL_H

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "TrkParticleBase/LinkToTrackParticleBase.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"


#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string>
#include "TF1.h"
#include "TMath.h"
#include "TProfile.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"



enum myIsEMType { ISEMLOOSE , ISEMMEDIUM , ISEMTIGHT };
enum { PCAND_EL, PCAND_MU, PCAND_PI };
enum { DET_BARRELA, DET_BARRELC, DET_ENDCAPA, DET_ENDCAPC };

#define NGAMMABINS 50

class Identifier;

struct lw_geo_hists_t
{
	int N;

	TH1F_LW				*hNTracks;
	TH1F_LW				*hHTFracTrack;
	TProfile_LW			*pHTFracTrackPhi;
	TProfile_LW			*pHTFracStrawZR;
	TProfile_LW			*pHTFracStrawZAI;
	TProfile_LW			*pHTFracStrawZCO;

	TProfile_LW			*pHTFracGammaEl;
	TProfile_LW			*pHTFracGammaPosEl;
	TProfile_LW			*pHTFracGammaNegEl;

	TProfile_LW			*pHTFracGammaMu;
	TProfile_LW			*pHTFracGammaPosMu;
	TProfile_LW			*pHTFracGammaNegMu;

	TProfile_LW			*pHTFracGammaPi;
	TProfile_LW			*pHTFracGammaPosPi;
	TProfile_LW			*pHTFracGammaNegPi;

	TProfile_LW			*pHTFracGammaAll;
	TProfile_LW			*pHTFracGammaPosAll;
	TProfile_LW			*pHTFracGammaNegAll;
};

struct lw_partcand_hists_t
{
	int N;

	TH1F_LW				*hNCand;
	TH1F_LW				*hPIDProb;
	TH1F_LW				*hHTFrac;
	TProfile_LW			*pPIDProbEta;
	TProfile_LW			*pHTFracEta;
};

class TRT_Electron_Monitoring_Tool : public ManagedMonitorToolBase
{
public:

	TRT_Electron_Monitoring_Tool( const std::string & type,
				      const std::string & name,
				      const IInterface* parent );

	virtual ~TRT_Electron_Monitoring_Tool();

	virtual StatusCode bookHistograms();
	virtual StatusCode fillHistograms();
	virtual StatusCode procHistograms();

	virtual StatusCode initialize();
	virtual StatusCode finalize();

 protected:

	const TRT_ID*	m_pTRTHelper;

  	const InDetDD::TRT_DetectorManager *m_mgr;

	void bookGeoHistograms( lw_geo_hists_t &hists, const std::string& name);
	void bookPCandHistograms( MonGroup &monGroup, lw_partcand_hists_t &hists, const std::string& name);

	void loopOverConversions(std::vector<Trk::Track*> &v_usedTrks);
	//	const Trk::Track* getTrackFromTrackAtVertex(Trk::VxTrackAtVertex * VxTrkTag);
	bool conversionQualityCuts(const xAOD::TrackParticle* trkTag, const xAOD::TrackParticle* trkProbe);

	void loopOverRecElectrons(std::vector<Trk::Track*> &v_usedTrks);
	bool electronQualityCuts(const xAOD::Electron *electron);

	void loopOverMuons(std::vector<Trk::Track*> &v_usedTrks);
	bool muonQualityCuts(const xAOD::Muon *muon);

	void loopOverTracks(std::vector<Trk::Track*> &v_usedTrks);
	bool pionQualityCuts(xAOD::TrackParticle *trk, std::vector<Trk::Track*> &v_usedTrks);

	bool fillAllHistograms( xAOD::TrackParticle *trk, float mass=0, int PCand=0 );
	int myBarrelEC(int m_barrel_ec);
	bool isGasInCorOuter(int det, int phi, int layer);

	TH1F_LW*     trtBookHistoLW(MonGroup &mongroup, const std::string& hName, const std::string& hTitle, int bins, double lowbin, double highbin, double x_labelSize,double y_lableSize, const std::string& xTitle, const std::string& yTitle);
	TH2F_LW*     trtBookHistoLW(MonGroup &mongroup, const std::string& hName, const std::string& hTitle, int xbins, double lowxbins, double highxbins, int ybins, double lowybins, double highybins, double x_labelSize, double y_lableSize, const std::string& xTitle, const std::string& yTitle);
	TProfile_LW* trtBookHistoLW(MonGroup &mongroup, const std::string& hName, const std::string& hTitle, int bins, double lowbin, double highbin, double ymin, double ymax, double x_labelSize, double y_lableSize, const std::string& xTitle, const std::string& yTitle, double msize, int mcolor, int mstyle);

	void trtRegHist(LWHist* hist, MonGroup &mongrp, const char* hName);

private:

  	const DataVector<xAOD::TrackParticle> *m_trkpCollection;
	std::string m_tracksName;

  	const xAOD::ElectronContainer* m_electronContainer;
	std::string m_electronsName;

  	const xAOD::MuonContainer* m_muonContainer;
	std::string m_muonsName;

	const xAOD::VertexContainer* m_conversionContainer;
	std::string m_conversionName;

	std::string m_isEMFlag;
	myIsEMType m_isEMType;

	bool m_doTracksMon;
	bool m_doElectronMon;
	bool m_doMuonMon;

	bool m_doRecElectrons;
	bool m_doConversions;

	bool m_doShift;
	bool m_doExpert;
	bool m_isOnline;
	bool m_useTRTOnly;
	unsigned int m_NMinTRTHits;

	int m_nEtaBins;
	int m_nPhiBins;
	int m_nZRBins;
	float m_ZRMax;
	float m_PionTRTHitCut;

	lw_geo_hists_t m_tBarrelA;
	lw_geo_hists_t m_tBarrelC;
	lw_geo_hists_t m_tEndCapA;
	lw_geo_hists_t m_tEndCapC;

	lw_partcand_hists_t m_tEl;
	lw_partcand_hists_t m_tMu;
	lw_partcand_hists_t m_tPi;

};
#endif
