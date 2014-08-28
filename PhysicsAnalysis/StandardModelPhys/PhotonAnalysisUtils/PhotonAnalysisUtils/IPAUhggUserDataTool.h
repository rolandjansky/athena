/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//

///////////////////////////////////////////////////////////////////////
// IPAUhggUserDataTool.h (c) Marcello FANTI 21/09/2007                  //
///////////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_IHGGUSERDATATOOL_H
#define PHOTONANALYSISUTILS_IHGGUSERDATATOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPAUhggUserDataTool("IPAUhggUserDataTool", 1, 0);
        
class IPAUhggUserDataTool : virtual public IAlgTool 
{
  
 public:

  enum { not_filled = -99999 } ;

  class UserData {
  public:

    UserData() {
      ud_Ph_eta = 0 ;
      ud_Ph_phi = 0 ;
      ud_Ph_pt = 0 ;
      ud_Ph_convFlag = 0 ;
      ud_Ph_conv_vtx_Rmin = 0 ;
      ud_Ph_conv_vtx_chi2 = 0 ;
      ud_Jet_eta = 0 ;
      ud_Jet_phi = 0 ;
      ud_Jet_pt = 0 ;
      ud_Jet_E = 0 ;
      ud_El_eta = 0 ;
      ud_El_phi = 0 ;
      ud_El_pt = 0 ;
      ud_El_q = 0 ;
      ud_Mu_eta = 0 ;
      ud_Mu_phi = 0 ;
      ud_Mu_pt = 0 ;
      ud_Mu_q = 0 ;
      ud_LeptonJets_pt = 0;
      Reset() ;
    } ;
    
    void Reset() {
      ud_run = not_filled ;
      ud_event = not_filled ;
      //
      if ( ud_Ph_eta )            ud_Ph_eta->clear() ;
      if ( ud_Ph_phi )            ud_Ph_phi->clear() ;
      if ( ud_Ph_pt )             ud_Ph_pt->clear() ;
      if ( ud_Ph_convFlag )       ud_Ph_convFlag->clear() ;
      if ( ud_Ph_conv_vtx_Rmin )  ud_Ph_conv_vtx_Rmin->clear() ;
      if ( ud_Ph_conv_vtx_chi2 )  ud_Ph_conv_vtx_chi2->clear() ;
      //
      ud_Njets = not_filled ;
      if ( ud_Jet_eta )  ud_Jet_eta->clear() ;
      if ( ud_Jet_phi )  ud_Jet_phi->clear() ;
      if ( ud_Jet_pt )   ud_Jet_pt->clear() ;
      if ( ud_Jet_E )    ud_Jet_E->clear() ;
      //
      ud_Nleptons = not_filled ;
      ud_Nelectrons = not_filled ;
      ud_Nmuons = not_filled ;
      ud_LeptonJetsSumpt = not_filled ;
      if ( ud_El_eta )  ud_El_eta->clear() ;
      if ( ud_El_phi )  ud_El_phi->clear() ;
      if ( ud_El_pt )   ud_El_pt->clear() ;
      if ( ud_El_q )    ud_El_q->clear() ;
      if ( ud_Mu_eta )  ud_Mu_eta->clear() ;
      if ( ud_Mu_phi )  ud_Mu_phi->clear() ;
      if ( ud_Mu_pt )   ud_Mu_pt->clear() ;
      if ( ud_Mu_q )    ud_Mu_q->clear() ;
      if ( ud_LeptonJets_pt ) ud_LeptonJets_pt->clear() ;
      //
      ud_mgg = not_filled ;
      ud_cosThetaStar = not_filled ;
      ud_cosThetaStarCS = not_filled ;
      ud_ptgg = not_filled ;
      ud_detagg = not_filled ;
      ud_dphigg = not_filled ;
      ud_mggj = not_filled ;
      ud_mjj = not_filled ;
      ud_okHggInclusive = not_filled ;
      ud_okHgg1j = not_filled ;
      ud_okHgg2j = not_filled ;
      ud_okHggZWjj = not_filled ;
      ud_okHggEtmiss = not_filled ;
      ud_okHggLepton = not_filled ;
      ud_L1_2em18 = not_filled ;
      ud_L2_2g20  = not_filled ;
      ud_EF_2g20  = not_filled ;
      //ud_L1_em60 = not_filled ;
      //ud_L2_g60 = not_filled ;
      //ud_EF_g60 = not_filled ;
      ud_PAU_truth = not_filled ;
      ud_Hmass_truth = not_filled ;
      ud_brem_truth = not_filled ;
      ud_mcweight = not_filled;
    };

    //
    int   ud_run;             // run number
    int   ud_event;           // event number
    // the two main photons:
    std::vector<float> *ud_Ph_eta;
    std::vector<float> *ud_Ph_phi;
    std::vector<float> *ud_Ph_pt;
    std::vector<int>   *ud_Ph_convFlag;
    std::vector<float> *ud_Ph_conv_vtx_Rmin;
    std::vector<float> *ud_Ph_conv_vtx_chi2;
    // the jets:
    int ud_Njets;
    std::vector<float> *ud_Jet_eta;
    std::vector<float> *ud_Jet_phi;
    std::vector<float> *ud_Jet_pt;
    std::vector<float> *ud_Jet_E;
    // the leptons:
    int ud_Nleptons;
    int ud_Nelectrons;
    std::vector<float> *ud_El_eta;
    std::vector<float> *ud_El_phi;
    std::vector<float> *ud_El_pt;
    std::vector<int>   *ud_El_q;
    int ud_Nmuons;
    std::vector<float> *ud_Mu_eta;
    std::vector<float> *ud_Mu_phi;
    std::vector<float> *ud_Mu_pt;
    std::vector<int>   *ud_Mu_q;
    //////// jets removing photons and medium electron
    float ud_LeptonJetsSumpt;
    std::vector<float> *ud_LeptonJets_pt;
    // ETmiss:
    float ud_Etmiss;
    float ud_Etmiss_phi;
    //
    float ud_mgg;             // gamma-gamma invariant mass
    float ud_cosThetaStar;    // angle between Higgs and leading photon (Higgs rest frame)
    float ud_cosThetaStarCS;  // angle between Higgs and leading photon (Collins-Soper frame)
    float ud_ptgg;            // pt of gamma-gamma system
    float ud_detagg;          // delta eta between leading and subleading photons
    float ud_dphigg;          // delta phi between leading and subleading photons
    float ud_mggj;            // gamma-gamma-jet invariant mass (for gg+1jet events)
    float ud_mjj;             // jet-jet invariant mass (for gg+2jet events)
    int   ud_okHggInclusive;  // ==1 if event selected by inclusive analysis
    int   ud_okHgg1j;         // ==1 if event selected by gg+1jet analysis
    int   ud_okHgg2j;         // ==1 if event selected by gg+2jet analysis
    int   ud_okHggZWjj;       // ==1 if event selected by gg+ZW->jj analysis
    int   ud_okHggEtmiss;     // ==1 if event selected by gg+etmiss analysis
    int   ud_okHggLepton;     // ==1 if event selected by gg+lepton analysis
    int   ud_L1_2em18;        // L1 trigger decision 2em18
    int   ud_L2_2g20;         // L2 trigger decision 2g20
    int   ud_EF_2g20;         // EF trigger decision 2g20
    //int   ud_L1_em60;         // L1 trigger decision em60
    //int   ud_L2_g60;          // L2 trigger decision g60
    //int   ud_EF_g60;          // EF trigger decision g60
    int   ud_PAU_truth;       // ==1 if event is H->gg process, otherwise ==0
    float ud_Hmass_truth;     // true Higgs mass
    int   ud_brem_truth;      // ==1 if at least 1 photon is from brem
    float ud_mcweight;
  } ;
  
 public:
  /** Virtual destructor */
  virtual ~IPAUhggUserDataTool(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() 
    { 
      return IID_IPAUhggUserDataTool; 
    };

  
  virtual IPAUhggUserDataTool::UserData & accessUserData() =0;
  virtual void saveUserData() =0;

  virtual bool DumpingNtuple() =0;
};


#endif // PHOTONANALYSISUTILS_IHGGUSERDATATOOL_H
