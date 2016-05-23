/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_pull_h
#define INDETPHYSVALMONITORING_InDetPerfPlot_pull_h
/**
 * @file InDetPerfPlot_pull.h
 * @author shaun roe
**/

#include "InDetPlotBase.h"
//could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"


///class holding pull plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_pull:public InDetPlotBase {
public:
	enum Param{D0, Z0, PHI, THETA, QOVERP, NPARAMS};
	enum Projections{MEAN, RES, NPROJECTIONS};
	InDetPerfPlot_pull(InDetPlotBase * pParent, const std::string & dirName);
	///The fill method fills the main TH1 histograms which are later projected onto profiles to obtain the res plots
	void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);
	virtual ~InDetPerfPlot_pull(){/**nop**/}
	
private:
	std::vector<TH1*> m_pullPlots;
	std::vector<TH1*> m_pullPlotsWide;
	std::vector<std::string> m_paramNames;

	virtual void initializePlots();
	virtual void finalizePlots();	
};











#endif
