/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONISOLATIONPLOTS_H
#define MUONHISTUTILS_MUONISOLATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"

namespace Muon{

class IsoPlots:public PlotBase {
    public:
      IsoPlots(PlotBase* pParent, std::string sDir, std::string sConeSize);
      void fill(const xAOD::Muon& muon, const xAOD::Iso::IsolationType &isoType);
      void fill(float fIso, float fPt);
      std::string m_sConeSize;

      TH1* cone;   
      TH1* conerel;

    private:
      void initializePlots();
    
};

class MuonIsolationPlots:public PlotBase {
   public:
     MuonIsolationPlots(PlotBase* pParent, std::string sDir);
     void fill(const xAOD::Muon& muon);

     IsoPlots m_oPtCone20;
     IsoPlots m_oPtCone30;
     IsoPlots m_oPtCone40;
     IsoPlots m_oEtCone20;
     IsoPlots m_oEtCone30;
     IsoPlots m_oEtCone40;
};

}

#endif
