/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_FAKES
#define INDETPHYSVALMONITORING_INDETPERFPLOT_FAKES
/**
 * @file InDetPerfPlot_fakes.h
 * @author shaun roe
**/


//std includes
#include <string>

//local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h" 

///class holding fake plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_fakes:public InDetPlotBase {

  friend class InDetPerfPlot_ExtendedFakes; // @asogaard

public:
  enum Category {ALL, IN_JETS, N_FAKETYPES};
	InDetPerfPlot_fakes(InDetPlotBase * pParent, const std::string & dirName);
	void fill(const xAOD::TrackParticle& trkprt, const bool isFake, const Category f=ALL);
	
private:
	///fakes Histograms
	TH1*  m_fakepT  ;
  TH1*  m_fakePtLow ;
  TH1*  m_fakephi ;
  TH1*  m_fakeeta ;
  TH1*  m_faked0 ;
  TH1*  m_fakez0 ;
 	//fake rates as TProfiles
 	TProfile * m_track_fakerate_vs_pt;
 	TProfile * m_track_fakerate_vs_eta;
 	TProfile * m_track_fakerate_vs_phi;
 	TProfile * m_track_fakerate_vs_d0;
 	TProfile * m_track_fakerate_vs_z0;
 	
 	
 	
	
	//plot base has nop default implementation of this; we use it to book the histos
	void initializePlots();
	
	
};




#endif
