/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_PT
#define INDETPHYSVALMONITORING_INDETPERFPLOT_PT
/**
 * @file InDetPerfPlot_Pt.h
 * @author shaun roe
**/


//std includes
#include <string>

//local includes
#include "TrkValHistUtils/PlotBase.h"
#include "xAODBase/IParticle.h"  

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_Pt:public PlotBase {
public:
	InDetPerfPlot_Pt(PlotBase * pParent, const std::string & dirName);
	void fill(const xAOD::IParticle& particle);
	
private:
	///Pt Histogram with full pt range
	TH1* m_recPt;
	///Pt histogram with restricted range
	TH1* m_recPtLow;
	
	//plot base has nop default implementation of this; we use it to book the histos
	void initializePlots();
	
	
};




#endif
