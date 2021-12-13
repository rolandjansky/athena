/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUDQA_EVETOPLOTS_H
#define TAUDQA_EVETOPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODTau/TauJet.h"

namespace Tau{

class EVetoPlots: public PlotBase {
  public:
    EVetoPlots(PlotBase *pParent, const std::string& sDir, std::string sTauJetContainerName);
    virtual ~EVetoPlots();
    void fill(const xAOD::TauJet& tau, float weight);

    TH1* m_HadRadius;	 
    TH1* m_EMRadius;	 
    TH1* m_IsoFrac;	 
    TH1* m_CentFrac;	 
    TH1* m_id_RNNEleScore;
    TH1* m_pt_eleRNNloose;
    TH1* m_pt_eleRNNmed; 
    TH1* m_pt_eleRNNtight;
    TH1* m_pt_eleRNNlooseHighPt;
    TH1* m_pt_eleRNNmedHighPt; 
    TH1* m_pt_eleRNNtightHighPt;
    
  private:
    void initializePlots();
    std::string m_sTauJetContainerName;
};

}

#endif
