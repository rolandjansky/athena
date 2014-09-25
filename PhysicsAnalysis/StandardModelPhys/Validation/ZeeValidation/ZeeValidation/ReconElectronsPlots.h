/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEVALIDATION_RECONELECTRONSPLOTS_H
#define ZEEVALIDATION_RECONELECTRONSPLOTS_H
	
#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

namespace ZeeValidation{
  
  class ReconElectronsPlots:public PlotBase {
  public:
    ReconElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);

    void fill(const xAOD::Electron* electron, int level);
    void fillinAcc(const xAOD::Electron* electron, int level);
    void fill(const xAOD::EventInfo* eventInfo, const xAOD::VertexContainer* vertices);
    
    void finalize();
    
    //nLevels = 5 (Loose, Medium, Tight, OQ, Reco)
    static const int nLevels = 5;
    static const std::string cLevelLabel[nLevels];

    // Reco only information
    std::string m_sParticleType;
    
    TH1* electron_n;
    TH1* photon_n;
    TH1* electron_author;
    TH1* nvtx;
    TH1* mu;
    TH1* bcid;
    TH1* track_n;
    TH1* pv_x;
    TH1* pv_y;
    TH1* pv_z;

    TH1* electron_pt[nLevels];
    TH1* electron_eta[nLevels];
    TH1* electron_phi[nLevels];
    TH2* electron_etavsphi[nLevels];

    TH1* electron_eff_pt[nLevels-1];
    TH1* electron_eff_eta[nLevels-1];
    TH1* electron_eff_phi[nLevels-1];
    
  private:
        
    virtual void initializePlots();
    
  };
}

#endif
