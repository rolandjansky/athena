/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define PhotonIDTool_cxx

#include "egammaAnalysisUtils/PhotonIDTool.h"

namespace EgammaPID {
  enum BitDefPhoton {
    ClusterEtaRange_Photon             =  0,
    ClusterEtaRange_PhotonLoose        =  1,
    // selection for Loose photons
    ClusterHadronicLeakage_PhotonLoose =  2,
    ClusterMiddleEnergy_PhotonLoose    =  3,
    ClusterMiddleEratio37_PhotonLoose  =  4,
    ClusterMiddleEratio33_PhotonLoose  =  5,
    ClusterMiddleWidth_PhotonLoose     =  6,
    // selection for tight photons
    ClusterHadronicLeakage_Photon = 10,
    ClusterMiddleEnergy_Photon    = 11,
    ClusterMiddleEratio37_Photon  = 12,
    ClusterMiddleEratio33_Photon  = 13,
    ClusterMiddleWidth_Photon     = 14,
    ClusterStripsEratio_Photon    = 15,
    ClusterStripsDeltaEmax2_Photon= 16,
    ClusterStripsDeltaE_Photon    = 17,
    ClusterStripsWtot_Photon      = 18,
    ClusterStripsFracm_Photon     = 19,
    ClusterStripsWeta1c_Photon    = 20,
    ClusterStripsDEmaxs1_Photon   = 21,
    TrackMatchEoverP_Photon       = 22,
    Isolation_Photon              = 29,
    ClusterIsolation_Photon       = 30,
    TrackIsolation_Photon         = 31
  };
}

unsigned int PhotonIDTool::isEM(int tuneloose, int tunetight) 
{
  // my bits for single cuts (12 bit -> 2^12-1 = 4095)
  //unsigned BitEta2   =    1;
  unsigned BitRhad   =    2;
  unsigned BitReta   =    4;
  unsigned BitRphi   =    8;
  unsigned BitWeta2  =   16;
  unsigned BitF1     =   32;
  //unsigned BitEmax2r =   64;
  unsigned BitDeltaE =  128;
  unsigned BitWtot   =  256;
  unsigned BitFside  =  512;
  unsigned BitW1     = 1024;
  unsigned BitEratio = 2048;

  int CutEnabButFailLoose = 0; 
  int CutEnabButFailTight = 0; 
  
  this->PhotonCutsLoose(tuneloose,CutEnabButFailLoose);
  this->PhotonCutsTight(tunetight,CutEnabButFailTight);

  int iflag = 0;

  if (m_e277>=0.1) { // valid for both LOOSE and TIGHT

    if (m_eta2>2.47) {
      iflag |= ( 0x1 << EgammaPID::ClusterEtaRange_PhotonLoose);
      iflag |= ( 0x1 << EgammaPID::ClusterEtaRange_Photon);
    } else {

      if ( CutEnabButFailLoose & BitRhad  ) // fail LOOSE Rhad cut
        iflag |= ( 0x1 << EgammaPID::ClusterHadronicLeakage_PhotonLoose);

      if ( CutEnabButFailLoose & BitReta  ) // fail LOOSE Reta cut
        iflag |= ( 0x1 << EgammaPID::ClusterMiddleEratio37_PhotonLoose);

      if ( CutEnabButFailLoose & BitWeta2 ) // fail LOOSE Weta2 cut
        iflag |= ( 0x1 << EgammaPID::ClusterMiddleWidth_PhotonLoose);

      if ( CutEnabButFailTight & BitRhad  ) // fail TIGHT Rhad cut
        iflag |= ( 0x1 << EgammaPID::ClusterHadronicLeakage_Photon);

      if ( CutEnabButFailTight & BitReta  ) // fail TIGHT Reta cut
        iflag |= ( 0x1 << EgammaPID::ClusterMiddleEratio37_Photon);

      if ( CutEnabButFailTight & BitRphi  ) // fail TIGHT Rphi cut
        iflag |= ( 0x1 << EgammaPID::ClusterMiddleEratio33_Photon);

      if ( CutEnabButFailTight & BitWeta2 ) // fail TIGHT Weta2 cut
        iflag |= ( 0x1 << EgammaPID::ClusterMiddleWidth_Photon);
    }
  } else {
    iflag |= ( 0x1 << EgammaPID::ClusterMiddleEnergy_PhotonLoose);
    iflag |= ( 0x1 << EgammaPID::ClusterMiddleEnergy_Photon);
  }

  if ( CutEnabButFailTight & BitF1 ) // fail TIGHT F1 cut
    iflag |= ( 0x1 << EgammaPID::ClusterStripsEratio_Photon); // f1

  if ( CutEnabButFailTight & BitDeltaE ) // fail TIGHT DeltaE cut
    iflag |= ( 0x1 << EgammaPID::ClusterStripsDeltaE_Photon); // DeltaE

  if ( CutEnabButFailTight & BitEratio ) // fail TIGHT Eratio cut
    iflag |= ( 0x1 << EgammaPID::ClusterStripsDEmaxs1_Photon);   // Eratio

  if ( CutEnabButFailTight & BitWtot ) // fail TIGHT Wtot cut
    iflag |= ( 0x1 << EgammaPID::ClusterStripsWtot_Photon); // Wtot

  if ( CutEnabButFailTight & BitFside ) // fail TIGHT Fside cut
    iflag |= ( 0x1 << EgammaPID::ClusterStripsFracm_Photon); // Fside

  if ( CutEnabButFailTight & BitW1 ) // fail TIGHT W1 cut
    iflag |= ( 0x1 << EgammaPID::ClusterStripsWeta1c_Photon); // W1

  return iflag;
}


bool PhotonIDTool::PhotonCutsLooseElectron(int& CutEnabButFail)
{
  this->LoadLooseCuts(1);
  return PhotonCutsParam(false,  // bool UseCutEta2      ,
			 true,   // bool UseCutHad       ,
			 true,   // bool UseCutS2ratio1  ,
			 false,  // bool UseCutS2ratio2  ,
			 true,   // bool UseCutS2weta2   ,
			 false,  // bool UseCutS1f1      , // set to FALSE, it's now applied at the reconstruction level
			 false,  // bool UseCutS1emax2r  ,
			 false,  // bool UseCutS1deltae  ,
			 false,  // bool UseCutS1wtot    ,
			 false,  // bool UseCutS1fracm   ,
			 false,  // bool UseCutS1w1      ,
			 false,  // bool UseCutS1eratio
			 10000. ,
			 -999.,
			 CutEnabButFail);
}


bool PhotonIDTool::PhotonCutsFixPtEta(float FixPt, float FixEta)
{
  return PhotonCutsParam(true,true,true,true,true,true,true,true,true,true,true,false,
			 FixPt, FixEta);
}


bool PhotonIDTool::PhotonCutsLoose(int tune, int& CutEnabButFail) 
{
  switch (tune) {
    
  case 1:
    this->LoadLooseCuts(1);
    return PhotonCutsParam(false,  // bool UseCutEta2      ,
			   true,   // bool UseCutHad       ,
			   true,   // bool UseCutS2ratio1  ,
			   false,  // bool UseCutS2ratio2  ,
			   true,   // bool UseCutS2weta2   ,
			   false,  // bool UseCutS1f1      , // set to FALSE, it's now applied at the reconstruction level
			   false,  // bool UseCutS1emax2r  ,
			   false,  // bool UseCutS1deltae  ,
			   false,  // bool UseCutS1wtot    ,
			   false,  // bool UseCutS1fracm   ,
			   false,  // bool UseCutS1w1      ,
			   false,  // bool UseCutS1eratio
			   10000. ,
			   -999.,
			   CutEnabButFail);
    break;

  case 2:
    this->LoadLooseCuts(2);
    return PhotonCutsParam(false,  // bool UseCutEta2      ,
			   true,   // bool UseCutHad       ,
			   true,   // bool UseCutS2ratio1  ,
			   false,  // bool UseCutS2ratio2  ,
			   true,   // bool UseCutS2weta2   ,
			   false,  // bool UseCutS1f1      , // set to FALSE, it's now applied at the reconstruction level
			   false,  // bool UseCutS1emax2r  ,
			   false,  // bool UseCutS1deltae  ,
			   false,  // bool UseCutS1wtot    ,
			   false,  // bool UseCutS1fracm   ,
			   false,  // bool UseCutS1w1      ,
			   false,  // bool UseCutS1eratio
			   10000. ,
			   -999.,
			   CutEnabButFail);
    break;

  case 3:
    this->LoadLooseCuts(3);
    return PhotonCutsParam(false,  // bool UseCutEta2      ,
			   true,   // bool UseCutHad       ,
			   true,   // bool UseCutS2ratio1  ,
			   false,  // bool UseCutS2ratio2  ,
			   true,   // bool UseCutS2weta2   ,
			   false,  // bool UseCutS1f1      , // set to FALSE, it's now applied at the reconstruction level
			   false,  // bool UseCutS1emax2r  ,
			   false,  // bool UseCutS1deltae  ,
			   false,  // bool UseCutS1wtot    ,
			   false,  // bool UseCutS1fracm   ,
			   false,  // bool UseCutS1w1      ,
			   false,  // bool UseCutS1eratio
			   10000. ,
			   -999.,
			   CutEnabButFail);
    break;

  case 4:
    this->LoadLooseCuts(4);
    return PhotonCutsParam(false,  // bool UseCutEta2      ,
			   true,   // bool UseCutHad       ,
			   true,   // bool UseCutS2ratio1  ,
			   false,  // bool UseCutS2ratio2  ,
			   true,   // bool UseCutS2weta2   ,
			   false,  // bool UseCutS1f1      , // set to FALSE, it's now applied at the reconstruction level
			   false,  // bool UseCutS1emax2r  ,
			   false,  // bool UseCutS1deltae  ,
			   false,  // bool UseCutS1wtot    ,
			   false,  // bool UseCutS1fracm   ,
			   false,  // bool UseCutS1w1      ,
			   false,  // bool UseCutS1eratio
			   10000. ,
			   -999.,
			   CutEnabButFail);
    break;

  case 91: // old loose1 (itune2 cuts + fix pt=0)
    this->LoadDefaultCuts();
    return PhotonCutsFixPtEta(10000.); // 10 GeV thresholds
    break;

  case 92: // old loose2 (itune2 cuts + drop strips selections)
    this->LoadDefaultCuts();
    return PhotonCutsParam(true,  // bool UseCutEta2      ,
			   true,  // bool UseCutHad       ,
			   true,  // bool UseCutS2ratio1  ,
			   true,  // bool UseCutS2ratio2  ,
			   true,  // bool UseCutS2weta2   ,
			   true,  // bool UseCutS1f1      ,
			   false, // bool UseCutS1emax2r  ,
			   false, // bool UseCutS1deltae  ,
			   false, // bool UseCutS1wtot    ,
			   false, // bool UseCutS1fracm   ,
			   false, // bool UseCutS1w1      ,
			   false  // bool UseCutS1eratio
			   );
    break;

  case 93: // old loose3 (itune2 cuts + fix pt=0 && drop strips selections)
    this->LoadDefaultCuts();
    return PhotonCutsParam(true,  // bool UseCutEta2      ,
			   true,  // bool UseCutHad       ,
			   true,  // bool UseCutS2ratio1  ,
			   true,  // bool UseCutS2ratio2  ,
			   true,  // bool UseCutS2weta2   ,
			   true,  // bool UseCutS1f1      ,
			   false, // bool UseCutS1emax2r  ,
			   false, // bool UseCutS1deltae  ,
			   false, // bool UseCutS1wtot    ,
			   false, // bool UseCutS1fracm   ,
			   false, // bool UseCutS1w1      ,
			   false, // bool UseCutS1eratio
			   10000. );
    break;
    
  case 94: // old loose4 (like loose3, but new eta binning for cut thereshold, and no Eta2 selection)
    this->LoadLooseCuts(0);
    return PhotonCutsParam(false, // bool UseCutEta2      ,
			   true,  // bool UseCutHad       ,
			   true,  // bool UseCutS2ratio1  ,
			   false, // bool UseCutS2ratio2  , 
			   true,  // bool UseCutS2weta2   ,
			   true,  // bool UseCutS1f1      ,
			   false, // bool UseCutS1emax2r  ,
			   false, // bool UseCutS1deltae  ,
			   false, // bool UseCutS1wtot    ,
			   false, // bool UseCutS1fracm   ,
			   false, // bool UseCutS1w1      ,
			   false, // bool UseCutS1eratio
			   10000. );
    break;

  case 95: // old loose5 (like loose4, but drop ratio2 cut and restore wtot cut)
    this->LoadLooseCuts(0);
    return PhotonCutsParam(false,  // bool UseCutEta2      ,
			   true,   // bool UseCutHad       ,
			   true,   // bool UseCutS2ratio1  ,
			   false,  // bool UseCutS2ratio2  ,
			   true,   // bool UseCutS2weta2   ,
			   false,  // bool UseCutS1f1      ,
			   false,  // bool UseCutS1emax2r  ,
			   false,  // bool UseCutS1deltae  ,
			   true,   // bool UseCutS1wtot    ,
			   false,  // bool UseCutS1fracm   ,
			   false,  // bool UseCutS1w1      ,
			   false, // bool UseCutS1eratio
			   10000. );
    break;

  }
  return false;
}

bool PhotonIDTool::PhotonCutsMedium(int tune, int& CutEnabButFail) {

  switch (tune) {

    case 1:
      this->LoadMediumCuts(1);
      return PhotonCutsParam(false,  // bool UseCutEta2      ,
          true,   // bool UseCutHad       ,
          true,   // bool UseCutS2ratio1  ,
          false,  // bool UseCutS2ratio2  ,
          true,   // bool UseCutS2weta2   ,
          false,  // bool UseCutS1f1      , // set to FALSE, it's now applied at the reconstruction level
          false,  // bool UseCutS1emax2r  ,
          false,  // bool UseCutS1deltae  ,
          false,  // bool UseCutS1wtot    ,
          false,  // bool UseCutS1fracm   ,
          false,  // bool UseCutS1w1      ,
          true,  // bool UseCutS1eratio
          10000. ,
          -999.,
          CutEnabButFail);
      break;
  }

  return false;
}

bool PhotonIDTool::PhotonCutsTight(int tune, int& CutEnabButFail) 
{
  this->LoadTightCuts(m_conv,tune);

  if (tune==0) {
    return PhotonCutsParam(false,  // bool UseCutEta2      , // directly implemented in cut menus
			   true,   // bool UseCutHad       ,
			   true,   // bool UseCutS2ratio1  ,
			   true,   // bool UseCutS2ratio2  ,
			   true,   // bool UseCutS2weta2   ,
			   true,   // bool UseCutS1f1      , 
			   true,   // bool UseCutS1emax2r  , // still used in first Rel 15 optimization
			   true,   // bool UseCutS1deltae  ,
			   true,   // bool UseCutS1wtot    ,
			   true,   // bool UseCutS1fracm   ,
			   true,   // bool UseCutS1w1      ,
			   false,  // bool UseCutS1eratio   // not used yet
			   -999.,
			   -999.,
			   CutEnabButFail);
  } else {
    return PhotonCutsParam(false,  // bool UseCutEta2      , // directly implemented in cut menus
			   true,   // bool UseCutHad       ,
			   true,   // bool UseCutS2ratio1  ,
			   true,   // bool UseCutS2ratio2  ,
			   true,   // bool UseCutS2weta2   ,
			   true,   // bool UseCutS1f1      , // set to FALSE? It's now applied at the reconstruction level
			   false,  // bool UseCutS1emax2r  , // replaced by eratio cut
			   true,   // bool UseCutS1deltae  ,
			   true,   // bool UseCutS1wtot    ,
			   true,   // bool UseCutS1fracm   ,
			   true,   // bool UseCutS1w1      ,
			   true,   // bool UseCutS1eratio
			   -999.,
			   -999.,
			   CutEnabButFail);
  }
}


bool PhotonIDTool::PhotonCutsParam(bool UseCutEta2      ,
				   bool UseCutHad       ,
				   bool UseCutS2ratio1  ,
				   bool UseCutS2ratio2  ,
				   bool UseCutS2weta2   ,
				   bool UseCutS1f1      ,
				   bool UseCutS1emax2r  ,
				   bool UseCutS1deltae  ,
				   bool UseCutS1wtot    ,
				   bool UseCutS1fracm   ,
				   bool UseCutS1w1      ,
				   bool UseCutS1eratio  ,
				   float FixPt          ,
				   float FixEta         ,
				   int& CutEnabButFail  )
{
  float thePt = 0;
  if (FixPt!=-999.) 
    thePt = FixPt; // use fixed value
  else
    thePt = m_pt;  // use current p_T
  
  float theEta = 0;
  if (FixEta!=-999) 
    theEta = FixEta; // use fixed value
  else
    theEta = m_eta2;  // use current eta

  int CutEnabBits = 0; // Enabled cuts
  int CutPassBits = 0; // Cuts passed

  if ( UseCutEta2 ) {
    CutEnabBits += 1;
    if ( CutEta2() )
      CutPassBits += 1;
  }

  if ( UseCutHad ) {
    CutEnabBits += 2;
    double rhad_cut = m_rhad_cut.Get(thePt,theEta);
    if ( rhad_cut && CutHad(rhad_cut) )
      CutPassBits += 2;
  }

  if ( UseCutS2ratio1 ) {
    CutEnabBits += 4;
    double ratio1_cut = m_ratio1_cut.Get(thePt,theEta);
    if ( ratio1_cut && CutS2ratio1(ratio1_cut) ) 
      CutPassBits += 4;
  }

  if ( UseCutS2ratio2 ) {
    CutEnabBits += 8;
    double ratio2_cut = m_ratio2_cut.Get(thePt,theEta);
    if ( ratio2_cut && CutS2ratio2(ratio2_cut) ) 
      CutPassBits += 8;
  }

  if ( UseCutS2weta2 ) {
    CutEnabBits += 16;
    double weta2_cut = m_weta2_cut.Get(thePt,theEta);
    if ( weta2_cut && CutS2weta2(weta2_cut) ) 
      CutPassBits += 16;
  }

  if ( UseCutS1f1 ) {
    CutEnabBits += 32;
    double f1_cut_min = m_f1_cut_min.Get(thePt,theEta);
    //double f1_cut_max = m_f1_cut_max.Get(thePt,theEta);
    if ( f1_cut_min /*&& f1_cut_max*/ && CutS1f1(f1_cut_min/*,f1_cut_max*/) ) 
      CutPassBits += 32;
  } 

  if ( UseCutS1emax2r ) {
    CutEnabBits += 64;
    double emax2r_cut = m_emax2r_cut.Get(thePt,theEta);
    if ( emax2r_cut && CutS1emax2r(emax2r_cut) ) 
      CutPassBits += 64;
  } 

  if ( UseCutS1deltae ) {
    CutEnabBits += 128;
    double deltae_cut = m_deltae_cut.Get(thePt,theEta);
    if ( deltae_cut && CutS1deltae(deltae_cut) )
      CutPassBits += 128;
  } 

  if ( UseCutS1wtot ) {
    CutEnabBits += 256;
    double wtot_cut = m_wtot_cut.Get(thePt,theEta);
    if ( wtot_cut && CutS1wtot(wtot_cut) ) 
      CutPassBits += 256;
  }

  if ( UseCutS1fracm ) {
    CutEnabBits += 512;
    double fracm_cut = m_fracm_cut.Get(thePt,theEta);
    if ( fracm_cut && CutS1fracm(fracm_cut) ) 
      CutPassBits += 512;
  }

  if ( UseCutS1w1 ) {
    CutEnabBits += 1024;
    double w1_cut = m_w1_cut.Get(thePt,theEta);
    if ( w1_cut && CutS1w1(w1_cut) )
      CutPassBits += 1024;
  }

  if ( UseCutS1eratio ) {
    CutEnabBits += 2048;
    double eratio_cut = m_eratio_cut.Get(thePt,theEta);
    if ( eratio_cut && CutS1eratio(eratio_cut) )
      CutPassBits += 2048;
  }

  // Comparing the two binary values using the bitwise-exclusive XOR operator ^
  // provides a bit for each cut that was enables but failed
  //int CutEnabButFail = ( CutEnabBits ^ CutPassBits );
  // note to self: it could be useful to return CutEnabButFail;
  //std::cout << CutEnabButFail <<std::endl;

  CutEnabButFail = ( CutEnabBits ^ CutPassBits );

  if ( CutPassBits != CutEnabBits )
    return false;
  else
    return true;

}

void PhotonIDTool::LoadLooseCuts(int tune) 
{
  //if ( tune<0 || tune>1) tune=1; //set defaults;
 
  switch (tune) {

  case 0: // development cuts (Marco Delmastro, 2009, never used)

    static double pt_DVs_0[1]  = { 1.e32 }; // no pT dependence (for the time being)
    static double eta_DVs_0[9] = { 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47 }; // new eta binning

    static double cut_rhad_loose_0[1][9]   = { {  0.006 ,  0.006 ,   0.003 ,  0.003 ,  0.003 ,  0.008 ,  0.009 ,  0.007 ,  0.007 } };
    static double cut_ratio1_loose_0[1][9] = { {  0.925 ,  0.925 ,   0.920 ,  0.920 ,  0.920 ,  0.905 ,  0.925 ,  0.915 ,  0.915 } };
    static double cut_ratio2_loose_0[1][9] = { {  0.600 ,  0.600 ,   0.600 ,  0.600 ,  0.600 ,  0.680 ,  0.740 ,  0.800 ,  0.800 } };
    static double cut_weta2_loose_0[1][9]  = { { 0.0108 , 0.0108 ,  0.0115 , 0.0115 , 0.0115 , 0.0114 , 0.0114 , 0.0123 , 0.0123 } };
    static double cut_wtot_loose_0[1][9]   = { {   2.65 ,   2.65 ,    2.75 ,   2.80 ,   2.80 ,   2.85 ,   1.75 ,   1.30 ,   1.30 } };

    m_rhad_cut.Initialize  ( 1, 9, pt_DVs_0, eta_DVs_0, &cut_rhad_loose_0[0][0],   -999. );
    m_ratio1_cut.Initialize( 1, 9, pt_DVs_0, eta_DVs_0, &cut_ratio1_loose_0[0][0], -999. );
    m_ratio2_cut.Initialize( 1, 9, pt_DVs_0, eta_DVs_0, &cut_ratio2_loose_0[0][0], -999. );
    m_weta2_cut.Initialize ( 1, 9, pt_DVs_0, eta_DVs_0, &cut_weta2_loose_0[0][0],  -999. );
    m_wtot_cut.Initialize  ( 1, 9, pt_DVs_0, eta_DVs_0, &cut_wtot_loose_0[0][0],   -999. );

    static double pt_f1_0[1]     = { 1.e32 };
    static double eta_f1_0[1]    = { 1.e32 };
    static double cut_f1_min_0[1][1] = {{ 0.005 }};
    static double cut_f1_max_0[1][1] = {{ 0.995 }};

    m_f1_cut_min.Initialize( 1, 1, pt_f1_0, eta_f1_0, &cut_f1_min_0[0][0], -999. );
    m_f1_cut_max.Initialize( 1, 1, pt_f1_0, eta_f1_0, &cut_f1_max_0[0][0], -999. );

    break;

  case 1: // default "electron" cut, first official photon loose menu (thresholds fron Nicolas, 2009-06-08)

    static double pt_DVs_1[1]  = { 100000.*1e3 }; // no pT dependence (for the time being) // same as in Athena
    static double eta_DVs_1[9] = { 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47 }; // new eta binning

    static double cut_rhad_loose_1[1][9]   = { { 0.010, 0.007, 0.006, 0.008, 0.010, 0.025, 0.021, 0.014, 0.019 } };
    static double cut_ratio1_loose_1[1][9] = { { 0.927, 0.912, 0.926, 0.916, 0.750, 0.906, 0.932, 0.913, 0.915 } };
    static double cut_weta2_loose_1[1][9]  = { { 0.012, 0.012, 0.013, 0.013, 0.025, 0.015, 0.012, 0.013, 0.012 } };

    m_rhad_cut.Initialize  ( 1, 9, pt_DVs_1, eta_DVs_1, &cut_rhad_loose_1[0][0],   -999. );
    m_ratio1_cut.Initialize( 1, 9, pt_DVs_1, eta_DVs_1, &cut_ratio1_loose_1[0][0], -999. );
    m_weta2_cut.Initialize ( 1, 9, pt_DVs_1, eta_DVs_1, &cut_weta2_loose_1[0][0],  -999. );

    static double pt_f1_1[1]     = { 1.e32 };
    static double eta_f1_1[1]    = { 1.e32 };
    static double cut_f1_min_1[1][1] = {{ 0.005 }};
    static double cut_f1_max_1[1][1] = {{ 0.995 }};
    
    m_f1_cut_min.Initialize( 1, 1, pt_f1_1, eta_f1_1, &cut_f1_min_1[0][0], -999. );
    m_f1_cut_max.Initialize( 1, 1, pt_f1_1, eta_f1_1, &cut_f1_max_1[0][0], -999. );
    
    break;

  case 2: // optimized loose menu after 2010 data analysis (relaxed cuts) (Henso Abreau, January 2011)

    static double pt_DVs_2[1]  = { 100000.*1e3 }; // no pT dependence (for the time being) // same as in Athena
    static double eta_DVs_2[9] = { 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47 }; // new eta binning

    static double cut_rhad_loose_2[1][9]   = { { 0.010, 0.010, 0.010, 0.008, 0.010, 0.025, 0.019, 0.014, 0.019 } };
    static double cut_ratio1_loose_2[1][9] = { { 0.927, 0.912, 0.921, 0.916, 0.750, 0.906, 0.920, 0.908, 0.915 } };
    static double cut_weta2_loose_2[1][9]  = { { 0.012, 0.012, 0.013, 0.013, 0.025, 0.015, 0.013, 0.013, 0.012 } };

    m_rhad_cut.Initialize  ( 1, 9, pt_DVs_2, eta_DVs_2, &cut_rhad_loose_2[0][0],   -999. );
    m_ratio1_cut.Initialize( 1, 9, pt_DVs_2, eta_DVs_2, &cut_ratio1_loose_2[0][0], -999. );
    m_weta2_cut.Initialize ( 1, 9, pt_DVs_2, eta_DVs_2, &cut_weta2_loose_2[0][0],  -999. );

    static double pt_f1_2[1]     = { 1.e32 };
    static double eta_f1_2[1]    = { 1.e32 };
    static double cut_f1_min_2[1][1] = {{ 0.005 }};
    static double cut_f1_max_2[1][1] = {{ 0.995 }};
    
    m_f1_cut_min.Initialize( 1, 1, pt_f1_2, eta_f1_2, &cut_f1_min_2[0][0], -999. );
    m_f1_cut_max.Initialize( 1, 1, pt_f1_2, eta_f1_2, &cut_f1_max_2[0][0], -999. );
    
    break;

  case 3: // optimized loose menu after 2011 data analysis (April 2011)

    static double pt_DVs_3[1]  = { 100000.*1e3 }; // no pT dependence (for the time being) // same as in Athena
    static double eta_DVs_3[9] = { 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47 }; // new eta binning

    static double cut_rhad_loose_3[1][9]   = { { 0.010, 0.010, 0.010, 0.008, 0.010, 0.025, 0.015, 0.014, 0.019 } };
    static double cut_ratio1_loose_3[1][9] = { { 0.927, 0.912, 0.925, 0.916, 0.750, 0.906, 0.920, 0.908, 0.915 } };
    static double cut_weta2_loose_3[1][9]  = { { 0.012, 0.012, 0.013, 0.013, 0.025, 0.015, 0.013, 0.013, 0.012 } };

    m_rhad_cut.Initialize  ( 1, 9, pt_DVs_3, eta_DVs_3, &cut_rhad_loose_3[0][0],   -999. );
    m_ratio1_cut.Initialize( 1, 9, pt_DVs_3, eta_DVs_3, &cut_ratio1_loose_3[0][0], -999. );
    m_weta2_cut.Initialize ( 1, 9, pt_DVs_3, eta_DVs_3, &cut_weta2_loose_3[0][0],  -999. );

    static double pt_f1_3[1]     = { 1.e32 };
    static double eta_f1_3[1]    = { 1.e32 };
    static double cut_f1_min_3[1][1] = {{ 0.005 }};
    static double cut_f1_max_3[1][1] = {{ 0.995 }};
    
    m_f1_cut_min.Initialize( 1, 1, pt_f1_3, eta_f1_3, &cut_f1_min_3[0][0], -999. );
    m_f1_cut_max.Initialize( 1, 1, pt_f1_3, eta_f1_3, &cut_f1_max_3[0][0], -999. );

    break;

  case 4: // just importing the 2012 g20 trigger cuts -- J. Saxon 

    static double pt_DVs_4[1]  = { 100000.*1e3 }; 
    static double eta_DVs_4[9] = { 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47 };

    static double cut_rhad_loose_4[1][9]   = { { 0.02425, 0.02275, 0.02575, 0.01975, 0.01975, 0.02725, 0.02725, 0.02725, 0.01975 } };
    static double cut_ratio1_loose_4[1][9] = { { 0.8825, 0.8825, 0.8575, 0.8875, 0.750, 0.8725, 0.9025, 0.8875, 0.7575 } };
    static double cut_weta2_loose_4[1][9]  = { { 0.013, 0.014, 0.015, 0.015, 0.025, 0.016, 0.015, 0.015, 0.014} };

    m_rhad_cut.Initialize  ( 1, 9, pt_DVs_4, eta_DVs_4, &cut_rhad_loose_4[0][0],   -999. );
    m_ratio1_cut.Initialize( 1, 9, pt_DVs_4, eta_DVs_4, &cut_ratio1_loose_4[0][0], -999. );
    m_weta2_cut.Initialize ( 1, 9, pt_DVs_4, eta_DVs_4, &cut_weta2_loose_4[0][0],  -999. );

    break;


  }

}


void PhotonIDTool::LoadMediumCuts(int tune) {

  // if (tune < 1 || tune > 1) tune = 1; // set default to 1 if tune is bad.

  switch (tune) {

    case 1: // first medium cuts, from the first medium trigger at g20 -- J. Saxon

      static double pt_DVs_med0[1]  = { 1.e32 };
      static double eta_DVs_med0[9] = { 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47 };

      static double cut_rhad_med0[1][9]   = {{  0.02425, 0.02275, 0.02575, 0.01975, 0.01975, 0.02725, 0.02725, 0.02725, 0.01975} };
      static double cut_reta_med0[1][9] = {{  0.8825, 0.8825, 0.8575, 0.8875, 0.750, 0.8725, 0.9025, 0.8875, 0.7575 } };
      static double cut_weta2_med0[1][9]  = {{ 0.011,  0.012,  0.012, 0.0125, 0.025, 0.013, 0.012, 0.0128, 0.0128 }};
      static double cut_eratio_med0[1][9]  = {{ 0.60,  0.76,  0.70, 0.67,  0.68  ,  0.65,  0.68 ,  0.75,  0.78 }};

      m_rhad_cut.Initialize   ( 1, 9, pt_DVs_med0, eta_DVs_med0, &cut_rhad_med0[0][0],   -999. );
      m_ratio1_cut.Initialize ( 1, 9, pt_DVs_med0, eta_DVs_med0, &cut_reta_med0[0][0], -999. );
      m_weta2_cut.Initialize  ( 1, 9, pt_DVs_med0, eta_DVs_med0, &cut_weta2_med0[0][0], -999. );
      m_eratio_cut.Initialize ( 1, 9, pt_DVs_med0, eta_DVs_med0, &cut_eratio_med0[0][0],  -999. );

      break;

  }

}

void PhotonIDTool::LoadTightCuts(bool isConv, int tune) 
{
  //if (tune>2) tune=0;
  if (tune==14) tune=4; // tune 14 aliased to 4

  // pt bins
  static double pt_DVs_unconv[4] = { 40000., 60000., 80000., 1.e32 };
  static double pt_DVs_conv[5]   = { 20000., 40000., 60000., 80000., 1.e32 };

  // two pt bins
  static double pt_2DVs[2]  = { 8e4, 1.e32 };

  // no pt bins
  static double pt_DVs[1]  = { 1.e32 };

  // eta bins
  static double eta_DVs[9] = { 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47 };

  //
  // Rel 15 default optimized menus
  //

  // unconverted

  static double cut_rhad_tight_unconv0[4][9]   = {{0.009,0.0065,0.0059,0.0051,-999., 0.025,0.0085,0.0106,-999.},
						  {0.009,0.0065,0.0059,0.0051,-999., 0.025,0.0085,0.0106,-999.},
						  {0.009,0.0065,0.0059,0.0051,-999., 0.025,0.0085,0.0106,-999.},
						  {0.009,0.0065,0.0059,0.0051,-999., 0.025,0.0085,0.0106,-999.}};

  static double cut_ratio1_tight_unconv0[4][9] = {{0.956,0.932,0.934,0.953,-999.,0.948,0.937,0.936, -999.},
						  {0.956,0.932,0.934,0.953,-999.,0.948,0.937,0.936, -999.},
						  {0.956,0.932,0.934,0.953,-999.,0.948,0.937,0.936, -999.},
						  {0.956,0.932,0.934,0.953,-999.,0.948,0.937,0.936, -999.}};

  static double cut_ratio2_tight_unconv0[4][9] = {{0.91,0.958,0.951,0.898,-999.,0.89,0.922,0.934, -999.},
						  {0.91,0.958,0.951,0.898,-999.,0.89,0.922,0.934, -999.},
						  {0.91,0.958,0.951,0.898,-999.,0.89,0.922,0.934, -999.},
						  {0.91,0.958,0.951,0.898,-999.,0.89,0.922,0.934, -999.}};

  static double cut_weta2_tight_unconv0[4][9]  = {{0.012,0.012,0.013,0.0115,-999.,0.0149,0.012,0.013, -999.},
						  {0.012,0.012,0.013,0.0115,-999.,0.0149,0.012,0.013, -999.},
						  {0.012,0.012,0.013,0.0115,-999.,0.0149,0.012,0.013, -999.},
						  {0.012,0.012,0.013,0.0115,-999.,0.0149,0.012,0.013, -999.}};

  static double cut_wtot_tight_unconv0[4][9]   = {{4.3,3.7,4.1,3.7,-999.,4.46,4.4,3.9, -999.},
						  {4.3,3.7,4.1,3.7,-999.,4.46,4.4,3.9, -999.},
						  {4.3,3.7,4.1,3.7,-999.,4.46,4.4,3.9, -999.},
						  {4.3,3.7,4.1,3.7,-999.,4.46,4.4,3.9, -999.}};

  static double cut_w1_tight_unconv0[4][9]     = {{0.675,0.728,0.81,0.786,-999.,0.767,0.699,0.591, -999.},
						  {0.675,0.728,0.81,0.786,-999.,0.767,0.699,0.591, -999.},
						  {0.675,0.728,0.81,0.786,-999.,0.767,0.699,0.591, -999.},
						  {0.675,0.728,0.81,0.786,-999.,0.767,0.699,0.591, -999.}};

  static double cut_fracm_tight_unconv0[4][9]  = {{0.251,0.422,0.448,0.62,-999.,0.976,0.174,0.167, -999.},
						  {0.251,0.422,0.448,0.62,-999.,0.976,0.174,0.167, -999.},
						  {0.251,0.422,0.448,0.62,-999.,0.976,0.174,0.167, -999.},
						  {0.251,0.422,0.448,0.62,-999.,0.976,0.174,0.167,-999.}};

  static double cut_deltae_tight_unconv0[4][9] = {{95.,97.,55.,169.,-999.,173.,114.,400.,-999.},
						  {95.,97.,55.,169.,-999.,173.,114.,400.,-999.},
						  {95.,97.,55.,169.,-999.,173.,114.,400.,-999.},
						  {95.,97.,55.,169.,-999.,173.,114.,400., -999.}};

  static double cut_emax2r_tight_unconv0[4][9] = {{0.834,0.299,0.355,0.592,-999.,0.72,0.62,0.152, -999.},
						  {0.834,0.299,0.355,0.592,-999.,0.72,0.62,0.152, -999.},
						  {0.834,0.299,0.355,0.592,-999.,0.72,0.62,0.152, -999.},
						  {0.834,0.299,0.355,0.592,-999.,0.72,0.62,0.152, -999.}};

  // converted

  static double cut_rhad_tight_conv0[5][9]   = {{0.0097,0.0069,0.0046,0.0033,-999.,0.0248,0.0129,0.0126,-999.},
						{0.0050,0.0031,0.0057,0.0041,-999.,0.0151,0.0135,0.0086,-999.},
						{0.0049,0.0035,0.0056,0.0017,-999.,0.0093,0.0205,0.0067,-999.},
						{0.0073,0.0052,0.0029,0.0020,-999.,0.0122,0.0184,0.0064,-999.},
						{0.0061,0.0051,0.0042,0.0062,-999.,0.0061,0.0156,0.0053,-999.}};

  static double cut_ratio1_tight_conv0[5][9] = {{0.9381,0.9372,0.9427,0.9361,-999.,0.9395,0.9384,0.9186,-999.},
						{0.9432,0.9355,0.9403,0.9445,-999.,0.9358,0.9384,0.9160,-999.},
						{0.9502,0.9232,0.9437,0.9329,-999.,0.9287,0.9355,0.9182,-999.},
						{0.9549,0.9514,0.9451,0.9273,-999.,0.9113,0.9365,0.9268,-999.},
						{0.9501,0.9501,0.9495,0.9439,-999.,0.9130,0.9432,0.9267,-999.}};

  static double cut_ratio2_tight_conv0[5][9] = {{0.4167,0.3864,0.5116,0.3797,-999.,0.3139,0.4076,0.3898,-999.},
						{0.4497,0.4161,0.4716,0.4725,-999.,-1.0168,0.5536,0.7005,-999.},
						{0.4374,0.5770,0.5819,0.7085,-999.,0.6866,0.4309,0.5354,-999.},
						{0.6518,0.6357,0.7595,0.7133,-999.,0.8949,0.8955,0.8783,-999.},
						{0.8638,0.8534,0.8245,0.5598,-999.,0.6736,0.6181,0.7428,-999.}};

  static double cut_weta2_tight_conv0[5][9]  = {{0.0119,0.0120,0.0116,0.0116,-999.,0.0142,0.0119,0.0127,-999.},
						{0.0120,0.0117,0.0124,0.0120,-999.,0.0122,0.0115,0.0125,-999.},
						{0.0118,0.0120,0.0123,0.0125,-999.,0.0125,0.0114,0.0127,-999.},
						{0.0106,0.0109,0.0108,0.0115,-999.,0.0138,0.0119,0.0127,-999.},
						{0.0103,0.0109,0.0113,0.0105,-999.,0.0145,0.0117,0.0117,-999.}};

  static double cut_wtot_tight_conv0[5][9]   = {{2.119,2.983,8.83,2.989,-999.,2.824,12.66,5.326,-999.},
						{17.45,8.865,3.554,3.057,-999.,18.25,9.949,1.427,-999.},
						{8.079,2.834,2.869,3.182,-999.,15.13,14.94,2.463,-999.},
						{7.478,6.669,7.637,8.311,-999.,3.92,4.35,4.752,-999.},
						{2.372,6.984,4.12,6.723,-999.,8.117,3.427,3.055,-999.}};

  static double cut_w1_tight_conv0[5][9]     = {{0.7473,0.7219,0.7422,0.8010,-999.,0.8088,0.7693,0.6266,-999.},
						{0.7326,0.7069,0.7622,0.7867,-999.,0.7222,0.6756,0.6331,-999.},
						{0.7362,0.7876,0.7429,0.7548,-999.,0.7589,0.6584,0.6185,-999.},
						{0.7000,0.6837,0.7538,0.7559,-999.,0.7859,0.6591,0.6027,-999.},
						{0.6669,0.7056,0.6992,0.7258,-999.,0.7756,0.6779,0.5946,-999.}};

  static double cut_fracm_tight_conv0[5][9]  = {{0.6193,0.5438,0.7058,1.3260,-999.,1.1600,0.2926,0.2564,-999.},
						{0.3323,0.3676,0.4317,0.6435,-999.,1.1010,0.2529,0.2059,-999.},
						{0.2997,0.3302,0.4911,0.5303,-999.,0.5424,0.3055,0.2176,-999.},
						{0.2674,1.1140,1.0270,0.4525,-999.,0.9929,1.1980,0.2103,-999.},
						{0.8747,0.5860,0.9870,0.7918,-999.,0.8818,0.3063,0.6624,-999.}};

  static double cut_deltae_tight_conv0[5][9] = {{111.7,86.56,72.28,1553,-999.,181.8,205,2324,-999.},
						{1336,833.1,75.49,118.7,-999.,108,2203,1.29e+04,-999.},
						{1059,2739,196.7,2154,-999.,5132,181.1,247.1,-999.},
						{3342,4587,2846,4218,-999.,7045,283.1,333.3,-999.},
						{464.5,209.2,455.4,587.3,-999.,9917,1832,3157,-999.}};

  static double cut_emax2r_tight_conv0[5][9] = {{0.1830, 0.0903, 0.9359, 0.4895, -999., 3.4960, 5.5990,  0.3277, -999.},
						{0.1135, 0.2613, 2.3780, 0.4201, -999., 0.9000, 0.2733,  3.4910, -999.},
						{0.1741, 0.7085, 1.2170, 0.5363, -999., 0.5344, 0.5184,  2.4460, -999.},
						{1.7390, 1.2210, 0.2985, 0.4584, -999., 0.7361, 9.3310, 11.7000, -999.},
						{0.8088, 2.7530, 0.8638, 1.5550, -999., 0.3257, 5.0570,  8.3770, -999.}};

  //
  // New optimized menus
  //

  // unconverted menu form Valeria
  static double cut_rhad_tight_unconv1[1][9]   = {{ 0.0089, 0.007,  0.006,  0.008,  -999., 0.019, 0.021,  0.0137, -999. }};
  static double cut_ratio1_tight_unconv1[1][9] = {{ 0.955,  0.946,  0.948,  0.952,  -999., 0.941, 0.948,  0.935,  -999. }};
  static double cut_ratio2_tight_unconv1[1][9] = {{ 0.954,  0.95,   0.59,   0.82,   -999., 0.93,  0.947,  0.930,  -999. }};
  static double cut_weta2_tight_unconv1[1][9]  = {{ 0.0105, 0.0112, 0.0105, 0.0111, -999., 0.011, 0.0111, 0.0125, -999. }};
  static double cut_wtot_tight_unconv1[1][9]   = {{ 2.95,   4.4,    3.26,   3.4,    -999., 3.8,   3.5,    1.99,   -999. }};
  static double cut_w1_tight_unconv1[1][9]     = {{ 0.66,   0.69,   0.697,  0.81,   -999., 0.73,  0.631,  0.57,   -999. }};
  static double cut_fracm_tight_unconv1[1][9]  = {{ 0.284,  0.36,   0.36,   0.514,  -999., 0.67,  0.211,  0.191,  -999. }};
  static double cut_deltae_tight_unconv1[1][9] = {{ 92,     92,     99,     111,    -999., 92,    110,    380,    -999. }};
  static double cut_eratio_tight_unconv1[1][9] = {{ 0.63,   0.84,   0.823,  0.887,  -999., 0.88,  0.65,   0.6,    -999. }};

  // unconverted menu form Giovanni
  static double cut_rhad_tight_unconv2[1][9]   = {{ 0.00937, 0.00790, 0.01080, 0.00642, -999., 0.02090, 0.01640, 0.00988, -999. }};
  static double cut_ratio1_tight_unconv2[1][9] = {{ 0.946,   0.940,   0.948,   0.943,   -999., 0.942,   0.946,   0.923,   -999. }};
  static double cut_ratio2_tight_unconv2[1][9] = {{ 0.955,   0.937,   0.850,   0.916,   -999., 0.928,   0.929,   0.940,   -999. }};
  static double cut_weta2_tight_unconv2[1][9]  = {{ 0.0119,  0.0115,  0.0114,  0.0115,  -999., 0.0128,  0.0112,  0.0130,  -999. }};
  static double cut_wtot_tight_unconv2[1][9]   = {{ 4.5,     2.34,    4.5,     3.5,     -999., 3.2,     3.2,     1.9,     -999. }};
  static double cut_w1_tight_unconv2[1][9]     = {{ 0.658,   0.696,   0.712,   0.709,   -999., 0.732,   0.603,   0.593,   -999. }};
  static double cut_fracm_tight_unconv2[1][9]  = {{ 0.289,   0.313,   0.363,   0.478,   -999., 0.458,   0.219,   0.165,   -999. }};
  static double cut_deltae_tight_unconv2[1][9] = {{ 107,     200,     67,      200,     -999., 300,     103,     177,     -999. }};
  static double cut_eratio_tight_unconv2[1][9] = {{ 0.884,   0.877,   0.792,   0.874,   -999., 0.916,   0.73,    0.87,    -999. }};

  // converted menu from Giovanni
  static double cut_rhad_tight_conv1[1][9]   = {{ 0.00748, 0.00700, 0.00489, 0.00800, -999., 0.01490, 0.01440, 0.01020, -999. }};
  static double cut_ratio1_tight_conv1[1][9] = {{ 0.945,   0.933,   0.936,   0.937,   -999., 0.927,   0.938,   0.922,   -999. }};
  static double cut_ratio2_tight_conv1[1][9] = {{ 0.400,   0.426,   0.493,   0.437,   -999., 0.535,   0.479,   0.692,   -999. }};
  static double cut_weta2_tight_conv1[1][9]  = {{ 0.0114,  0.0111,  0.0126,  0.0124,  -999., 0.0134,  0.0118,  0.0125,  -999. }};
  static double cut_wtot_tight_conv1[1][9]   = {{ 2.8,     2.95,    2.89,    3.14,    -999., 3.7,     2.2,     1.6,     -999. }};
  static double cut_w1_tight_conv1[1][9]     = {{ 0.697,   0.709,   0.749,   0.78,    -999., 0.773,   0.652,   0.624,   -999. }};
  static double cut_fracm_tight_conv1[1][9]  = {{ 0.32,    0.428,   0.483,   0.51,    -999., 0.508,   0.252,   0.205,   -999. }};
  static double cut_deltae_tight_conv1[1][9] = {{ 200,     200,     122,     86,      -999., 123,     300,     300,     -999. }};
  static double cut_eratio_tight_conv1[1][9] = {{ 0.908,   0.911,   0.808,   0.803,   -999., 0.67,    0.922,   0.962,   -999. }};

  // first robust menus (Marco D., September 2010)

  static double cut_rhad_tight_unconv_robust1[1][9]   = {{ 0.0089    , 0.007    , 0.006    , 0.008    , -999, 0.019     , 0.021    , 0.0137   , -999 }};
  static double cut_reta_tight_unconv_robust1[1][9]   = {{ 0.950784  , 0.9398   , 0.9418   , 0.9458   , -999, 0.932066  , 0.939519 , 0.926519 , -999 }};
  static double cut_rphi_tight_unconv_robust1[1][9]   = {{ 0.954     , 0.95     , 0.59     , 0.82     , -999, 0.93      , 0.947    , 0.93     , -999 }};
  static double cut_weta2_tight_unconv_robust1[1][9]  = {{ 0.0107194 , 0.011459 , 0.010759 , 0.011359 , -999, 0.0114125 , 0.011479 , 0.012879 , -999 }};
  static double cut_wtot_tight_unconv_robust1[1][9]   = {{ 2.95	     , 4.4      , 3.26     , 3.4      , -999, 3.8       , 3.5      , 1.99     , -999 }};
  static double cut_w1_tight_unconv_robust1[1][9]     = {{ 0.66	     , 0.69     , 0.697    , 0.81     , -999, 0.73      , 0.631    , 0.57     , -999 }};
  static double cut_fside_tight_unconv_robust1[1][9]  = {{ 0.284     , 0.36     , 0.36     , 0.514    , -999, 0.67      , 0.211    , 0.191    , -999 }};
  static double cut_deltae_tight_unconv_robust1[1][9] = {{ 92	     , 92       , 99       , 111      , -999, 92        , 110      , 380      , -999 }};
  static double cut_eratio_tight_unconv_robust1[1][9] = {{ 0.63	     , 0.84     , 0.823    , 0.887    , -999, 0.88      , 0.65     , 0.6      , -999 }};

  static double cut_rhad_tight_conv_robust1[1][9]     = {{ 0.00748   , 0.007    , 0.00489  , 0.008    , -999, 0.0149    , 0.0144   , 0.0102   , -999 }};
  static double cut_reta_tight_conv_robust1[1][9]     = {{ 0.940784  , 0.9268   , 0.9298   , 0.9308   , -999, 0.918066  , 0.932    , 0.913519 , -999 }};
  static double cut_rphi_tight_conv_robust1[1][9]     = {{ 0.4	     , 0.426    , 0.493    , 0.437    , -999, 0.535     , 0.479    , 0.692    , -999 }};
  static double cut_weta2_tight_conv_robust1[1][9]    = {{ 0.0116194 , 0.011359 , 0.012859 , 0.012659 , -999, 0.0138125 , 0.012    , 0.012879 , -999 }};
  static double cut_wtot_tight_conv_robust1[1][9]     = {{ 2.8	     , 2.95     , 2.89     , 3.14     , -999, 3.7       , 2.2      , 1.6      , -999 }};
  static double cut_w1_tight_conv_robust1[1][9]       = {{ 0.697     , 0.709    , 0.749    , 0.78     , -999, 0.773     , 0.652    , 0.624    , -999 }};
  static double cut_fside_tight_conv_robust1[1][9]    = {{ 0.32	     , 0.428    , 0.483    , 0.51     , -999, 0.508     , 0.252    , 0.205    , -999 }};
  static double cut_deltae_tight_conv_robust1[1][9]   = {{ 200	     , 200      , 122      , 86       , -999, 123       , 300      , 300      , -999 }};
  static double cut_eratio_tight_conv_robust1[1][9]   = {{ 0.908     , 0.911    , 0.808    , 0.803    , -999, 0.67      , 0.922    , 0.962    , -999 }};
  
  // optimized robust menus (Henso A., January 2011) 

  static double cut_rhad_tight_unconv_robust2[1][9]   = {{ 0.0089    , 0.007    , 0.006    , 0.008    , -999, 0.019     , 0.019    , 0.0137   , -999 }};
  static double cut_reta_tight_unconv_robust2[1][9]   = {{ 0.950784  , 0.9398   , 0.9418   , 0.9458   , -999, 0.932066  , 0.928    , 0.924    , -999 }};
  static double cut_rphi_tight_unconv_robust2[1][9]   = {{ 0.954     , 0.95     , 0.59     , 0.82     , -999, 0.93      , 0.947    , 0.935    , -999 }};
  static double cut_weta2_tight_unconv_robust2[1][9]  = {{ 0.0107194 , 0.011459 , 0.010759 , 0.011359 , -999, 0.0114125 , 0.0110   , 0.0125   , -999 }};
  static double cut_wtot_tight_unconv_robust2[1][9]   = {{ 2.95	     , 4.4      , 3.26     , 3.4      , -999, 3.8       , 2.4      , 1.64     , -999 }};
  static double cut_w1_tight_unconv_robust2[1][9]     = {{ 0.66	     , 0.69     , 0.697    , 0.81     , -999, 0.73      , 0.631    , 0.58     , -999 }};
  static double cut_fside_tight_unconv_robust2[1][9]  = {{ 0.284     , 0.36     , 0.36     , 0.514    , -999, 0.67      , 0.211    , 0.181    , -999 }};
  static double cut_deltae_tight_unconv_robust2[1][9] = {{ 92	     , 92       , 99       , 111      , -999, 92        , 110      , 148      , -999 }};
  static double cut_eratio_tight_unconv_robust2[1][9] = {{ 0.63	     , 0.84     , 0.823    , 0.887    , -999, 0.88      , 0.71     , 0.78     , -999 }};
  
  static double cut_rhad_tight_conv_robust2[1][9]     = {{ 0.00748   , 0.007    , 0.00489  , 0.008    , -999, 0.0149    , 0.016    , 0.011    , -999 }};
  static double cut_reta_tight_conv_robust2[1][9]     = {{ 0.940784  , 0.9268   , 0.9298   , 0.9308   , -999, 0.918066  , 0.924    , 0.913    , -999 }};
  static double cut_rphi_tight_conv_robust2[1][9]     = {{ 0.4	     , 0.426    , 0.493    , 0.437    , -999, 0.535     , 0.479    , 0.692    , -999 }};
  static double cut_weta2_tight_conv_robust2[1][9]    = {{ 0.0116194 , 0.011359 , 0.012859 , 0.012659 , -999, 0.0138125 , 0.012    , 0.0129   , -999 }};
  static double cut_wtot_tight_conv_robust2[1][9]     = {{ 2.8	     , 2.95     , 2.89     , 3.14     , -999, 3.7       , 2.0      , 1.48     , -999 }};
  static double cut_w1_tight_conv_robust2[1][9]       = {{ 0.697     , 0.709    , 0.749    , 0.78     , -999, 0.773     , 0.652    , 0.614    , -999 }};
  static double cut_fside_tight_conv_robust2[1][9]    = {{ 0.32	     , 0.428    , 0.483    , 0.51     , -999, 0.508     , 0.252    , 0.215    , -999 }};
  static double cut_deltae_tight_conv_robust2[1][9]   = {{ 200	     , 200      , 122      , 86       , -999, 123       , 80       , 132      , -999 }};
  static double cut_eratio_tight_conv_robust2[1][9]   = {{ 0.908     , 0.911    , 0.808    , 0.803    , -999, 0.67      , 0.915    , 0.962    , -999 }};

  // revised optimized robust menus (Martin T., February 2011)

  static double cut_rhad_tight_unconv_robust3[1][9]   = {{ 0.0089    , 0.007    , 0.006    , 0.008    , -999, 0.019     , 0.019    , 0.0137   , -999 }};
  static double cut_reta_tight_unconv_robust3[1][9]   = {{ 0.950784  , 0.9398   , 0.9418   , 0.9458   , -999, 0.932066  , 0.928    , 0.924    , -999 }};
  static double cut_rphi_tight_unconv_robust3[1][9]   = {{ 0.954     , 0.95     , 0.59     , 0.82     , -999, 0.93      , 0.947    , 0.935    , -999 }};
  static double cut_weta2_tight_unconv_robust3[1][9]  = {{ 0.0107194 , 0.011459 , 0.010759 , 0.011359 , -999, 0.0114125 , 0.0110   , 0.0125   , -999 }};
  static double cut_wtot_tight_unconv_robust3[1][9]   = {{ 2.95	     , 4.4      , 3.26     , 3.4      , -999, 3.8       , 2.4      , 1.64     , -999 }};
  static double cut_w1_tight_unconv_robust3[1][9]     = {{ 0.66      , 0.69     , 0.697    , 0.81     , -999, 0.73      , 0.651    , 0.610    , -999 }}; 
  static double cut_fside_tight_unconv_robust3[1][9]  = {{ 0.284     , 0.36     , 0.36     , 0.514    , -999, 0.67      , 0.211    , 0.181    , -999 }};
  static double cut_deltae_tight_unconv_robust3[1][9] = {{ 92	     , 92       , 99       , 111      , -999, 92        , 110      , 148      , -999 }};
  static double cut_eratio_tight_unconv_robust3[1][9] = {{ 0.63	     , 0.84     , 0.823    , 0.887    , -999, 0.88      , 0.71     , 0.78     , -999 }};
  
  static double cut_rhad_tight_conv_robust3[1][9]     = {{ 0.00748   , 0.007    , 0.00489  , 0.008    , -999, 0.0149    , 0.016    , 0.011    , -999 }};
  static double cut_reta_tight_conv_robust3[1][9]     = {{ 0.940784  , 0.9268   , 0.9298   , 0.9308   , -999, 0.918066  , 0.924    , 0.913    , -999 }};
  static double cut_rphi_tight_conv_robust3[1][9]     = {{ 0.4	     , 0.426    , 0.493    , 0.437    , -999, 0.535     , 0.479    , 0.692    , -999 }};
  static double cut_weta2_tight_conv_robust3[1][9]    = {{ 0.0116194 , 0.011359 , 0.012859 , 0.012659 , -999, 0.0138125 , 0.012    , 0.0129   , -999 }};
  static double cut_wtot_tight_conv_robust3[1][9]     = {{ 2.8	     , 2.95     , 2.89     , 3.14     , -999, 3.7       , 2.0      , 1.48     , -999 }};
  static double cut_w1_tight_conv_robust3[1][9]       = {{ 0.697     , 0.709    , 0.749    , 0.78     , -999, 0.773     , 0.672    , 0.644    , -999 }}; 
  static double cut_fside_tight_conv_robust3[1][9]    = {{ 0.32	     , 0.428    , 0.483    , 0.51     , -999, 0.508     , 0.252    , 0.215    , -999 }};
  static double cut_deltae_tight_conv_robust3[1][9]   = {{ 200	     , 200      , 122      , 86       , -999, 123       , 80       , 132      , -999 }};
  static double cut_eratio_tight_conv_robust3[1][9]   = {{ 0.908     , 0.911    , 0.808    , 0.803    , -999, 0.67      , 0.915    , 0.962    , -999 }};

  // revised optimized robust menus (Martin T., February 2011)

  static double cut_rhad_tight_unconv_robust4[1][9]   = {{ 0.0089    , 0.007    , 0.006    , 0.008    , -999, 0.019     , 0.015    , 0.0137   , -999 }};
  static double cut_reta_tight_unconv_robust4[1][9]   = {{ 0.950784  , 0.9398   , 0.9418   , 0.9458   , -999, 0.932066  , 0.928    , 0.924    , -999 }};
  static double cut_rphi_tight_unconv_robust4[1][9]   = {{ 0.954     , 0.95     , 0.59     , 0.82     , -999, 0.93      , 0.947    , 0.935    , -999 }};
  static double cut_weta2_tight_unconv_robust4[1][9]  = {{ 0.0107194 , 0.011459 , 0.010759 , 0.011359 , -999, 0.0114125 , 0.0110   , 0.0125   , -999 }};
  static double cut_wtot_tight_unconv_robust4[1][9]   = {{ 2.95	     , 4.4      , 3.26     , 3.4      , -999, 3.8       , 2.4      , 1.64     , -999 }};
  static double cut_w1_tight_unconv_robust4[1][9]     = {{ 0.66      , 0.69     , 0.697    , 0.81     , -999, 0.73      , 0.651    , 0.610    , -999 }}; 
  static double cut_fside_tight_unconv_robust4[1][9]  = {{ 0.284     , 0.36     , 0.36     , 0.514    , -999, 0.67      , 0.211    , 0.181    , -999 }};
  static double cut_deltae_tight_unconv_robust4[1][9] = {{ 92	     , 92       , 99       , 111      , -999, 92        , 110      , 148      , -999 }};
  static double cut_eratio_tight_unconv_robust4[1][9] = {{ 0.63	     , 0.84     , 0.823    , 0.887    , -999, 0.88      , 0.71     , 0.78     , -999 }};
  
  static double cut_rhad_tight_conv_robust4[1][9]     = {{ 0.00748   , 0.007    , 0.00489  , 0.008    , -999, 0.0149    , 0.015    , 0.011    , -999 }};
  static double cut_reta_tight_conv_robust4[1][9]     = {{ 0.940784  , 0.9268   , 0.9298   , 0.9308   , -999, 0.918066  , 0.924    , 0.913    , -999 }};
  static double cut_rphi_tight_conv_robust4[1][9]     = {{ 0.4	     , 0.426    , 0.493    , 0.437    , -999, 0.535     , 0.479    , 0.692    , -999 }};
  static double cut_weta2_tight_conv_robust4[1][9]    = {{ 0.0116194 , 0.011359 , 0.012859 , 0.012659 , -999, 0.0138125 , 0.012    , 0.0129   , -999 }};
  static double cut_wtot_tight_conv_robust4[1][9]     = {{ 2.8	     , 2.95     , 2.89     , 3.14     , -999, 3.7       , 2.0      , 1.48     , -999 }};
  static double cut_w1_tight_conv_robust4[1][9]       = {{ 0.697     , 0.709    , 0.749    , 0.78     , -999, 0.773     , 0.672    , 0.644    , -999 }}; 
  static double cut_fside_tight_conv_robust4[1][9]    = {{ 0.32	     , 0.428    , 0.483    , 0.51     , -999, 0.508     , 0.252    , 0.215    , -999 }};
  static double cut_deltae_tight_conv_robust4[1][9]   = {{ 200	     , 200      , 122      , 86       , -999, 123       , 80       , 132      , -999 }};
  static double cut_eratio_tight_conv_robust4[1][9]   = {{ 0.908     , 0.911    , 0.808    , 0.803    , -999, 0.67      , 0.915    , 0.962    , -999 }};

  // (Tentative) revised optimized robust menus (Marco D., January 2012)

  // * Unconverted:
  // - Changed Reta  cut in 1.81-2.0 ( 0.928   --> 0.924  )
  // - Changed Rphi  cut in 1.81-2.0 ( 0.947   --> 0.93   )
  // - Changed Weta2 cut in 1.81-2.0 ( 0.0110  --> 0.0115 )

  // * Converted:
  // - Untouched (for now)

  static double cut_rhad_tight_unconv_robust5[1][9]   = {{ 0.0089    , 0.007    , 0.006    , 0.008    , -999, 0.019     , 0.015    , 0.0137   , -999 }};
  static double cut_reta_tight_unconv_robust5[1][9]   = {{ 0.950784  , 0.9398   , 0.9418   , 0.9458   , -999, 0.932066  , 0.924    , 0.924    , -999 }};
  static double cut_rphi_tight_unconv_robust5[1][9]   = {{ 0.954     , 0.95     , 0.59     , 0.82     , -999, 0.93      , 0.935    , 0.935    , -999 }};
  static double cut_weta2_tight_unconv_robust5[1][9]  = {{ 0.0107194 , 0.011459 , 0.010759 , 0.011359 , -999, 0.0114125 , 0.0115   , 0.0125   , -999 }};
  static double cut_wtot_tight_unconv_robust5[1][9]   = {{ 2.95	     , 4.4      , 3.26     , 3.4      , -999, 3.8       , 2.4      , 1.64     , -999 }};
  static double cut_w1_tight_unconv_robust5[1][9]     = {{ 0.66      , 0.69     , 0.697    , 0.81     , -999, 0.73      , 0.651    , 0.610    , -999 }}; 
  static double cut_fside_tight_unconv_robust5[1][9]  = {{ 0.284     , 0.36     , 0.36     , 0.514    , -999, 0.67      , 0.211    , 0.181    , -999 }};
  static double cut_deltae_tight_unconv_robust5[1][9] = {{ 92	     , 92       , 99       , 111      , -999, 92        , 110      , 148      , -999 }};
  static double cut_eratio_tight_unconv_robust5[1][9] = {{ 0.63	     , 0.84     , 0.823    , 0.887    , -999, 0.88      , 0.71     , 0.78     , -999 }};
  
  static double cut_rhad_tight_conv_robust5[1][9]     = {{ 0.00748   , 0.007    , 0.00489  , 0.008    , -999, 0.0149    , 0.015    , 0.011    , -999 }};
  static double cut_reta_tight_conv_robust5[1][9]     = {{ 0.940784  , 0.9268   , 0.9298   , 0.9308   , -999, 0.918066  , 0.924    , 0.913    , -999 }};
  static double cut_rphi_tight_conv_robust5[1][9]     = {{ 0.4	     , 0.426    , 0.493    , 0.437    , -999, 0.535     , 0.479    , 0.692    , -999 }}; 
  static double cut_weta2_tight_conv_robust5[1][9]    = {{ 0.0116194 , 0.011359 , 0.012859 , 0.012659 , -999, 0.0138125 , 0.012    , 0.0129   , -999 }};
  static double cut_wtot_tight_conv_robust5[1][9]     = {{ 2.8	     , 2.95     , 2.89     , 3.14     , -999, 3.7       , 2.0      , 1.48     , -999 }};
  static double cut_w1_tight_conv_robust5[1][9]       = {{ 0.697     , 0.709    , 0.749    , 0.78     , -999, 0.773     , 0.672    , 0.644    , -999 }}; 
  static double cut_fside_tight_conv_robust5[1][9]    = {{ 0.32	     , 0.428    , 0.483    , 0.51     , -999, 0.508     , 0.252    , 0.215    , -999 }};
  static double cut_deltae_tight_conv_robust5[1][9]   = {{ 200	     , 200      , 122      , 86       , -999, 123       , 80       , 132      , -999 }};
  static double cut_eratio_tight_conv_robust5[1][9]   = {{ 0.908     , 0.911    , 0.808    , 0.803    , -999, 0.67      , 0.915    , 0.962    , -999 }};

  // "Extreme" menu for Marcos to test

//   static double cut_rhad_tight_unconv_robust6[1][9]   = {{ 0.0089,	0.0070,	0.0060,	0.0080,	-999,	0.0190,	0.0150,	0.0137,	-999  }};
//   static double cut_reta_tight_unconv_robust6[1][9]   = {{ 0.9481,	0.9371,	0.9386,	0.9426,	-999,	0.9262,	0.9228,	0.9188,	-999  }};
//   static double cut_rphi_tight_unconv_robust6[1][9]   = {{ 0.9543,	0.9503,	0.5876,	0.8176,	-999,	0.9280,	0.9432,	0.9312,	-999  }};
//   static double cut_weta2_tight_unconv_robust6[1][9]  = {{ 0.0108,	0.0116,	0.0108,	0.0114,	-999,	0.0117,	0.0112,	0.0127,	-999  }};
//   static double cut_wtot_tight_unconv_robust6[1][9]   = {{ 2.9608,	4.4108,	3.2712,	3.4112,	-999,	3.8282,	2.4203,	1.6603,	-999  }};
//   static double cut_w1_tight_unconv_robust6[1][9]     = {{ 0.6716,	0.7016,	0.7115,	0.8245,	-999,	0.7460,	0.6695,	0.6285,	-999  }};
//   static double cut_fside_tight_unconv_robust6[1][9]  = {{ 0.3492,	0.4252,	0.4087,	0.5627,	-999,	0.7920,	0.3177,	0.2877,	-999  }};
//   static double cut_deltae_tight_unconv_robust6[1][9] = {{     90,	    90,	    95,	   107,	-999,	   101,	   102,	   140,	-999  }};
//   static double cut_eratio_tight_unconv_robust6[1][9] = {{ 0.6347,	0.8447,	0.8273,	0.8913,	-999,	0.8768,	0.7075,	0.7775,	-999  }};
  							   	       	       	       	     	       	       	       		         
//   static double cut_rhad_tight_conv_robust6[1][9]     = {{ 0.0075,	0.0070,	0.0049,	0.0080,	-999,	0.0149,	0.0150,	0.0110,	-999  }};
//   static double cut_reta_tight_conv_robust6[1][9]     = {{ 0.9384,	0.9244,	0.9252,	0.9262,	-999,	0.9109,	0.9195,	0.9085,	-999  }};
//   static double cut_rphi_tight_conv_robust6[1][9]     = {{ 0.3920,	0.4180,	0.4851,	0.4291,	-999,	0.5297,	0.4814,	0.6944,	-999  }};
//   static double cut_weta2_tight_conv_robust6[1][9]    = {{ 0.0118,	0.0115,	0.0130,	0.0128,	-999,	0.0142,	0.0123,	0.0132,	-999  }};
//   static double cut_wtot_tight_conv_robust6[1][9]     = {{ 2.8158,	2.9658,	2.9143,	3.1643,	-999,	3.7331,	2.0218,	1.5018,	-999  }};
//   static double cut_w1_tight_conv_robust6[1][9]       = {{ 0.7077,	0.7197,	0.7632,	0.7942,	-999,	0.7913,	0.6926,	0.6646,	-999  }};
//   static double cut_fside_tight_conv_robust6[1][9]    = {{ 0.3968,	0.5048,	0.5826,	0.6096,	-999,	0.6632,	0.3301,	0.2931,	-999  }};
//   static double cut_deltae_tight_conv_robust6[1][9]   = {{    197,	   197,	   119,	    83,	-999,	   120,	    76,	   128,	-999  }};
//   static double cut_eratio_tight_conv_robust6[1][9]   = {{ 0.9109,	0.9139,	0.8043,	0.7993,	-999,	0.6618,	0.9183,	0.9653,	-999  }};

  // corrected for loose thresholds

  static double cut_rhad_tight_unconv_robust6[1][9]   = {{ 0.0089,	0.0070,	0.0060,	0.0080,	-999,	0.0190,	0.0150,	0.0137,	-999  }};
  static double cut_reta_tight_unconv_robust6[1][9]   = {{ 0.9481,	0.9371,	0.9386,	0.9426,	-999,	0.9262,	0.932,	0.9188,	-999  }}; 
  static double cut_rphi_tight_unconv_robust6[1][9]   = {{ 0.9543,	0.9503,	0.5876,	0.8176,	-999,	0.9280,	0.9432,	0.9312,	-999  }};
  static double cut_weta2_tight_unconv_robust6[1][9]  = {{ 0.0120,	0.0120,	0.0113,	0.0114,	-999,	0.0120,	0.0130,	0.0127,	-999  }};
  static double cut_wtot_tight_unconv_robust6[1][9]   = {{ 2.9608,	4.4108,	3.2712,	3.4112,	-999,	3.8282,	2.4203,	1.6603,	-999  }};
  static double cut_w1_tight_unconv_robust6[1][9]     = {{ 0.6716,	0.7016,	0.7115,	0.8245,	-999,	0.7460,	0.6695,	0.6285,	-999  }};
  static double cut_fside_tight_unconv_robust6[1][9]  = {{ 0.3492,	0.4252,	0.4087,	0.5627,	-999,	0.7920,	0.3177,	0.2877,	-999  }};
  static double cut_deltae_tight_unconv_robust6[1][9] = {{     90,	    90,	    95,	   107,	-999,	   101,	   102,	   140,	-999  }};
  static double cut_eratio_tight_unconv_robust6[1][9] = {{ 0.6347,	0.8447,	0.8273,	0.8913,	-999,	0.8768,	0.7075,	0.7775,	-999  }};
  							   	       	       	       	     	       	       	       		         
  static double cut_rhad_tight_conv_robust6[1][9]     = {{ 0.0075,	0.0070,	0.0049,	0.0080,	-999,	0.0149,	0.0150,	0.0110,	-999  }};
  static double cut_reta_tight_conv_robust6[1][9]     = {{ 0.9384,	0.9244,	0.9252,	0.9262,	-999,	0.9109,	0.9320,	0.9150,	-999  }};
  static double cut_rphi_tight_conv_robust6[1][9]     = {{ 0.3920,	0.4180,	0.4851,	0.4291,	-999,	0.5297,	0.4814,	0.6944,	-999  }};
  static double cut_weta2_tight_conv_robust6[1][9]    = {{ 0.0120,	0.0120,	0.0130,	0.0130,	-999,	0.0150,	0.0123,	0.0132,	-999  }};
  static double cut_wtot_tight_conv_robust6[1][9]     = {{ 2.8158,	2.9658,	2.9143,	3.1643,	-999,	3.7331,	2.0218,	1.5018,	-999  }};
  static double cut_w1_tight_conv_robust6[1][9]       = {{ 0.7077,	0.7197,	0.7632,	0.7942,	-999,	0.7913,	0.6926,	0.6646,	-999  }};
  static double cut_fside_tight_conv_robust6[1][9]    = {{ 0.3968,	0.5048,	0.5826,	0.6096,	-999,	0.6632,	0.3301,	0.2931,	-999  }};
  static double cut_deltae_tight_conv_robust6[1][9]   = {{    197,	   197,	   119,	    83,	-999,	   120,	    76,	   128,	-999  }};
  static double cut_eratio_tight_conv_robust6[1][9]   = {{ 0.9109,	0.9139,	0.8043,	0.7993,	-999,	0.6618,	0.9183,	0.9653,	-999  }};

  // marcos jimenez's -- relaxed unconverted photon cuts based on smirnoved-electron comparison (jan 26 2012)

  static double cut_rhad_tight_unconv_robust7[1][9]   = {{ 0.0089,	0.0070,	0.0060,	0.0080,	-999,	0.0190,	0.0150,	0.0137,	-999  }};
  static double cut_reta_tight_unconv_robust7[1][9]   = {{ 0.9481,	0.9371,	0.9386,	0.9426,	-999,	0.9262,	0.932,	0.9188,	-999  }}; 
  static double cut_rphi_tight_unconv_robust7[1][9]   = {{ 0.9500,	0.9500,	0.5876,	0.8176,	-999,	0.9280,	0.9300,	0.9312,	-999  }};
  static double cut_weta2_tight_unconv_robust7[1][9]  = {{ 0.0120,	0.0120,	0.0113,	0.0114,	-999,	0.0120,	0.0130,	0.0127,	-999  }};
  static double cut_wtot_tight_unconv_robust7[1][9]   = {{ 2.9608,	4.4108,	3.2712,	3.4112,	-999,	3.8282,	2.4203,	1.6603,	-999  }};
  static double cut_w1_tight_unconv_robust7[1][9]     = {{ 0.7000,	0.7016,	0.7115,	0.8245,	-999,	0.7460,	0.6695,	0.6285,	-999  }};
  static double cut_fside_tight_unconv_robust7[1][9]  = {{ 0.3492,	0.4252,	0.4087,	0.5627,	-999,	0.7920,	0.3177,	0.2877,	-999  }};
  static double cut_deltae_tight_unconv_robust7[1][9] = {{     90,	    90,	    95,	   107,	-999,	   120,	   120,	   140,	-999  }};
  static double cut_eratio_tight_unconv_robust7[1][9] = {{ 0.6347,	0.8447,	0.8273,	0.8913,	-999,	0.8600,	0.7075,	0.7775,	-999  }};
  							   	       	       	       	     	       	       	       		         
  static double cut_rhad_tight_conv_robust7[1][9]     = {{ 0.0075,	0.0070,	0.0049,	0.0080,	-999,	0.0149,	0.0150,	0.0110,	-999  }};
  static double cut_reta_tight_conv_robust7[1][9]     = {{ 0.9384,	0.9244,	0.9252,	0.9262,	-999,	0.9109,	0.9320,	0.9150,	-999  }};
  static double cut_rphi_tight_conv_robust7[1][9]     = {{ 0.3920,	0.4180,	0.4851,	0.4291,	-999,	0.5297,	0.4814,	0.6944,	-999  }};
  static double cut_weta2_tight_conv_robust7[1][9]    = {{ 0.0120,	0.0120,	0.0130,	0.0130,	-999,	0.0150,	0.0123,	0.0132,	-999  }};
  static double cut_wtot_tight_conv_robust7[1][9]     = {{ 2.8158,	2.9658,	2.9143,	3.1643,	-999,	3.7331,	2.0218,	1.5018,	-999  }};
  static double cut_w1_tight_conv_robust7[1][9]       = {{ 0.7077,	0.7197,	0.7632,	0.7942,	-999,	0.7913,	0.6926,	0.6646,	-999  }};
  static double cut_fside_tight_conv_robust7[1][9]    = {{ 0.3968,	0.5048,	0.5826,	0.6096,	-999,	0.6632,	0.3301,	0.2931,	-999  }};
  static double cut_deltae_tight_conv_robust7[1][9]   = {{    197,	   197,	   119,	    83,	-999,	   120,	    76,	   128,	-999  }};
  static double cut_eratio_tight_conv_robust7[1][9]   = {{ 0.9109,	0.9139,	0.8043,	0.7993,	-999,	0.6618,	0.9183,	0.9653,	-999  }};

  // 10 cuts -- J. Saxon                                    0.6      0.8    1.15    1.37  1.52    1.81    2.01    2.37
  static double cut_rhad_tight_unconv_10[1][9]      = {{  0.013,   0.012,  0.012,  0.012, -999,  0.020,  0.020,  0.020, -999  }};
  static double cut_reta_tight_unconv_10[1][9]      = {{  0.945,    0.94,   0.94,   0.94, -999,  0.935, 0.9325,  0.918, -999  }};
  static double cut_rphi_tight_unconv_10[1][9]      = {{  0.945,   0.935,   0.93,   0.92, -999,   0.93,   0.94,   0.93, -999  }};
  static double cut_weta2_tight_unconv_10[1][9]     = {{ 0.0108,  0.0115,  0.011, 0.0115, -999, 0.0115, 0.0111, 0.0123, -999  }};
  static double cut_wtot_tight_unconv_10[1][9]      = {{   2.75,     3.0,    3.3,    3.5, -999,    3.0,    2.1,    1.6, -999  }};
  static double cut_w1_tight_unconv_10[1][9]        = {{   0.67,    0.69,   0.69,  0.715, -999,   0.72,   0.62,   0.60, -999  }};
  static double cut_fside_tight_unconv_10[1][9]     = {{   0.28,    0.32,   0.36,   0.39, -999,   0.38,   0.21,   0.19, -999  }};
  static double cut_deltae_tight_unconv_10[1][9]    = {{    180,     170,    165,    160, -999,    260,    550,    560, -999  }};
  static double cut_eratio_tight_unconv_10[1][9]    = {{   0.83,    0.84,   0.82,   0.84, -999,   0.86,   0.88,   0.90, -999  }};

  static double cut_rhad_tight_conv_10[1][9]        = {{  0.013,   0.011,  0.012,  0.012, -999,  0.018,  0.018,  0.016, -999  }};
  static double cut_reta_tight_conv_10[1][9]        = {{  0.935,    0.93,   0.93,  0.925, -999,  0.925,   0.93,  0.915, -999  }};
  static double cut_rphi_tight_conv_10[1][9]        = {{   0.57,    0.60,   0.60,   0.64, -999,   0.68,   0.72,   0.72, -999  }};
  static double cut_weta2_tight_conv_10[1][9]       = {{  0.011,  0.0115, 0.0120, 0.0120, -999, 0.0126, 0.0120, 0.0127, -999  }};
  static double cut_wtot_tight_conv_10[1][9]        = {{    2.8,     2.9,    3.1,    3.3, -999,    3.5,    2.2,    1.8, -999  }};
  static double cut_w1_tight_conv_10[1][9]          = {{   0.73,   0.715,   0.74,   0.75, -999,   0.75,   0.66,  0.615, -999  }};
  static double cut_fside_tight_conv_10[1][9]       = {{   0.32,    0.38,   0.45,   0.50, -999,   0.50,   0.27,   0.23, -999  }};
  static double cut_deltae_tight_conv_10[1][9]      = {{    160,     160,    120,    125, -999,    350,    520,    525, -999  }};
  static double cut_eratio_tight_conv_10[1][9]      = {{   0.87,    0.86,   0.84,   0.82, -999,   0.82,   0.89,   0.90, -999  }};

  // 2012 cuts -- J. Saxon                                    0.6      0.8    1.15       1.37  1.52     1.81     2.01    2.37
  static double cut_rhad_tight_unconv_2012[2][9]      = {{   0.020,   0.020, 0.01975, 0.01825, -999, 0.02425, 0.02575, 0.02325, -999  },
                                                         { 0.01825, 0.01975, 0.01525, 0.01675, -999, 0.02125, 0.02275, 0.01975, -999  }};
  static double cut_reta_tight_unconv_2012[1][9]      = {{    0.92,    0.92,    0.93,   0.925, -999,   0.925,   0.925,   0.910, -999  }};
  static double cut_rphi_tight_unconv_2012[1][9]      = {{    0.93,    0.93,    0.93,    0.92, -999,    0.93,    0.93,    0.93, -999  }};
  static double cut_weta2_tight_unconv_2012[1][9]     = {{   0.011,  0.0115,  0.0115,  0.0115, -999,   0.012,   0.012,  0.0128, -999  }};
  static double cut_wtot_tight_unconv_2012[1][9]      = {{     3.0,     3.0,     3.3,     3.5, -999,     3.3,     2.3,     2.0, -999  }};
  static double cut_w1_tight_unconv_2012[1][9]        = {{    0.67,    0.69,    0.69,   0.715, -999,    0.72,    0.66,   0.645, -999  }};
  static double cut_fside_tight_unconv_2012[1][9]     = {{    0.28,    0.33,    0.38,   0.425, -999,    0.42,   0.255,    0.24, -999  }};
  static double cut_deltae_tight_unconv_2012[1][9]    = {{     180,     170,     165,     160, -999,     425,     500,     560, -999  }};
  static double cut_eratio_tight_unconv_2012[1][9]    = {{    0.80,    0.80,    0.76,    0.82, -999,    0.78,    0.80,    0.80, -999  }};

  static double cut_rhad_tight_conv_2012[2][9]        = {{   0.020,   0.018, 0.01975,   0.018, -999, 0.02425,   0.024,   0.024, -999  },
                                                         { 0.01825, 0.01975, 0.01525, 0.01675, -999, 0.02125, 0.02275, 0.01975, -999  }};
  static double cut_reta_tight_conv_2012[1][9]        = {{    0.92,  0.9125,   0.915,    0.91, -999,   0.908,   0.917,   0.903, -999  }};
  static double cut_rphi_tight_conv_2012[1][9]        = {{    0.57,    0.60,    0.60,    0.64, -999,    0.68,    0.72,    0.72, -999  }};
  static double cut_weta2_tight_conv_2012[1][9]       = {{   0.011,  0.0117,   0.012,  0.0120, -999,  0.0130,   0.012,  0.0127, -999  }};
  static double cut_wtot_tight_conv_2012[1][9]        = {{     2.8,     2.9,     3.1,     3.3, -999,     3.5,     2.2,     1.8, -999  }};
  static double cut_w1_tight_conv_2012[1][9]          = {{    0.73,   0.715,    0.74,    0.75, -999,    0.75,    0.69,    0.66, -999  }};
  static double cut_fside_tight_conv_2012[1][9]       = {{    0.33,    0.38,    0.46,    0.52, -999,    0.52,    0.31,    0.25, -999  }};
  static double cut_deltae_tight_conv_2012[1][9]      = {{     160,     160,     120,     125, -999,     350,     520,     525, -999  }};
  static double cut_eratio_tight_conv_2012[1][9]      = {{    0.85,    0.85,    0.80,    0.78, -999,    0.82,    0.86,    0.88, -999  }};


  if (!isConv) { // UNCONVERTED PHOTONS

    switch (tune) {

    case 0:
      m_rhad_cut.Initialize  ( 4, 9, pt_DVs_unconv, eta_DVs, &cut_rhad_tight_unconv0[0][0],   -999. );
      m_ratio1_cut.Initialize( 4, 9, pt_DVs_unconv, eta_DVs, &cut_ratio1_tight_unconv0[0][0], -999. );
      m_ratio2_cut.Initialize( 4, 9, pt_DVs_unconv, eta_DVs, &cut_ratio2_tight_unconv0[0][0], -999. );
      m_weta2_cut.Initialize ( 4, 9, pt_DVs_unconv, eta_DVs, &cut_weta2_tight_unconv0[0][0],  -999. );
      m_wtot_cut.Initialize  ( 4, 9, pt_DVs_unconv, eta_DVs, &cut_wtot_tight_unconv0[0][0],   -999. );  
      m_w1_cut.Initialize    ( 4, 9, pt_DVs_unconv, eta_DVs, &cut_w1_tight_unconv0[0][0],     -999. );
      m_fracm_cut.Initialize ( 4, 9, pt_DVs_unconv, eta_DVs, &cut_fracm_tight_unconv0[0][0],  -999. );
      m_deltae_cut.Initialize( 4, 9 ,pt_DVs_unconv, eta_DVs, &cut_deltae_tight_unconv0[0][0], -999. );
      m_emax2r_cut.Initialize( 4, 9, pt_DVs_unconv, eta_DVs, &cut_emax2r_tight_unconv0[0][0], -999. );
      break;

    case 1:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv1[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_ratio1_tight_unconv1[0][0], -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_ratio2_tight_unconv1[0][0], -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv1[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv1[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv1[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fracm_tight_unconv1[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv1[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv1[0][0], -999. );
      break;

    case 2:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv2[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_ratio1_tight_unconv2[0][0], -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_ratio2_tight_unconv2[0][0], -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv2[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv2[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv2[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fracm_tight_unconv2[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv2[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv2[0][0], -999. );
      break;

    case 3:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv_robust1[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_unconv_robust1[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_unconv_robust1[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv_robust1[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv_robust1[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv_robust1[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_unconv_robust1[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv_robust1[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv_robust1[0][0], -999. );
      break;

    case 4: 
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv_robust2[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_unconv_robust2[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_unconv_robust2[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv_robust2[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv_robust2[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv_robust2[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_unconv_robust2[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv_robust2[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv_robust2[0][0], -999. );
      break;

    case 5: 
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv_robust3[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_unconv_robust3[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_unconv_robust3[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv_robust3[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv_robust3[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv_robust3[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_unconv_robust3[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv_robust3[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv_robust3[0][0], -999. );
      break;

    case 6: 
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv_robust4[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_unconv_robust4[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_unconv_robust4[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv_robust4[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv_robust4[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv_robust4[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_unconv_robust4[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv_robust4[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv_robust4[0][0], -999. );
      break;

    case 7: 
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv_robust5[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_unconv_robust5[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_unconv_robust5[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv_robust5[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv_robust5[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv_robust5[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_unconv_robust5[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv_robust5[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv_robust5[0][0], -999. );
      break;

    case 8: 
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv_robust6[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_unconv_robust6[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_unconv_robust6[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv_robust6[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv_robust6[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv_robust6[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_unconv_robust6[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv_robust6[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv_robust6[0][0], -999. );
      break;

    case 9: 
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv_robust7[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_unconv_robust7[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_unconv_robust7[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv_robust7[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv_robust7[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv_robust7[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_unconv_robust7[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv_robust7[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv_robust7[0][0], -999. );
      break;

    case 10:  
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_unconv_10[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_unconv_10[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_unconv_10[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_unconv_10[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_unconv_10[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_unconv_10[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_unconv_10[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_unconv_10[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_unconv_10[0][0], -999. );
      break;

    case 2012: 
      m_rhad_cut.Initialize  ( 2, 9, pt_2DVs, eta_DVs, &cut_rhad_tight_unconv_2012[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs,  eta_DVs, &cut_reta_tight_unconv_2012[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs,  eta_DVs, &cut_rphi_tight_unconv_2012[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs,  eta_DVs, &cut_weta2_tight_unconv_2012[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs,  eta_DVs, &cut_wtot_tight_unconv_2012[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs,  eta_DVs, &cut_w1_tight_unconv_2012[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs,  eta_DVs, &cut_fside_tight_unconv_2012[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs,  eta_DVs, &cut_deltae_tight_unconv_2012[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs,  eta_DVs, &cut_eratio_tight_unconv_2012[0][0], -999. );
      break;

    }

  } else { // CONVERTED PHOTONS

    switch (tune) {

    case 0:
      m_rhad_cut.Initialize  ( 5, 9, pt_DVs_conv, eta_DVs, &cut_rhad_tight_conv0[0][0],   -999. );
      m_ratio1_cut.Initialize( 5, 9, pt_DVs_conv, eta_DVs, &cut_ratio1_tight_conv0[0][0], -999. );
      m_ratio2_cut.Initialize( 5, 9, pt_DVs_conv, eta_DVs, &cut_ratio2_tight_conv0[0][0], -999. );
      m_weta2_cut.Initialize ( 5, 9, pt_DVs_conv, eta_DVs, &cut_weta2_tight_conv0[0][0],  -999. );
      m_wtot_cut.Initialize  ( 5, 9, pt_DVs_conv, eta_DVs, &cut_wtot_tight_conv0[0][0],   -999. );  
      m_w1_cut.Initialize    ( 5, 9, pt_DVs_conv, eta_DVs, &cut_w1_tight_conv0[0][0],     -999. );
      m_fracm_cut.Initialize ( 5, 9, pt_DVs_conv, eta_DVs, &cut_fracm_tight_conv0[0][0],  -999. );
      m_deltae_cut.Initialize( 5, 9 ,pt_DVs_conv, eta_DVs, &cut_deltae_tight_conv0[0][0], -999. );
      m_emax2r_cut.Initialize( 5, 9, pt_DVs_conv, eta_DVs, &cut_emax2r_tight_conv0[0][0], -999. );
      break;

    case 1: // same at tune==2
    case 2:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_conv1[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_ratio1_tight_conv1[0][0], -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_ratio2_tight_conv1[0][0], -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_conv1[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_conv1[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_conv1[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fracm_tight_conv1[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_conv1[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_conv1[0][0], -999. );
      break;

    case 3:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_conv_robust1[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_conv_robust1[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_conv_robust1[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_conv_robust1[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_conv_robust1[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_conv_robust1[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_conv_robust1[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_conv_robust1[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_conv_robust1[0][0], -999. );
      break;

    case 4:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_conv_robust2[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_conv_robust2[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_conv_robust2[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_conv_robust2[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_conv_robust2[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_conv_robust2[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_conv_robust2[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_conv_robust2[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_conv_robust2[0][0], -999. );
      break;

    case 5:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_conv_robust3[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_conv_robust3[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_conv_robust3[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_conv_robust3[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_conv_robust3[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_conv_robust3[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_conv_robust3[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_conv_robust3[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_conv_robust3[0][0], -999. );
      break;

    case 6:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_conv_robust4[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_conv_robust4[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_conv_robust4[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_conv_robust4[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_conv_robust4[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_conv_robust4[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_conv_robust4[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_conv_robust4[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_conv_robust4[0][0], -999. );
      break;

    case 7:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_conv_robust5[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_conv_robust5[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_conv_robust5[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_conv_robust5[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_conv_robust5[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_conv_robust5[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_conv_robust5[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_conv_robust5[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_conv_robust5[0][0], -999. );
      break;

    case 8:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_conv_robust6[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_conv_robust6[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_conv_robust6[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_conv_robust6[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_conv_robust6[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_conv_robust6[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_conv_robust6[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_conv_robust6[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_conv_robust6[0][0], -999. );
      break;

    case 9:
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_conv_robust7[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_conv_robust7[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_conv_robust7[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_conv_robust7[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_conv_robust7[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_conv_robust7[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_conv_robust7[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_conv_robust7[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_conv_robust7[0][0], -999. );
      break;

    case 10: 
      m_rhad_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_rhad_tight_conv_10[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_reta_tight_conv_10[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_rphi_tight_conv_10[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_weta2_tight_conv_10[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs, eta_DVs, &cut_wtot_tight_conv_10[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs, eta_DVs, &cut_w1_tight_conv_10[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs, eta_DVs, &cut_fside_tight_conv_10[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs, eta_DVs, &cut_deltae_tight_conv_10[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs, eta_DVs, &cut_eratio_tight_conv_10[0][0], -999. );
      break;


    case 2012: 
      m_rhad_cut.Initialize  ( 2, 9, pt_2DVs, eta_DVs, &cut_rhad_tight_conv_2012[0][0],   -999. );
      m_ratio1_cut.Initialize( 1, 9, pt_DVs,  eta_DVs, &cut_reta_tight_conv_2012[0][0],   -999. );
      m_ratio2_cut.Initialize( 1, 9, pt_DVs,  eta_DVs, &cut_rphi_tight_conv_2012[0][0],   -999. );
      m_weta2_cut.Initialize ( 1, 9, pt_DVs,  eta_DVs, &cut_weta2_tight_conv_2012[0][0],  -999. );
      m_wtot_cut.Initialize  ( 1, 9, pt_DVs,  eta_DVs, &cut_wtot_tight_conv_2012[0][0],   -999. );  
      m_w1_cut.Initialize    ( 1, 9, pt_DVs,  eta_DVs, &cut_w1_tight_conv_2012[0][0],     -999. );
      m_fracm_cut.Initialize ( 1, 9, pt_DVs,  eta_DVs, &cut_fside_tight_conv_2012[0][0],  -999. );
      m_deltae_cut.Initialize( 1, 9 ,pt_DVs,  eta_DVs, &cut_deltae_tight_conv_2012[0][0], -999. );
      m_eratio_cut.Initialize( 1, 9, pt_DVs,  eta_DVs, &cut_eratio_tight_conv_2012[0][0], -999. );
      break;

    } 

  }

  // common to all tunes
  static double pt_f1[1]     = { 1.e32 };
  static double eta_f1[1]    = { 1.e32 };
  static double cut_f1_min[1][1] = {{ 0.005 }};
  static double cut_f1_max[1][1] = {{ 0.995 }};
  m_f1_cut_min.Initialize( 1, 1, pt_f1, eta_f1, &cut_f1_min[0][0], -999. );
  m_f1_cut_max.Initialize( 1, 1, pt_f1, eta_f1, &cut_f1_max[0][0], -999. );

}



void PhotonIDTool::LoadDefaultCuts() 
{
  static double pt_rhad[1]  = {1.e32}; // no pT dependence
  static double eta_rhad[5] = {0.70,  1.5,   1.8,   2.0,   1.e32};
  static double cut_rhad[1][5]  = { {0.006, 0.003, 0.008, 0.009, 0.007} };

  m_rhad_cut.Initialize( 1, 5, pt_rhad, eta_rhad, &cut_rhad[0][0], -999. );

  static double pt_S2cut[7]      = { 30000., 40000., 50000., 60000., 70000., 80000., 1.e32 };
  static double eta_S2cut[7]     = { 0.7, 1.0, 1.5, 1.8, 2.0, 2.37, 1.e32};

  static double cut_ratio1[7][7] = { {0.925, 0.925, 0.920, 0.905, 0.925, 0.915, -999.},
				     {0.935, 0.925, 0.925, 0.905, 0.930, 0.920, -999.},
				     {0.941, 0.932, 0.930, 0.910, 0.930, 0.920, -999.},
				     {0.943, 0.937, 0.932, 0.910, 0.930, 0.922, -999.},
				     {0.946, 0.940, 0.935, 0.916, 0.930, 0.922, -999.},
				     {0.946, 0.940, 0.937, 0.910, 0.940, 0.928, -999.},
				     {0.952, 0.946, 0.943, 0.925, 0.940, 0.930, -999.} };
  
  m_ratio1_cut.Initialize( 7, 7, pt_S2cut, eta_S2cut, &cut_ratio1[0][0], -999. );

  static double cut_ratio2[7][7] = { {0.60, 0.62, 0.60, 0.680, 0.740, 0.800, -999.},
				     {0.66, 0.69, 0.65, 0.730, 0.800, 0.860, -999.},
				     {0.75, 0.75, 0.76, 0.820, 0.870, 0.880, -999.},
				     {0.82, 0.82, 0.82, 0.860, 0.900, 0.900, -999.},
				     {0.86, 0.86, 0.86, 0.890, 0.900, 0.910, -999.},
				     {0.89, 0.88, 0.88, 0.900, 0.910, 0.925, -999.},
				     {0.92, 0.92, 0.90, 0.915, 0.926, 0.925, -999.} };

  m_ratio2_cut.Initialize( 7, 7, pt_S2cut, eta_S2cut, &cut_ratio2[0][0], -999. );

  static double cut_weta2[7][7] = { {0.0108, 0.0115, 0.0115, 0.0114, 0.0114, 0.0123, -999.},
				    {0.0105, 0.0110, 0.0113, 0.0129, 0.0112, 0.0122, -999.},
				    {0.0105, 0.0108, 0.0114, 0.0127, 0.0110, 0.0120, -999.},
				    {0.0100, 0.0105, 0.0108, 0.0127, 0.0108, 0.0118, -999.},
				    {0.0101, 0.0104, 0.0106, 0.0120, 0.0107, 0.0117, -999.},
				    {0.0098, 0.0104, 0.0107, 0.0123, 0.0108, 0.0119, -999.},
				    {0.0097, 0.0102, 0.0105, 0.0125, 0.0108, 0.0114, -999.} };

  m_weta2_cut.Initialize( 7, 7, pt_S2cut, eta_S2cut, &cut_weta2[0][0], -999. );

  static double pt_f1[1]     = { 1.e32 };
  static double eta_f1[1]    = { 1.e32 };
  static double cut_f1_min[1][1] = {{ 0.005 }};
  static double cut_f1_max[1][1] = {{ 1.000 }};

  m_f1_cut_min.Initialize( 1, 1, pt_f1, eta_f1, &cut_f1_min[0][0], -999. );
  m_f1_cut_max.Initialize( 1, 1, pt_f1, eta_f1, &cut_f1_max[0][0], -999. );

  static double pt_S1cut[8]    = { 25000., 30000., 40000., 50000., 60000., 70000., 80000., 1.e32 };
  static double eta_S1cut[8]   = { 0.7, 1.0, 1.37, 1.52, 1.8, 2.0, 2.37, 1.e32}; // return false for bin 3 and 7 (crack and high eta)

  //                                                  crack                   high eta  
  //                                                   ^^^                      ^^^
  static double cut_wtot[8][8] = { {2.15, 2.55, 2.65, -999., 2.85, 1.75, 1.30, -999.,},
				   {2.05, 2.30, 2.45, -999., 2.65, 1.60, 1.30, -999.,},
				   {2.35, 2.60, 2.62, -999., 2.95, 1.82, 1.35, -999.,},
				   {2.40, 2.60, 2.80, -999., 3.00, 1.90, 1.40, -999.,},
				   {2.55, 2.75, 2.95, -999., 2.90, 2.20, 1.50, -999.,},
				   {2.15, 2.80, 2.75, -999., 2.80, 2.40, 1.55, -999.,},
				   {2.40, 2.40, 2.65, -999., 3.00, 2.00, 1.60, -999.,},
				   {2.30, 2.60, 2.80, -999., 2.90, 2.20, 1.60, -999.,} };

  m_wtot_cut.Initialize( 8, 8, pt_S1cut, eta_S1cut, &cut_wtot[0][0], -999. );

  //                                                    crack                   high eta  
  //                                                     ^^^                      ^^^
  static double cut_emax2r[8][8] = { {130., 162., 220., -999., 357., 280., 300., -999.}, // emax2r [MeV]
				     {135., 150., 240., -999., 315., 240., 250., -999.},
				     {175., 265., 270., -999., 400., 460., 270., -999.},
				     {220., 260., 280., -999., 600., 500., 700., -999.},
				     {367., 550., 395., -999., 700., 700., 700., -999.},
				     {300., 400., 400., -999., 500., 500., 400., -999.},
				     {280., 280., 420., -999., 500., 400., 370., -999.},
				     {460., 340., 400., -999., 650., 500., 460., -999.} };

  m_emax2r_cut.Initialize( 8, 8, pt_S1cut, eta_S1cut, &cut_emax2r[0][0], -999. );

  //                                                    crack                   high eta  
  //                                                     ^^^                      ^^^
  static double cut_deltae[8][8] = { {100.,  75.,  90., -999., 200., 130., 140., -999.}, // deltaE [MeV]
				     {100.,  75.,  90., -999., 150., 140., 155., -999.},
				     {130., 130., 135., -999., 150., 190., 145., -999.},
				     {140., 110., 185., -999., 150., 320., 400., -999.},
				     {300., 260., 190., -999., 460., 540., 550., -999.},
				     {280., 400., 190., -999., 250., 500., 400., -999.},
				     {320., 280., 280., -999., 360., 500., 460., -999.},
				     {200., 200., 200., -999., 600., 500., 600., -999.} };

  m_deltae_cut.Initialize( 8, 8, pt_S1cut, eta_S1cut, &cut_deltae[0][0], -999. );
  
  //                                                      crack                      high eta  
  //                                                       ^^^                         ^^^
  static double cut_fracm[8][8] = { {0.262, 0.342, 0.402, -999., 0.462, 0.231, 0.180, -999.},
				    {0.262, 0.342, 0.402, -999., 0.452, 0.231, 0.180, -999.},
				    {0.255, 0.340, 0.385, -999., 0.450, 0.230, 0.180, -999.},
				    {0.250, 0.340, 0.385, -999., 0.450, 0.230, 0.180, -999.},
				    {0.245, 0.340, 0.380, -999., 0.450, 0.230, 0.180, -999.},
				    {0.245, 0.340, 0.400, -999., 0.450, 0.230, 0.180, -999.},
				    {0.250, 0.350, 0.400, -999., 0.440, 0.250, 0.190, -999.},
				    {0.250, 0.350, 0.400, -999., 0.450, 0.250, 0.190, -999.} };

  m_fracm_cut.Initialize( 8, 8, pt_S1cut, eta_S1cut, &cut_fracm[0][0], -999. );

  static double cut_w1[8][8] = { {0.650, 0.680, 0.680, -999., 0.720, 0.640, 0.620, -999.},
				 {0.650, 0.680, 0.680, -999., 0.680, 0.640, 0.620, -999.},
				 {0.640, 0.680, 0.700, -999., 0.720, 0.640, 0.600, -999.},
				 {0.680, 0.700, 0.720, -999., 0.740, 0.680, 0.620, -999.},
				 {0.680, 0.700, 0.700, -999., 0.720, 0.640, 0.620, -999.},
				 {0.640, 0.655, 0.700, -999., 0.700, 0.700, 0.640, -999.},
				 {0.620, 0.680, 0.700, -999., 0.740, 0.680, 0.680, -999.},
				 {0.660, 0.660, 0.680, -999., 0.740, 0.655, 0.620, -999.} };

  m_w1_cut.Initialize( 8, 8, pt_S1cut, eta_S1cut, &cut_w1[0][0], -999. );

}


bool PhotonIDTool::PhotonCutsBool(int itune) 
{
  return ( this->PhotonCutsBits(itune) == 15 );
}

int PhotonIDTool::PhotonCutsBits(int itune)
{
  this->ComputeDerivedQuantities();
  if ( m_pt<100. ) return 0;
  if ( m_eta2   == -999. ||
       m_ethad  == -999. ||
       m_e277   == -999. ||
       m_e237   == -999. ||
       m_e233   == -999. ||
       m_weta2  == -999. ||
       m_f1     == -999. ||
       m_emax2  == -999. ||
       m_emin2  == -999. ||
       m_fracm  == -999. ||
       m_wtot   == -999. ||
       m_w1     == -999. ) return -1;
  return this->cuts(itune);
}

int PhotonIDTool::cuts(int itune)
{
 int status=0;
 if (m_eta2<1.37 || (m_eta2>1.52 && m_eta2<2.37)) status+=1;

// default tuning = 2
 if (itune==0) itune=2;

 if (itune==1) {
   if (this->CutHad_tune1()) status+=2;
   if (this->CutS2_tune1()) status+=4;
   if (this->CutS1_tune1()) status+=8;
 }
 if (itune==2) {
   if (this->CutHad_tune2()) status+=2;
   if (this->CutS2_tune2()) status+=4;
   if (this->CutS1_tune2()) status+=8;
 }
 if (itune==3) {
   if (this->CutHad_tune3()) status+=2;
   if (this->CutS2_tune3()) status+=4;
   if (this->CutS1_tune3()) status+=8;
 }
 return status;
}

//
// =====================================================================================================
// tune1 = cuts from January 2007

bool PhotonIDTool::CutHad_tune1() {
  static double rcut[5]={0.010,0.0030,0.0120,0.011,0.025};

  int ibin;
  if (m_eta2<0.80) ibin=0;
  else if (m_eta2<1.5) ibin=1;
  else if (m_eta2<1.8) ibin=2;
  else if (m_eta2<2.0) ibin=3;
  else  ibin=4;
   
  if (m_rhad>rcut[ibin]) return false;
  return true;
}  

bool PhotonIDTool::CutS2_tune1() {
// new cuts
    static double   ratio1_cut[4][5]={{0.91,0.925,0.90,0.92,0.925},
                                      {0.935,0.933,0.912,0.925,0.91},
                                      {0.943,0.942,0.92,0.940,0.93},
                                      {0.953,0.947,0.925,0.945,0.93}};
    static double   ratio2_cut[4][5]={{0.65,0.65,0.76,0.80,0.75},
                                      {0.75,0.75,0.77,0.80,0.84},
                                      {0.83,0.83,0.85,0.88,0.88},
                                      {0.89,0.89,0.91,0.91,0.92}};
    static double   weta2_cut[4][5]={{0.0109,0.0115,0.0130,0.0117,0.0120},
                                     {0.0105,0.0110,0.0125,0.0112,0.0120},
                                     {0.0101,0.0107,0.0121,0.0109,0.0118},
                                     {0.0100,0.0104,0.0115,0.0105,0.0115}};


   if (m_e277<0.1) return false;
   if (m_e237<0.1) return false;

   double ratio1=m_e237/m_e277;
   double ratio2=m_e233/m_e237;
   double weta2 = m_weta2;

   int ibine;
   if (m_pt<30000.) ibine=0;
   else if (m_pt<40000.) ibine=1;
   else if (m_pt<50000.) ibine=2;
   else ibine=3;

   int ibin;
   if (m_eta2<0.80) ibin=0;
   else if (m_eta2<1.50) ibin=1;
   else if (m_eta2<1.80) ibin=2;
   else if (m_eta2<2.0)  ibin=3;
   else ibin=4;

   if (ratio1 < ratio1_cut[ibine][ibin]) return false;
   if (ratio2 < ratio2_cut[ibine][ibin]) return false;
   if (weta2 > weta2_cut[ibine][ibin]) return false;
   return true;

}

bool PhotonIDTool::CutS1_tune1() {

// emax2r is  MeV
    static double   cut_emax2r[3][5]={{180.,270.,400.,350.,350.},
                                      {180.,270.,450.,300.,300.},
                                      {180.,210.,450.,300.,500.}};

// deltaE in MeV
    static double   cut_deltae[3][5]={{140.,120.,300.,210.,250.},
                                      {140.,120.,350.,230.,300.},
                                      {140.,120.,350.,230.,300.}};

// wtot (no unit)
    static double   wtot_cut[3][5]={{2.5,2.7,3.0,2.1,1.45},
                                    {2.4,2.7,2.9,2.1,1.45},
                                    {2.2,2.7,2.8,2.1,1.45}};

// fracs1 (no unit)
    static double   fracm_cut[3][5]={{0.33,0.45,0.47,0.30,0.19},
                                     {0.30,0.43,0.47,0.27,0.18},
                                     {0.25,0.35,0.45,0.22,0.18}};
// w1 (no unit)
    static double   w1_cut[3][5]={{0.67,0.70,0.75,0.67,0.65},
                                  {0.67,0.70,0.73,0.65,0.63},
                                  {0.65,0.69,0.73,0.65,0.63}};

   if (m_f1<0.005) return false;

   int ibin=-1;
   if (m_eta2<0.80) ibin=0;
   else if (m_eta2<1.37) ibin=1;
   else if (m_eta2>1.52 && m_eta2<1.8) ibin=2;
   else if (m_eta2>1.8  && m_eta2<2.0) ibin=3;
   else if (m_eta2>2.0  && m_eta2<2.37) ibin=4;
   if (ibin<0) return false;

   int ibine=0;
   if (m_pt>30000.) ibine=1;
   if (m_pt>40000.) ibine=2;

   if (m_emax2r > cut_emax2r[ibine][ibin]) return false;
   if (m_deltae > cut_deltae[ibine][ibin]) return false;
   if (m_wtot > wtot_cut[ibine][ibin]) return false;
   if (m_fracm > fracm_cut[ibine][ibin]) return false;
   if (m_w1 > w1_cut[ibine][ibin]) return false;
    
   return true;

}


// ===============================================================================================
// tune2 from August 2007 (R.Souche)

bool PhotonIDTool::CutHad_tune2()
{
    static double rcut[5]={0.006,0.003,0.008,0.009,0.007}; //cut unconverted photon

    int ibin;
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.5) ibin=1;
    else if (m_eta2<1.8) ibin=2;
    else if (m_eta2<2.0) ibin=3;
    else  ibin=4;

    if (m_rhad>rcut[ibin]) return false;
    return true;
}

bool PhotonIDTool::CutS2_tune2()
{
// all the variables below are without unit
// new cuts
    static double ratio1_cut[7][6]={{0.925,0.925,0.920,0.905,0.925,0.915},//1% H, 60% fake
                                      {0.935,0.925,0.925,0.905,0.930,0.920},
                                      {0.941,0.932,0.930,0.910,0.93,0.92},
                                      {0.943,0.937,0.932,0.910,0.93,0.922},
                                      {0.946,0.94,0.935,0.916,0.93,0.922},
                                      {0.946,0.94,0.937,0.91,0.94,0.928},
                                      {0.952,0.946,0.943,0.925,0.94,0.93}};

    static double ratio2_cut[7][6]={{0.6,0.62,0.6,0.68,0.74,0.8}, //1% exclusion phot de H
                                      {0.66,0.69,0.65,0.73,0.8,0.86},//50% exclusion jet
                                      {0.75,0.75,0.76,0.82,0.87,0.88},
                                      {0.82,0.82,0.82,0.86,0.9,0.9},
                                      {0.86,0.86,0.86,0.89,0.9,0.91},
                                      {0.89,0.88,0.88,0.9,0.91,0.925},
                                      {0.92,0.92,0.9,0.915,0.926,0.925}};

// new cuts 
    static double weta2_cut_sup[7][6]={{0.0108,0.0115,0.0115,0.0114,0.0114,0.0123}, //rejet 2% des phot de H
                                         {0.0105,0.011,0.0113,0.0129,0.0112,0.0122}, //40 - 60% exclusion jet
                                         {0.0105,0.0108,0.0114,0.0127,0.011,0.012},
                                         {0.01,0.0105,0.0108,0.0127,0.0108,0.0118},
                                         {0.0101,0.0104,0.0106,0.012,0.0107,0.0117},
                                         {0.0098,0.0104,0.0107,0.0123,0.0108,0.0119},
                                         {0.0097,0.0102,0.0105,0.0125,0.0108,0.0114}};
                                       
   if (m_e277<0.1) {
     return false;
   }

    double ratio1=m_e237/m_e277;
    double ratio2=m_e233/m_e237;
    double weta2 = m_weta2;

    int ibine;
    if (m_pt<30000.) ibine=0; //cf CutHad ??
    else if (m_pt<40000.) ibine=1;
    else if (m_pt<50000.) ibine=2;
    else if (m_pt<60000.) ibine=3;
    else if (m_pt<70000.) ibine=4;
    else if (m_pt<80000.) ibine=5;
    else ibine=6;

// new eta division
    int ibin;
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.0) ibin=1;
    else if (m_eta2<1.50) ibin=2;
    else if (m_eta2<1.80) ibin=3;
    else if (m_eta2<2.0)  ibin=4;
    else ibin=5;


    if (ratio1 < ratio1_cut[ibine][ibin]) return false;
    if (ratio2 < ratio2_cut[ibine][ibin]) return false; 
    if (weta2 > weta2_cut_sup[ibine][ibin] ) return false;
    return true;

}

bool PhotonIDTool::CutS1_tune2()
{
// wtot_cut is without unit
// new cuts
    static double wtot_cut[8][6]={{2.15,2.55,2.65,2.85,1.75,1.3},
                                    {2.05,2.3,2.45,2.65,1.6,1.3},
                                    {2.35,2.6,2.62,2.95,1.82,1.35},
                                    {2.4,2.6,2.8,3.0,1.9,1.4},
                                    {2.55,2.75,2.95,2.9,2.2,1.5},
                                    {2.15,2.8,2.75,2.8,2.4,1.55},
                                    {2.4,2.4,2.65,3.0,2.0,1.6},
                                    {2.3,2.6,2.8,2.9,2.2,1.6}};

// emax2r is  in MeV
    static double cut_emax2r[8][6]={{130.,162.,220.,357.,280.,300.},
                                      {135.,150.,240.,315.,240.,250.},
                                      {175.,265.,270.,400.,460.,270.},
                                      {220.,260.,280.,600.,500.,700.},
                                      {367.,550.,395.,700.,700.,700.},
                                      {300.,400.,400.,500.,500.,400.},
                                      {280.,280.,420.,500.,400.,370.},
                                      {460.,340.,400.,650.,500.,460.}};

// deltaE is in energy unit (->MeV)
// new cuts
    static double cut_deltae[8][6]={{100.,75.,90.,200.,130.,140.},
                                      {100.,75.,90.,150.,140.,155.},//penser que certaines coupures ne sont pas realistes
                                      {130.,130.,135.,150.,190.,145.},//cf 1000
                                      {140.,110.,185.,150.,320.,400.},
                                      {300.,260.,190.,460.,540.,550.},
                                      {280.,400.,190.,250.,500.,400.},
                                      {320.,280.,280.,360.,500.,460.},
                                      {200.,200.,200.,600.,500.,600.}};

// fracm_cut is without unit
    static double fracm_cut[8][6]={{0.262,0.342,0.402,0.462,0.231,0.18},
                                     {0.262,0.342,0.402,0.452,0.231,0.18},
                                     {0.255,0.340,0.385,0.45,0.230,0.18},
                                     {0.25,0.340,0.385,0.45,0.23,0.18},
                                     {0.245,0.34,0.38,0.45,0.23,0.18},
                                     {0.245,0.34,0.40,0.45,0.23,0.18},
                                     {0.25,0.35,0.40,0.44,0.25,0.19},
                                     {0.25,0.35,0.40,0.45,0.25,0.19}};

// w1_cut is without unit
// new cuts
    static double w1_cut[8][6]={{0.65,0.68,0.68,0.72,0.64,0.62},
                                  {0.65,0.68,0.68,0.68,0.64,0.62},
                                  {0.64,0.68,0.7,0.72,0.64,0.6},
                                  {0.68,0.7,0.72,0.74,0.68,0.62},
                                  {0.68,0.7,0.7,0.72,0.64,0.62},
                                  {0.64,0.655,0.7,0.7,0.7,0.64},
                                  {0.62,0.68,0.7,0.74,0.68,0.68},
                                  {0.66,0.66,0.68,0.74,0.655,0.62}};
    if (m_eta2>2.37) return false;
    if (m_f1<0.005)  return false;

    int ibin=-1; 
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.00) ibin=1;
    else if (m_eta2<1.37) ibin=2;
    else if (m_eta2>1.52 && m_eta2<1.8) ibin=3;
    else if (m_eta2>1.8  && m_eta2<2.0) ibin=4;
    else if (m_eta2>2.0  && m_eta2<2.37) ibin=5;

    int ibine;
    if (m_pt<25000.) ibine=0; //cf CutHad ??
    else if (m_pt<30000.) ibine=1;
    else if (m_pt<40000.) ibine=2;
    else if (m_pt<50000.) ibine=3;
    else if (m_pt<60000.) ibine=4;
    else if (m_pt<70000.) ibine=5;
    else if (m_pt<80000.) ibine=6;
    else ibine=7; 
    if (ibin==-1) return false;

    if (m_emax2r > cut_emax2r[ibine][ibin]) return false;
    if (m_deltae > cut_deltae[ibine][ibin]) return false;

    if (m_wtot > wtot_cut[ibine][ibin]) return false;
    if (m_fracm > fracm_cut[ibine][ibin]) return false;
    if (m_w1 > w1_cut[ibine][ibin]) return false;

    return true;
}


// ==================================================================
// tune3 different cuts for converted / unconverted photons
bool PhotonIDTool::CutHad_tune3()
{
// new cuts
    double rcut_nc[5]={0.006,0.002,0.008,0.009,0.007}; //cut unconverted photon
    double rcut_c[5]={0.02,0.01,0.04,0.04,0.05}; //environ 30% fake passe pour moins de 1%de H coupe

   
    int ibin;
    if (m_eta2<0.80) ibin=0;
    else if (m_eta2<1.5) ibin=1;
    else if (m_eta2<1.8) ibin=2;
    else if (m_eta2<2.0) ibin=3;
    else  ibin=4;
   
    if (m_conv==0)
        {if (m_rhad>rcut_nc[ibin]) return false;}
    else
        {if (m_rhad>rcut_c[ibin]) return false;}
    return true;
}

bool PhotonIDTool::CutS2_tune3()
{
// all the variables below are without unit
// new cuts for unconverted photon
    static double ratio1_cut_nc[7][6]={{0.94,0.93,0.93,0.913,0.934,0.92},//1% H, 70-80% fake
                                         {0.943,0.934,0.934,0.915,0.938,0.928},
                                         {0.946,0.942,0.94,0.92,0.937,0.928},
                                         {0.95,0.94,0.938,0.922,0.94,0.928},
                                         {0.946,0.938,0.938,0.92,0.936,0.928},
                                         {0.948,0.944,0.948,0.928,0.938,0.93},
                                         {0.95,0.944,0.945,0.93,0.94,0.926}};

    static double ratio2_cut_nc[7][6]={{0.8,0.75,0.75,0.8,0.79,0.84}, 
                                         {0.9,0.86,0.86,0.86,0.88,0.9},//50% exclusion jet
                                         {0.9,0.88,0.88,0.87,0.88,0.93},
                                         {0.92,0.9,0.87,0.89,0.91,0.93},
                                         {0.93,0.926,0.915,0.925,0.93,0.93},
                                         {0.94,0.92,0.925,0.93,0.93,0.935},
                                         {0.95,0.94,0.935,0.935,0.94,0.938}};

    static double weta2_cut_inf_nc[7][6]={{0.0081,0.0085,0.0087,0.009,0.009,0.01}, //0
                                            {0.0076,0.0085,0.0087,0.009,0.009,0.01},//1
                                            {0.0078,0.0077,0.008,0.0082,0.0088,0.0095},//2
                                            {0.0075,0.0078,0.0079,0.008,0.0085,0.0094},//3
                                            {0.0075,0.0076,0.008,0.0083,0.0085,0.0094},//4
                                            {0.0075,0.0079,0.008,0.0082,0.0088,0.0095},//5
                                            {0.0072,0.0076,0.008,0.008,0.0087,0.009}};//6

    static double weta2_cut_sup_nc[7][6]={{0.0103,0.0106,0.011,0.0118,0.011,0.0118}, //rejet 2% des phot de H
                                            {0.0102,0.0105,0.011,0.012,0.0109,0.0118}, //40 - 60% exclusion jet
                                            {0.0099,0.0104,0.0107,0.0118,0.0107,0.0116},
                                            {0.0098,0.0102,0.0107,0.0116,0.0106,0.0118},
                                            {0.01,0.0104,0.0106,0.0116,0.0107,0.0115},
                                            {0.0099,0.0102,0.0104,0.012,0.0107,0.0115},
                                            {0.0097,0.0105,0.0106,0.0112,0.0106,0.0114}};

//new cuts for converted photon
    static double ratio1_cut_c[7][6]={{0.915,0.913,0.91,0.875,0.92,0.915},
                                        {0.915,0.915,0.92,0.885,0.926,0.91},
                                        {0.934,0.926,0.92,0.89,0.932,0.91},
                                        {0.938,0.932,0.925,0.89,0.928,0.915},
                                        {0.94,0.93,0.93,0.9,0.93,0.925},
                                        {0.945,0.93,0.93,0.9,0.935,0.92},
                                        {0.945,0.93,0.94,0.91,0.94,0.92}};

    static double weta2_cut_inf_c[7][6]={{0.0085,0.0086,0.0092,0.0096,0.0095,0.0105},
                                           {0.0083,0.0089,0.0092,0.0093,0.009,0.0102},
                                           {0.008,0.0085,0.0089,0.009,0.009,0.01},
                                           {0.008,0.0085,0.0087,0.009,0.009,0.0101},
                                           {0.0079,0.0085,0.0087,0.0089,0.009,0.01},
                                           {0.008,0.0087,0.0089,0.009,0.009,0.01},
                                           {0.008,0.0084,0.0084,0.0089,0.009,0.0095}};

    static double weta2_cut_sup_c[7][6]={{0.0111,0.0117,0.0116,0.0128,0.0114,0.0125},
                                           {0.0108,0.011,0.0114,0.0128,0.0113,0.0123},
                                           {0.0107,0.0109,0.0113,0.0127,0.0113,0.0121},
                                           {0.0104,0.0111,0.0112,0.0124,0.0111,0.012},
                                           {0.0103,0.0111,0.0113,0.0121,0.0112,0.012},
                                           {0.0105,0.0106,0.011,0.012,0.011,0.0117},
                                           {0.0102,0.0105,0.011,0.0115,0.0107,0.0116}};

    static double ratio2_cut_c[7][6]={{0.62,0.66,0.65,0.7,0.75,0.82},  // 2% de H exclue au min
                                        {0.64,0.69,0.65,0.78,0.84,0.86}, //20% de fake exclue au min
                                        {0.66,0.71,0.75,0.83,0.88,0.885}, // competition entre les 2
                                        {0.78,0.79,0.82,0.86,0.88,0.89}, //surtout pour les pt faibles
                                        {0.78,0.8,0.79,0.87,0.88,0.9}, //et les eta faible
                                        {0.84,0.83,0.85,0.87,0.91,0.91},
                                        {0.885,0.89,0.86,0.91,0.905,0.92}};



   if (m_e277<0.1)
   {
     return false;
   }

    double ratio1=m_e237/m_e277;
    double ratio2=m_e233/m_e237;
    double weta2 = m_weta2;

    int ibine;
    if (m_pt<30000.) ibine=0; //cf CutHad ??
    else if (m_pt<40000.) ibine=1;
    else if (m_pt<50000.) ibine=2;
    else if (m_pt<60000.) ibine=3;
    else if (m_pt<70000.) ibine=4;
    else if (m_pt<80000.) ibine=5;
    else ibine=6;

// new eta division
    int ibin;
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.0) ibin=1;
    else if (m_eta2<1.50) ibin=2;
    else if (m_eta2<1.80) ibin=3;
    else if (m_eta2<2.0)  ibin=4;
    else ibin=5;

    if (m_conv==0)
        {
        if (ratio1 < ratio1_cut_nc[ibine][ibin]) return false;
        if(weta2 < weta2_cut_inf_nc[ibine][ibin] || weta2 > weta2_cut_sup_nc[ibine][ibin]) return false;
        if (ratio2 < ratio2_cut_nc[ibine][ibin]) return false;
        }
    else
        {
        if (ratio1 < ratio1_cut_c[ibine][ibin]) return false;
        if(weta2 < weta2_cut_inf_c[ibine][ibin] || weta2 > weta2_cut_sup_c[ibine][ibin]) return false;
        if (ratio2 < ratio2_cut_c[ibine][ibin]) return false;
        }
   return true;
}

bool PhotonIDTool::CutS1_tune3()
{
//for unconverted photon
// wtot_cut is without unit
// new cuts
    static double wtot_cut_nc[8][6]={{2.15,2.55,2.65,2.85,1.75,1.3},
                                    {2.15,2.4,2.55,2.75,1.7,1.35},
                                    {2.3,2.6,2.6,2.9,1.8,1.35},
                                    {2.2,2.5,2.65,2.95,1.9,1.35},
                                    {2.45,2.65,2.65,2.9,2.1,1.4},
                                    {2.4,2.7,2.65,2.8,2.1,1.5},
                                    {2.4,2.4,2.65,3.0,2.1,1.6},
                                    {2.3,2.6,2.8,2.9,2.2,1.5}};

// emax2r is  in MeV
// new cuts
    static double cut_emax2r_nc[8][6]={{130.,162.,280.,400.,280.,300.},
                                      {135.,230.,270.,400.,242.5,200.},
                                      {175.,265.,270.,330.,460.,270.},
                                      {220.,260.,280.,600.,500.,700.},
                                      {370.,550.,395.,700.,700.,700.},
                                      {170.,230.,250.,600.,360.,400.},
                                      {280.,280.,420.,500.,400.,330.},
                                      {460.,340.,400.,650.,500.,460.}};

// deltaE is in energy unit (->MeV)
// new cuts
    static double cut_deltae_nc[8][6]={{105.,75.,90.,300.,140.,155.},
                                      {105.,75.,90.,100.,140.,155.},//penser que certaines coupures ne sont pas realistes
                                      {140.,150.,135.,175.,190.,145.},//cf 1000
                                      {150.,110.,185.,150.,320.,400.},
                                      {300.,260.,190.,460.,540.,550.},
                                      {280.,400.,190.,300.,500.,400.},
                                      {320.,280.,280.,360.,1000.,300.},
                                      {175.,170.,200.,560.,380.,560.}};
                 
// fracm_cut is without unit
    static double fracm_cut_nc[8][6]={{0.265,0.345,0.405,0.49,0.235,0.18},
                                     {0.265,0.39,0.405,0.49,0.235,0.18},
                                     {0.255,0.33,0.37,0.48,0.245,0.18},
                                     {0.25,0.325,0.37,0.45,0.23,0.17},
                                     {0.235,0.32,0.35,0.45,0.215,0.205},
                                     {0.24,0.32,0.37,0.43,0.22,0.205},
                                     {0.26,0.36,0.41,0.43,0.27,0.2},
                                     {0.245,0.325,0.355,0.485,0.225,0.19}};
// w1_cut is without unit
// new cuts
    static double w1_cut_nc[8][6]={{0.65,0.68,0.68,0.74,0.64,0.62},
                                  {0.65,0.68,0.68,0.72,0.64,0.62},
                                  {0.64,0.68,0.7,0.72,0.64,0.6},
                                  {0.68,0.7,0.72,0.74,0.68,0.62},
                                  {0.68,0.7,0.68,0.72,0.64,0.62},
                                  {0.64,0.655,0.68,0.72,0.7,0.64},
                                  {0.62,0.68,0.7,0.74,0.68,0.68},
                                  {0.66,0.66,0.68,0.74,0.655,0.62}};

    static double wtot_cut_c[8][6]={{2.15,2.45,2.65,2.95,1.7,1.25},
                                      {2.2,2.7,2.75,2.8,1.8,1.3},
                                      {2.3,2.5,2.65,3.0,1.85,1.4},
                                      {2.4,2.8,2.75,2.95,1.9,1.4},
                                      {2.25,2.65,2.75,3.05,2.2,1.4},
                                      {2.25,2.8,2.9,3.0,2.2,1.5},
                                      {2.4,2.8,3.0,3.0,1.9,1.5},
                                      {2.3,2.7,2.9,3.1,2.0,1.5}};

    static double cut_emax2r_c[8][6]={{100.,120.,220.,280.,220.,200.},
                                        {130.,130.,195.,250.,220.,240.},
                                        {175.,265.,250.,400.,260.,230.},
                                        {220.,280.,320.,400.,230.,290.},
                                        {300.,340.,320.,500.,500.,360.},
                                        {230.,300.,300.,500.,400.,290.},
                                        {210.,280.,290.,400.,270.,330.},
                                        {190.,300.,310.,500.,320.,250.}};

    static double cut_deltae_c[8][6]={{105.,75.,90.,300.,140.,155.},
                                      {105.,75.,90.,100.,140.,155.},//penser que certaines coupures ne sont pas realistes
                                      {140.,150.,135.,175.,190.,145.},//cf 1000
                                      {150.,110.,185.,150.,320.,400.},
                                      {300.,260.,190.,460.,540.,550.},
                                      {280.,400.,190.,300.,500.,400.},
                                      {320.,280.,280.,360.,1000.,300.},
                                      {175.,170.,200.,560.,380.,560.}};

    static double fracm_cut_c[8][6]={{0.27,0.35,0.39,0.45,0.23,0.2},
                                       {0.28,0.37,0.38,0.475,0.25,0.19},
                                       {0.3,0.37,0.4,0.49,0.25,0.2},
                                       {0.3,0.4,0.45,0.49,0.26,0.2},
                                       {0.32,0.45,0.45,0.48,0.26,0.2},
                                       {0.31,0.4,0.41,0.46,0.26,0.21},
                                       {0.3,0.38,0.41,0.48,0.26,0.2},
                                       {0.27,0.35,0.4,0.485,0.27,0.22}};
                                       
// w1_cut is without unit
// new cuts
    static double w1_cut_c[8][6]={{0.65,0.68,0.68,0.72,0.64,0.62},
                                  {0.65,0.68,0.68,0.68,0.64,0.62},
                                  {0.64,0.68,0.7,0.72,0.64,0.6},
                                  {0.68,0.7,0.72,0.74,0.68,0.62},
                                  {0.68,0.7,0.7,0.72,0.64,0.62},
                                  {0.64,0.655,0.7,0.72,0.7,0.64},
                                  {0.62,0.68,0.7,0.74,0.68,0.68},
                                  {0.66,0.66,0.68,0.74,0.655,0.62}};

    if (m_eta2>2.37) return false;
    if (m_f1<0.005)  return false;

    int ibin=-1;
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.00) ibin=1;
    else if (m_eta2<1.37) ibin=2;
    else if (m_eta2>1.52 && m_eta2<1.8) ibin=3;
    else if (m_eta2>1.8  && m_eta2<2.0) ibin=4;
    else if (m_eta2>2.0  && m_eta2<2.37) ibin=5;

    int ibine;
    if (m_pt<25000.) ibine=0; //cf CutHad ??
    else if (m_pt<30000.) ibine=1;
    else if (m_pt<40000.) ibine=2;
    else if (m_pt<50000.) ibine=3;
    else if (m_pt<60000.) ibine=4;
    else if (m_pt<70000.) ibine=5;
    else if (m_pt<80000.) ibine=6;
    else ibine=7; 
    if (ibin==-1) return false;


    if (m_conv == 0)
        {
        if (m_wtot > wtot_cut_nc[ibine][ibin]) return false;
        if (m_emax2r > cut_emax2r_nc[ibine][ibin]) return false;
        if(m_deltae > cut_deltae_nc[ibine][ibin]) return false;
        if (m_fracm > fracm_cut_nc[ibine][ibin]) return false;
        if (m_w1 > w1_cut_nc[ibine][ibin]) return false;
        }
    else
        {
        if (m_wtot > wtot_cut_c[ibine][ibin]) return false;
        if (m_emax2r > cut_emax2r_c[ibine][ibin]) return false;
        if (m_deltae > cut_deltae_c[ibine][ibin]) return false;
        if (m_fracm > fracm_cut_c[ibine][ibin]) return false;
        if (m_w1 > w1_cut_c[ibine][ibin]) return false;
        }

    return true;
}

