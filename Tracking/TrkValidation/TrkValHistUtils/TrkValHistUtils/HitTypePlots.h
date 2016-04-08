/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_HITTYPEPLOTS_H
#define TRKVALHISTUTILS_HITTYPEPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

namespace Trk{

class HitTypePlots:public PlotBase {
    public:
      HitTypePlots(PlotBase* pParent, std::string sHitType, std::string sHitLabel, int iMin, int iMax);
      void fill(int iHits, float fEta, float fPhi);
    
      std::string m_sHitType;
      std::string m_sHitLabel;
      int m_iMin;
      int m_iMax;

      TH1* Hits;
      TH2* HitsVsEta;   
      TH2* HitsVsPhi;   

    private:
      void initializePlots();
    
};
}

#endif
