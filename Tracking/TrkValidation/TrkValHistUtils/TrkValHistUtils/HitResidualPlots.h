/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#ifndef TRKVALHISTUTILS_HITRESIDUALPLOTS_H
#define TRKVALHISTUTILS_HITRESIDUALPLOTS_H

#include "PlotBase.h"
#include "TrkEventPrimitives/ResidualPull.h"


namespace Trk{

class HitResidualPlots: public PlotBase {
    public:
 HitResidualPlots(PlotBase *pParent, std::string sDir, std::string sType=""): PlotBase(pParent, sDir),m_sType(sType) { init(); }
      void fill( const Trk::ResidualPull& resPull );

      TH1* residuals;
      TH1* pulls;

    private:
      std::string m_sType;
      void init();
      void initializePlots();
};
 
}
#endif
#endif // not XAOD_ANALYSIS
