/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_pull_h
#define INDETPHYSVALMONITORING_InDetPerfPlot_pull_h
/**
 * @file InDetPerfPlot_pull.h
 * @author shaun roe
**/

#include "TrkValHistUtils/PlotBase.h"
//could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"


///class holding pull plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_pull:public PlotBase {
public:
	enum Param{D0, Z0, PHI, THETA, QOVER_PT, NPARAMS};
	enum Projections{MEAN, RES, NPROJECTIONS};
	InDetPerfPlot_pull(PlotBase * pParent, const std::string & dirName);
	///The fill method fills the main TH1 histograms which are later projected onto profiles to obtain the res plots
	void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);
	virtual ~InDetPerfPlot_pull(){/**nop**/}
	
private:
	std::vector<TH1*> m_pullPlots;
	std::vector<TH1*> m_pullPlotsWide;
	std::vector<std::string> m_paramNames;
	
	virtual void initializePlots();
	virtual void finalizePlots();
	std::string formName(const unsigned int p) const;
	std::string formTitle(const unsigned int parameter) const;
	
};











#endif