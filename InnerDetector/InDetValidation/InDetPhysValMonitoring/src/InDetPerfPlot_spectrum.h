/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_SPECTRUM
#define INDETPHYSVALMONITORING_INDETPERFPLOT_SPECTRUM
/**
 * @file InDetPerfPlot_spectrum.h
 * @author Stewart Swift
**/


//std includes
#include <string>

//local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h" 
#include "xAODTruth/TruthParticle.h"


class InDetPerfPlot_spectrum:public InDetPlotBase {
public:
  enum Category {ALL, IN_JETS, N_FAKETYPES};
	InDetPerfPlot_spectrum(InDetPlotBase * pParent, const std::string & dirName);
	void fillSpectrum(const xAOD::TrackParticle& trkprt,Float_t prob);
	void fillSpectrum(const xAOD::TruthParticle& particle);

private:

	TH1* m_nSCTHits;
	TH1*  m_nPixHits ;
	TH1*  m_nTotHits ;
	TH1*  m_nSCTDeadSensors ;
	TH1*  m_nPixDeadSensors ;
	TH1*  m_nTotDeadSensors ;

	TH1* m_recoMatchProb;
	TH1* m_recoPt;
	TH1* m_recoEta;
	TH1* m_recoPhi;

	TH1* m_truthPt;
	TH1* m_truthEta;
	TH1* m_truthPhi;


 	TProfile * m_nSCTHits_vs_eta;
 	TProfile * m_nPixHits_vs_eta;
 	TProfile * m_nTotHits_vs_eta;


 	TProfile * m_nSCTDeadSensors_vs_eta;
 	TProfile * m_nPixDeadSensors_vs_eta;
 	TProfile * m_nTotDeadSensors_vs_eta;

	
	//plot base has nop default implementation of this; we use it to book the histos
	void initializePlots();
	
	
};




#endif
