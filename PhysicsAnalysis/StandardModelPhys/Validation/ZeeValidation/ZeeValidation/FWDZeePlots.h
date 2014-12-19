/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEVALIDATION_FWDZEEPLOTS_H
#define ZEEVALIDATION_FWDZEEPLOTS_H
	
#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBase/IParticle.h"

namespace ZeeValidation{
  
  class FWDZeePlots:public PlotBase {
  public:
    FWDZeePlots(PlotBase* pParent, std::string sDir, std::string sParticleType);  
    void fillZPlots(TLorentzVector& , int level );
    void fillElPlots(TLorentzVector& eleccen, TLorentzVector& elecfwd, int level  );

   //nLevels = 4 ( Loose, Tight, OQ, Reco )
    static const int nLevels = 4;
    static const std::string cLevelLabel[nLevels];

    // Reco only information
    std::string m_sParticleType;
    
    TH1* h_z_mass[nLevels];
    TH1* h_z_pt[nLevels];
    TH1* h_z_y[nLevels];

    TH1* h_dr_electrons[nLevels];

    TH1* h_pt_central_electron[nLevels];
    TH1* h_eta_central_electron[nLevels];
    TH1* h_phi_central_electron[nLevels];
    TH1* h_pt_fwd_electron[nLevels];
    TH1* h_eta_fwd_electron[nLevels];
    TH1* h_phi_fwd_electron[nLevels];

  private:
 
    virtual void initializePlots();
    
  };
}

#endif
