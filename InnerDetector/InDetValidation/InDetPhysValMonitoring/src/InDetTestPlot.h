/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetTestPlot_PT
#define INDETPHYSVALMONITORING_InDetTestPlot
/**
 * @file InDetTestPlot.h
 * @author shaun roe
**/


//std includes
#include <string>

//local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetTestPlot:public InDetPlotBase {
public:
	InDetTestPlot(InDetPlotBase * pParent, const std::string & dirName);
	void fill(const xAOD::TrackParticle& particle);
	
private:
	TH1* m_test;
	
	//plot base has nop default implementation of this; we use it to book the histos
	void initializePlots();
	
	
};




#endif
