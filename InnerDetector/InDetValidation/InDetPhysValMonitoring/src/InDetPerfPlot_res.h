/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_res
#define INDETPHYSVALMONITORING_InDetPerfPlot_res
/**
 * @file InDetPerfPlot_res.h
 * @author shaun roe
**/


//std includes
#include <string>
#include <vector>


//local includes

#include "TrkValHistUtils/PlotBase.h"

//could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

//fwd declaration
class IToolSvc;
class IExtrapolator;


///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_res:public PlotBase {
public:
	enum Param{D0, Z0, PHI, THETA, Z0SIN_THETA, QOVER_PT, NPARAMS};
	enum Projections{MEAN, RES, NPROJECTIONS};
	InDetPerfPlot_res(PlotBase * pParent, const std::string & dirName);
	///The fill method fills the main TH1 histograms which are later projected onto profiles to obtain the res plots
	void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);
	virtual  ~InDetPerfPlot_res(){/** nop **/}
	
private:
	unsigned int m_trackEtaBins;
	std::vector<std::vector<TH1*> >m_resPlots;
	std::vector<std::pair<TH1*, TH1*> > m_projectedPlots;
	std::vector<std::string> m_paramNames;
	std::vector<std::string> m_projectionNames;
	
	virtual void initializePlots();
	virtual void finalizePlots();
	std::string formName(const unsigned int p, const unsigned int eta) const;
	std::string formProjectionName(const unsigned int projection, const unsigned int parameter) const;
	std::string formProjectionTitle(const unsigned int projection, const unsigned int parameter) const;
	
};


#endif
