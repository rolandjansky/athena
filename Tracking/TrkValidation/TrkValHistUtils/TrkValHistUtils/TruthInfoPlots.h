/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_TRUTHINFOPLOTS_H
#define TRKVALHISTUTILS_TRUTHINFOPLOTS_H

#include "PlotBase.h"
#include "xAODTruth/TruthParticle.h"

namespace Trk{

class TruthInfoPlots: public PlotBase {
  public:
    TruthInfoPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir){ init();}
    void fill(const xAOD::TruthParticle& truthprt);
 		
    TH1* truthType;
    TH1* origin;

  private:
    void init();
    void initializePlots();
			
};

}

#endif

