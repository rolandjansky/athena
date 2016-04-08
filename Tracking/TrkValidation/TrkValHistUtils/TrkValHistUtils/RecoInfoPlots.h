/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_RECOINFOPLOTS_H
#define TRKVALHISTUTILS_RECOINFOPLOTS_H

#include "PlotBase.h"
#include "xAODTracking/TrackParticle.h"

namespace Trk{

class RecoInfoPlots: public PlotBase {
  public:
 RecoInfoPlots(PlotBase *pParent, std::string sDir, std::string sType=""):PlotBase(pParent, sDir),m_sType(sType){ init();}
    void fill(const xAOD::TrackParticle& trkprt);
 		
    TH1* trackfitchi2;
    TH1* trackfitndof;
    TH1* trackcon;
    TH1* trackchi2prob;
    
  private:
    std::string m_sType;
    void init();
    void initializePlots();
			
};

}

#endif

