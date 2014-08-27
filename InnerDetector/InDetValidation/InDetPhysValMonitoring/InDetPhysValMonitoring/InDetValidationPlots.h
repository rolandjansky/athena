/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETVALIDATIONPLOTS
#define INDETPHYSVALMONITORING_INDETVALIDATIONPLOTS


//std includes
#include <string>

//local includes
#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/IDHitPlots.h"
#include "xAODTracking/TrackParticle.h"  

///class holding plots for Inner Detector Validation and implementing fill methods
class InDetValidationPlots:public PlotBase {
public:
	InDetValidationPlots(PlotBase * pParent, const std::string & dirName);
	void fill(const xAOD::TrackParticle & trkParticle);
	
private:
	Trk::IDHitPlots m_idPlots;
	
	
};




#endif
