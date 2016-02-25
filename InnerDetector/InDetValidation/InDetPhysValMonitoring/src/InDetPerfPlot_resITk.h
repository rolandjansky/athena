/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_resITk
#define INDETPHYSVALMONITORING_InDetPerfPlot_resITk
/**
 * @file InDetPerfPlot_resITk.h
 * @author shaun roe
**/


//std includes
#include <string>
#include <vector>
#include "TProfile.h"

//local includes

#include "TrkValHistUtils/PlotBase.h"

//could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

#include "InDetPlotBase.h"


#include <map>

//fwd declaration
class IToolSvc;
class IExtrapolator;


///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_resITk:public PlotBase {
public:

  InDetPerfPlot_resITk(PlotBase * pParent, const std::string & dirName);

  void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);
//  virtual bool isDefined(TString t);
	virtual ~InDetPerfPlot_resITk(){/** nop **/}
	
	void getTrackParameters(const xAOD::TruthParticle& truthprt);
	void getTrackParameters(const xAOD::TrackParticle& truthprt);

private:
    bool m_primTrk;
		bool m_secdTrk;
		bool m_allTrk;
		
		void initializePlots();
	  void finalizePlots();

		std::map<TString, float> trkP;
		std::map<TString, float> truetrkP;
		std::map<TString, float> trkErrP;

		TH1* m_resITk_pull_d0;
		TH1* m_resITk_pull_z0;
		TH1* m_resITk_pull_qoverp;
		TH1* m_resITk_pull_qoverpt;
		TH1* m_resITk_pull_theta;
		TH1* m_resITk_pull_phi;
		TH1* m_resITk_pull_pt;

};


#endif
