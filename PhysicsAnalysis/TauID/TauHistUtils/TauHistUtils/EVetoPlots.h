/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_EVETOPLOTS_H
#define TAUHISTUTILS_EVETOPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODTau/TauJet.h"

namespace Tau{

class EVetoPlots: public PlotBase {
  public:
    EVetoPlots(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);
    virtual ~EVetoPlots();
    void fill(const xAOD::TauJet& tau);

    TH1* m_HadRadius;	 
    TH1* m_EMRadius;	 
    TH1* m_IsoFrac;	 
    TH1* m_CentFrac;	 
    TH1* m_id_BDTEleScore;
    TH1* m_pt_eleBDTloose;
    TH1* m_pt_eleBDTmed; 
    TH1* m_pt_eleBDTtight;
    TH1* m_pt_eleBDTlooseHighPt;
    TH1* m_pt_eleBDTmedHighPt; 
    TH1* m_pt_eleBDTtightHighPt;
    
  private:
    void initializePlots();
    std::string m_sTauJetContainerName;
};

}

#endif
