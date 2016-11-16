/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
Name:        TElectronMultiLeptonSelector

Author:      Karsten Koeneke
Created:     October 2012

Description: Electron selector tool to select objects in pure ROOT using the multi-lepton cuts
******************************************************************************/


// This class header
#include "TElectronMultiLeptonSelector.h"
// STL includes
#include <math.h>

//=============================================================================
// Constructor
//=============================================================================
Root::TElectronMultiLeptonSelector::TElectronMultiLeptonSelector(const char* name) :
  TSelectorToolBase(name),
  asg::AsgMessaging(std::string(name)),
  GeV(1000.0),
  m_cutPosition_Coverage(-9),
  m_cutPosition_RHad(-9),
  m_cutPosition_F3(-9),
  m_cutPosition_Reta(-9),
  m_cutPosition_W2(-9),
  m_cutPosition_Wstot(-9),
  m_cutPosition_Eratio(-9),
  m_cutPosition_DeltaEta(-9),
  m_cutPosition_DeltaPhiRes(-9),
  m_cutPosition_NSilicon(-9),
  m_cutPosition_NPixel(-9),
  m_cutPosition_NBlayer(-9),
  m_cutPosition_TR(-9),
  m_cutPosition_TightDeltaPhi(-9)
{
}



//=============================================================================
// Destructor
//=============================================================================
Root::TElectronMultiLeptonSelector::~TElectronMultiLeptonSelector()
{
}



//=============================================================================
// Initialize this selector tool
//=============================================================================
int Root::TElectronMultiLeptonSelector::initialize()
{
  // use an int as a StatusCode
  int sc(1);

  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:
  // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...
  // if ( m_cutPosition_nSCTMin < 0 ) sc = 0; // Exceeded the number of allowed cuts (32)


  // RHad
  m_cutPosition_Coverage = m_accept.addCut( "Coverage", "Coverage" );
  if ( m_cutPosition_Coverage < 0 ) sc = 0; 

  // RHad
  m_cutPosition_RHad = m_accept.addCut( "RHad", "RHad" );
  if ( m_cutPosition_RHad < 0 ) sc = 0; 

  // f3
  m_cutPosition_F3 = m_accept.addCut( "F3", "F3" );
  if ( m_cutPosition_F3 < 0 ) sc = 0; 

  // Reta
  m_cutPosition_Reta = m_accept.addCut( "Reta", "Reta" );
  if ( m_cutPosition_Reta < 0 ) sc = 0; 

  // w2
  m_cutPosition_W2 = m_accept.addCut( "W2", "W2" );
  if ( m_cutPosition_W2 < 0 ) sc = 0; 

  // Check the energy in the strips before cutting on it
  m_cutPosition_Wstot = m_accept.addCut( "Wstot", "Wstot" );
  if ( m_cutPosition_Wstot < 0 ) sc = 0; 

  // Eratio
  m_cutPosition_Eratio = m_accept.addCut( "Eratio", "Eratio" );
  if ( m_cutPosition_Eratio < 0 ) sc = 0; 

  // Delta Eta
  m_cutPosition_DeltaEta = m_accept.addCut( "DeltaEta", "DeltaEta" );
  if ( m_cutPosition_DeltaEta < 0 ) sc = 0; 

  // Rescale deltaPhi
  m_cutPosition_DeltaPhiRes = m_accept.addCut( "DeltaPhiRes", "DeltaPhiRes" );
  if ( m_cutPosition_DeltaPhiRes < 0 ) sc = 0; 

  // Si
  m_cutPosition_NSilicon = m_accept.addCut( "NSilicon", "NSilicon" );
  if ( m_cutPosition_NSilicon < 0 ) sc = 0; 

  //Pix
  m_cutPosition_NPixel = m_accept.addCut( "NPixel", "NPixel" );
  if ( m_cutPosition_NPixel < 0 ) sc = 0; 

  //Blayer
  
  m_cutPosition_NBlayer = m_accept.addCut( "NBLayer", "NBLayer" );
  if ( m_cutPosition_NBlayer < 0 ) sc = 0; 

  //TRT Ratio in crack
  m_cutPosition_TR = m_accept.addCut( "TR", "TR" );
  if ( m_cutPosition_TR < 0 ) sc = 0; 

  //Tight deltaPhi
  m_cutPosition_TightDeltaPhi = m_accept.addCut( "TightDeltaPhi", "TightDeltaPhi" );
  if ( m_cutPosition_TightDeltaPhi < 0 ) sc = 0; 

  return sc;
}


//=============================================================================
// Calculate the actual accept of each cut individually.
//=============================================================================
const Root::TAccept& Root::TElectronMultiLeptonSelector::accept( const double eta,
                                                                 const double eT,
                                                                 const double rHad,
                                                                 const double rHad1,
                                                                 const double Reta,
                                                                 const double w2, 
                                                                 const double f1,
                                                                 const double f3,
                                                                 const double wstot,
                                                                 const double DEmaxs1, 
                                                                 const double deltaEta,
                                                                 const int nSi,
                                                                 const int nSiDeadSensors,
                                                                 const int nPix, 
                                                                 const int nPixDeadSensors, 
                                                                 const double deltaPhiRes,
                                                                 const double dpOverp, 
                                                                 const double TRratio,
                                                                 const int nTRTTotal,
                                                                 const int nBlayerHits,
                                                                 const bool expectBlayer
                                                                 )  const
{
  // Reset the cut result bits to zero (= fail cut)
  m_accept.clear();

  // -----------------------------------------------------------

  
  //Get eta/et bins
  unsigned int eTBin = this->getEtBinH4l(eT);
  unsigned int etaBin = this->getEtaBinH4l(eta);
  
  //High Low Brem
  bool isBrem = this->getBremCategoryH4l(dpOverp, eTBin, etaBin);


  // Do the actual selection

  // Coverage
  bool doPassCoverage = (fabs(eta)>2.47 ) ? false : true ;
  if(  !doPassCoverage ){ ATH_MSG_DEBUG("Failed Coverage, et=" << eT << ", eta=" << eta);}
  m_accept.setCutResult( m_cutPosition_Coverage, doPassCoverage );

  // RHad
  bool doPassRHad = ( passRHad(rHad,rHad1,eTBin,etaBin) ) ? true : false ;
  if(  !doPassRHad ){ ATH_MSG_DEBUG("Failed RHad, et=" << eT << ", eta=" << eta);}
  m_accept.setCutResult( m_cutPosition_RHad, doPassRHad );

  // f3
  bool doPassF3 = ( eT>=90*GeV || passF3(f3,eTBin,etaBin) ) ? true : false ;
  if(  !doPassF3 ){ ATH_MSG_DEBUG("Failed F3, et=" << eT << ", eta=" << eta);}
  m_accept.setCutResult( m_cutPosition_F3, doPassF3 );

  // Reta
  bool doPassReta = ( passReta(Reta,eTBin,etaBin) ) ? true : false ;
  if(  !doPassReta ){ ATH_MSG_DEBUG("Failed Reta, et=" << eT << ", eta=" << eta);}
  m_accept.setCutResult( m_cutPosition_Reta, doPassReta );

  // w2
  bool doPassW2 = ( passW2(w2,eTBin,etaBin) ) ? true : false ;
  if(  !doPassW2 ){ ATH_MSG_DEBUG("Failed W2, et=" << eT << ", eta=" << eta);}
  m_accept.setCutResult( m_cutPosition_W2, doPassW2 );

  // Check the energy in the strips before cutting on it
  bool doPassWstot = ( f1 <= 0.005 || passWstot(wstot,eTBin,etaBin) ) ? true : false ;
  if(  !doPassWstot ){ ATH_MSG_DEBUG("Failed Wstot, et=" << eT << ", eta=" << eta );}
  m_accept.setCutResult( m_cutPosition_Wstot, doPassWstot );

  // Eratio
  bool doPassEratio = ( f1 <= 0.005 || passEratio(DEmaxs1,eTBin,etaBin) ) ? true : false ;
  if(  !doPassEratio ){ ATH_MSG_DEBUG("Failed Eratio, et=" << eT << ", eta=" << eta );}
  m_accept.setCutResult( m_cutPosition_Eratio, doPassEratio );
  
  // Delta Eta
  bool doPassDeltaEta = ( passDeltaEta(deltaEta, eTBin, etaBin) ) ? true : false ;
  if(  !doPassDeltaEta ){ ATH_MSG_DEBUG("Failed DeltaEta, et=" << eT << ", eta=" << eta );}
  m_accept.setCutResult( m_cutPosition_DeltaEta, doPassDeltaEta );

  // Rescale deltaPhi
  bool doPassDeltaPhiRes = ( passDeltaPhiRes(deltaPhiRes, isBrem, eTBin, etaBin) ) ? true : false ;
  if(  !doPassDeltaPhiRes ){ ATH_MSG_DEBUG("Failed DeltaPhiRes, et=" << eT << ", eta=" << eta );}
  m_accept.setCutResult( m_cutPosition_DeltaPhiRes, doPassDeltaPhiRes );
  
  //Si
  bool doPassNSilicon = ( (nSi + nSiDeadSensors) < 7 ) ? false : true ;
  if(  !doPassNSilicon ){ ATH_MSG_DEBUG("Failed NSilicon, et=" << eT << ", eta=" << eta );}
  m_accept.setCutResult( m_cutPosition_NSilicon, doPassNSilicon );

  //Pix
  bool doPassNPixel = ( (nPix+nPixDeadSensors) < 2 ) ? false : true ;
  if(  !doPassNPixel ){ ATH_MSG_DEBUG("Failed NPixel, et=" << eT << ", eta=" << eta );}
  m_accept.setCutResult( m_cutPosition_NPixel, doPassNPixel );
 
  //BLayer
  bool doPassNBLayer = (expectBlayer && nBlayerHits < 1) ? false : true ;
  if(  !doPassNBLayer ){ ATH_MSG_DEBUG("Failed NBLayer, et=" << eT << ", eta=" << eta );}
  m_accept.setCutResult( m_cutPosition_NBlayer, doPassNBLayer ); 
  
  //TRT Ratio in crack
  bool doPassTR = (passTR(TRratio,eta,nTRTTotal) ) ? true : false ;
  if(  !doPassTR ){ ATH_MSG_DEBUG("Failed TR, et=" << eT << ", eta=" << eta );}
  m_accept.setCutResult( m_cutPosition_TR, doPassTR );

  //TightDeltaPhi cut 
  bool doPassTightDeltaPhi = ( passTightDeltaPhi(deltaPhiRes,  expectBlayer, nBlayerHits ,eTBin, etaBin) ) ? true : false ;
  if(  !doPassTightDeltaPhi ){ ATH_MSG_DEBUG("Failed TightDeltaPhi, et=" << eT << ", eta=" << eta );}
  m_accept.setCutResult( m_cutPosition_TightDeltaPhi, doPassTightDeltaPhi );

  return m_accept;
}




// Helper functions

//---------------------------------------------------------------------------------------
// Gets the Eta bin [0-9] given the eta
unsigned int Root::TElectronMultiLeptonSelector::getEtaBinH4l(double eta) const {
  const unsigned int nEtaBins = 10;
  static const double etaBins[nEtaBins] = {0.1,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47};
  
  for(unsigned int etaBin = 0; etaBin < nEtaBins; ++etaBin){
    if(fabs(eta) < etaBins[etaBin])
      return etaBin;
  }
  return 9;
}


//---------------------------------------------------------------------------------------
// Gets the Et bin [0-10] given the et (MeV)
unsigned int Root::TElectronMultiLeptonSelector::getEtBinH4l(double eT) const {
  const unsigned int nEtBins = 10;
  static const double eTBins[nEtBins] = {5*GeV,10*GeV,15*GeV,20*GeV,30*GeV,40*GeV,50*GeV,60*GeV,70*GeV,80*GeV};
  
  for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
    if(eT < eTBins[eTBin])
      return eTBin;
  }
  return 10;
}


//----------------------------------------------------------------------------------------

// Determine whether electron is high- or low-brem using dp/p
bool Root::TElectronMultiLeptonSelector::getBremCategoryH4l(double dpOverp, unsigned int eTBin, unsigned int etaBin) const {

    //                                      0.0    0.1    0.6    0.8    1.15   1.37   1.52   1.81   2.01   2.37    2.47

 static const double bremThresholds[11][10] = {{0.098, 0.098, 0.154, 0.270, 0.306, 0.422, 0.482, 0.606, 0.306, 0.382}, // 0-5 GeV
					       {0.110, 0.110, 0.178, 0.374, 0.410, 0.478, 0.610, 0.658, 0.314, 0.410}, // 5-10 GeV
					       {0.118, 0.118, 0.194, 0.430, 0.438, 0.502, 0.642, 0.674, 0.322, 0.434}, // 10-15 GeV
					       {0.122, 0.122, 0.202, 0.438, 0.458, 0.538, 0.678, 0.722, 0.330, 0.440}, // 15-20 GeV
					       {0.142, 0.142, 0.226, 0.506, 0.574, 0.586, 0.714, 0.730, 0.338, 0.462}, // 20-30 GeV
					       {0.242, 0.242, 0.362, 0.654, 0.674, 0.630, 0.798, 0.806, 0.362, 0.490}, // 30-40 GeV
					       {0.242, 0.242, 0.362, 0.654, 0.674, 0.630, 0.798, 0.806, 0.362, 0.490}, // 40-50 GeV
					       {0.242, 0.242, 0.362, 0.654, 0.674, 0.630, 0.798, 0.806, 0.362, 0.490}, // 50-60 GeV
					       {0.242, 0.242, 0.362, 0.654, 0.674, 0.630, 0.798, 0.806, 0.362, 0.490}, // 60-70 GeV
					       {0.242, 0.242, 0.362, 0.654, 0.674, 0.630, 0.798, 0.806, 0.362, 0.490}, // 70-80 GeV
					       {0.242, 0.242, 0.362, 0.654, 0.674, 0.630, 0.798, 0.806, 0.362, 0.490}};// >80 GeV
	
  if(dpOverp < bremThresholds[eTBin][etaBin]) return false; // Low-brem electron
  return true; // High-brem electron
}

//----------------------------------------------------------------------------------------
bool Root::TElectronMultiLeptonSelector::passRHad(double rhad, double rhad1,unsigned int etbin,unsigned int etabin) const {


  static const double cutrhad[11][10] = {{ 0.258,  0.258, 0.208,  0.206,  0.256,  0.188,  0.254,  0.254,  0.226,  0.176 } // 0 - 5 GeV
                                         ,{ 0.155,  0.155, 0.115,  0.125,  0.125,  0.100,  0.140,  0.135,  0.125,  0.105 } // 5 - 10 GeV
                                         ,{ 0.075,  0.075, 0.065,  0.065,  0.065,  0.055,  0.080,  0.080,  0.065,  0.050 } // 10 - 15 GeV
                                         ,{ 0.055,  0.055, 0.045,  0.045,  0.045,  0.040,  0.055,  0.055,  0.050,  0.040 } // 15 - 20 GeV
                                         ,{ 0.038,  0.038, 0.032,  0.032,  0.032,  0.026,  0.040,  0.040,  0.035,  0.030 } // 20 - 30 GeV
                                         ,{ 0.025,  0.025, 0.022,  0.022,  0.022,  0.019,  0.030,  0.030,  0.025,  0.020 } // 30 - 40 GeV
                                         ,{ 0.025,  0.025, 0.021,  0.022,  0.021,  0.019,  0.028,  0.028,  0.023,  0.020 } // 40 - 50 GeV
                                         ,{ 0.025,  0.025, 0.021,  0.022,  0.021,  0.019,  0.028,  0.028,  0.023,  0.020 } // 50 - 60 GeV
                                         ,{ 0.025,  0.025, 0.021,  0.022,  0.021,  0.019,  0.028,  0.028,  0.023,  0.020 } // 60 - 70 GeV
                                         ,{ 0.025,  0.025, 0.021,  0.022,  0.021,  0.019,  0.028,  0.028,  0.023,  0.020 } // 70 - 80 GeV
                                         ,{ 0.025,  0.025, 0.021,  0.022,  0.021,  0.019,  0.028,  0.028,  0.023,  0.020 } }; // 80 - inf GeV

  if(etabin == 3 || etabin == 4){
    if (rhad > cutrhad[etbin][etabin])
      return false;
  } else{
    if(rhad1 > cutrhad[etbin][etabin])
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------
bool Root::TElectronMultiLeptonSelector::passF3(double f3,unsigned int etbin,unsigned int etabin) const{

  //                             0.0     0.1       0.6       0.8    1.15      1.37     1.52    1.81    2.01     2.37     2.47
  static const double cutf3[11][10] = {{  0.0593 , 0.0593 , 0.0540 , 0.0430 , 0.0481 ,  9999 , 0.0363 , 0.0391 , 0.0420 , 9999 } // 0 - 5 GeV
                                       ,{ 0.0377 , 0.0377 , 0.0356 , 0.0327 , 0.0365 ,  9999 , 0.0275 , 0.0315 , 0.0360 , 9999 } // 5 - 10 GeV
                                       ,{ 0.0279 , 0.0279 , 0.0261 , 0.0239 , 0.0267 ,  9999 , 0.0217 , 0.0261 , 0.0270 , 9999 } // 10 - 15 GeV
                                       ,{ 0.0259 , 0.0259 , 0.0219 , 0.0211 , 0.0239 ,  9999 , 0.0203 , 0.0253 , 0.0270 , 9999 } // 15 - 20 GeV
                                       ,{ 0.0252 , 0.0252 , 0.0199 , 0.0196 , 0.0225 ,  9999 , 0.0207 , 0.0261 , 0.0270 , 9999 } // 20 - 30 GeV
                                       ,{ 0.0259 , 0.0259 , 0.0197 , 0.0193 , 0.0215 ,  9999 , 0.0223 , 0.0274 , 0.0270 , 9999 } // 30 - 40 GeV
                                       ,{ 0.0265 , 0.0265 , 0.0201 , 0.0201 , 0.0222 ,  9999 , 0.0240 , 0.0291 , 0.0290 , 9999 } // 40 - 50 GeV
                                       ,{ 0.0265 , 0.0265 , 0.0201 , 0.0201 , 0.0222 ,  9999 , 0.0240 , 0.0291 , 0.0290 , 9999 } // 50 - 60 GeV
                                       ,{ 0.0292 , 0.0292 , 0.0219 , 0.0215 , 0.0241 ,  9999 , 0.0264 , 0.0327 , 0.0315 , 9999 } // 60 - 70 GeV
                                       ,{ 0.0292 , 0.0292 , 0.0219 , 0.0215 , 0.0241 ,  9999 , 0.0264 , 0.0327 , 0.0315 , 9999 } // 70 - 80 GeV
                                       ,{  9999  ,  9999  ,  9999  ,  9999  ,  9999  ,  9999 ,  9999  ,  9999  ,  9999  , 9999 } }; // 80 - inf GeV
  
  if(f3 > cutf3[etbin][etabin]) return false;
  
  return true;
}

//----------------------------------------------------------------------------------------
bool Root::TElectronMultiLeptonSelector::passReta(double rEta, unsigned int eTBin, unsigned int etaBin) const {
 
  //                                    0.0     0.1      0.6       0.8     1.15      1.37     1.52     1.81     2.01     2.37     2.47
  static const double cutReta37[11][10] = {{    0.600 ,  0.600 ,  0.600 ,  0.600 ,  0.650 ,  0.650 ,  0.700 ,  0.690 ,  0.690 ,  0.690 }    // 0 - 5 GeV
                                           , {  0.750 ,  0.750 ,  0.730 ,  0.730 ,  0.735 ,  0.700 ,  0.735 ,  0.735 ,  0.775 ,  0.775 }    // 5 - 10 GeV
                                           , {  0.827 ,  0.827 ,  0.804 ,  0.826 ,  0.770 ,  0.700 ,  0.770 ,  0.802 ,  0.830 ,  0.830 }    // 10 - 15 GeV
                                           , {  0.863 ,  0.863 ,  0.845 ,  0.826 ,  0.770 ,  0.700 ,  0.813 ,  0.847 ,  0.853 ,  0.853 }    // 15 - 20 GeV
                                           , {  0.893 ,  0.893 ,  0.878 ,  0.864 ,  0.839 ,  0.700 ,  0.851 ,  0.873 ,  0.879 ,  0.878 }    // 20 - 30 GeV
                                           , {  0.893 ,  0.893 ,  0.878 ,  0.864 ,  0.839 ,  0.700 ,  0.851 ,  0.873 ,  0.879 ,  0.878 }    // 30 - 40 GeV
                                           , {  0.917 ,  0.917 ,  0.908 ,  0.900 ,  0.888 ,  0.700 ,  0.883 ,  0.898 ,  0.898 ,  0.896 }    // 40 - 50 GeV
                                           , {  0.917 ,  0.917 ,  0.908 ,  0.900 ,  0.888 ,  0.700 ,  0.883 ,  0.898 ,  0.898 ,  0.896 }    // 50 - 60 GeV
                                           , {  0.917 ,  0.917 ,  0.908 ,  0.900 ,  0.888 ,  0.700 ,  0.883 ,  0.898 ,  0.898 ,  0.896 }    // 60 - 70 GeV
                                           , {  0.917 ,  0.917 ,  0.908 ,  0.900 ,  0.888 ,  0.700 ,  0.883 ,  0.898 ,  0.898 ,  0.896 }    // 70 - 80 GeV
                                           , {  0.917 ,  0.917 ,  0.908 ,  0.900 ,  0.888 ,  0.700 ,  0.883 ,  0.898 ,  0.898 ,  0.896 } }; // 80 - inf GeV
  if(rEta < cutReta37[eTBin][etaBin]) return false;
  return true;
}

//----------------------------------------------------------------------------------------

bool Root::TElectronMultiLeptonSelector::passW2(double w2, unsigned int eTBin, unsigned int etaBin) const{

  //                                  0.0       0.1      0.6     0.8       1.15      1.37    1.52     1.81      2.01      2.37     2.47
  static const double cutWeta2[11][10] = { {   0.0166 , 0.0166 , 0.0172 , 0.0167 , 0.0170 , 0.0385 , 0.0164 , 0.0152 , 0.0156 , 0.0157 }   // 0 - 5 GeV
                                           , { 0.0145 , 0.0145 , 0.0152 , 0.0154 , 0.0158 , 0.0347 , 0.0159 , 0.0140 , 0.0150 , 0.0150 }   // 5 - 10 GeV
                                           , { 0.0129 , 0.0129 , 0.0137 , 0.0141 , 0.0146 , 0.0311 , 0.0151 , 0.0133 , 0.0140 , 0.0140 }   // 10 - 15 GeV
                                           , { 0.0122 , 0.0122 , 0.0129 , 0.0133 , 0.0139 , 0.0278 , 0.0145 , 0.0128 , 0.0140 , 0.0140 }   // 15 - 20 GeV
                                           , { 0.0117 , 0.0117 , 0.0123 , 0.0126 , 0.0131 , 0.0257 , 0.0139 , 0.0124 , 0.0135 , 0.0135 }   // 20 - 30 GeV
                                           , { 0.0117 , 0.0117 , 0.0123 , 0.0126 , 0.0131 , 0.0257 , 0.0139 , 0.0124 , 0.0135 , 0.0135 }   // 30 - 40 GeV
                                           , { 0.0112 , 0.0112 , 0.0118 , 0.0121 , 0.0125 , 0.0247 , 0.0132 , 0.0120 , 0.0130 , 0.0135 }   // 40 - 50 GeV
                                           , { 0.0112 , 0.0112 , 0.0118 , 0.0121 , 0.0125 , 0.0247 , 0.0132 , 0.0120 , 0.0130 , 0.0135 }   // 50 - 60 GeV
                                           , { 0.0112 , 0.0112 , 0.0118 , 0.0121 , 0.0125 , 0.0247 , 0.0132 , 0.0120 , 0.0130 , 0.0135 }   // 60 - 70 GeV
                                           , { 0.0112 , 0.0112 , 0.0118 , 0.0121 , 0.0125 , 0.0247 , 0.0132 , 0.0120 , 0.0130 , 0.0135 }   // 70 - 80 GeV
                                           , { 0.0112 , 0.0112 , 0.0118 , 0.0121 , 0.0125 , 0.0247 , 0.0132 , 0.0120 , 0.0130 , 0.0135 } };// 80 - inf GeV
  

  if(w2 > cutWeta2[eTBin][etaBin]) return false;  
  return  true;
}

//----------------------------------------------------------------------------------------
bool Root::TElectronMultiLeptonSelector::passWstot(double wstot, unsigned int eTBin, unsigned int etaBin) const {  

  //                                       0.0     0.1      0.6       0.8       1.15      1.37      1.52      1.81      2.01      2.37     2.47
  static const double cutWstot[11][10] = { {   3.926,   3.926,   4.069,   4.501,   4.986,   9999,   4.650,   3.190,   1.966,   9999 }    // 0 - 5 GeV
                                           , { 3.296,   3.296,   3.427,   3.936,   4.309,   9999,   4.313,   2.845,   1.818,   9999 }    // 5 - 10 GeV
                                           , { 3.095,   3.095,   3.202,   3.708,   4.095,   9999,   3.968,   2.692,   1.754,   9999 }    // 10 - 15 GeV
                                           , { 3.035,   3.035,   3.129,   3.553,   3.941,   9999,   3.758,   2.555,   1.714,   9999 }    // 15 - 20 GeV
                                           , { 3.035,   3.035,   3.129,   3.508,   3.793,   9999,   3.609,   2.505,   1.703,   9999 }    // 20 - 30 GeV
                                           , { 2.881,   2.881,   2.941,   3.319,   3.506,   9999,   3.380,   2.381,   1.644,   9999 }    // 30 - 40 GeV
                                           , { 2.881,   2.881,   2.941,   3.319,   3.506,   9999,   3.380,   2.381,   1.644,   9999 }    // 40 - 50 GeV
                                           , { 2.881,   2.881,   2.941,   3.319,   3.506,   9999,   3.380,   2.381,   1.644,   9999 }    // 50 - 60 GeV
                                           , { 2.881,   2.881,   2.941,   3.319,   3.506,   9999,   3.380,   2.381,   1.644,   9999 }    // 60 - 70 GeV
                                           , { 2.881,   2.881,   2.941,   3.319,   3.506,   9999,   3.380,   2.381,   1.644,   9999 }    // 70 - 80 GeV
                                           , { 2.881,   2.881,   2.941,   3.319,   3.506,   9999,   3.380,   2.381,   1.644,   9999 } }; // 80 - inf GeV
  
  if(wstot > cutWstot[eTBin][etaBin]) return false;
  return  true;
}
//----------------------------------------------------------------------------------------
bool Root::TElectronMultiLeptonSelector::passEratio(double DEmaxs1, unsigned int eTBin, unsigned int etaBin) const {  

    static const double cutDEmaxs1[11][10] = { {    0.278 ,  0.278 ,  0.150 ,  0.150 ,  0.250 , -9999 ,  0.100 ,  0.136 ,  0.492 , -9999 }    // 0 - 5 GeV
                                               , {  0.506 ,  0.506 ,  0.320 ,  0.281 ,  0.250 , -9999 ,  0.250 ,  0.281 ,  0.543 , -9999 }    // 5 - 10 GeV
                                               , {  0.587 ,  0.587 ,  0.509 ,  0.420 ,  0.335 , -9999 ,  0.335 ,  0.369 ,  0.616 , -9999 }    // 10 - 15 GeV
                                               , {  0.591 ,  0.591 ,  0.556 ,  0.420 ,  0.335 , -9999 ,  0.335 ,  0.369 ,  0.639 , -9999 }    // 15 - 20 GeV
                                               , {  0.627 ,  0.627 ,  0.617 ,  0.420 ,  0.335 , -9999 ,  0.417 ,  0.505 ,  0.653 , -9999 }    // 20 - 30 GeV
                                               , {  0.627 ,  0.627 ,  0.617 ,  0.420 ,  0.335 , -9999 ,  0.417 ,  0.505 ,  0.653 , -9999 }    // 30 - 40 GeV
                                               , {  0.627 ,  0.627 ,  0.617 ,  0.420 ,  0.335 , -9999 ,  0.417 ,  0.505 ,  0.653 , -9999 }    // 40 - 50 GeV
                                               , {  0.627 ,  0.627 ,  0.617 ,  0.420 ,  0.335 , -9999 ,  0.417 ,  0.505 ,  0.653 , -9999 }    // 50 - 60 GeV
                                               , {  0.627 ,  0.627 ,  0.617 ,  0.420 ,  0.335 , -9999 ,  0.417 ,  0.505 ,  0.653 , -9999 }    // 60 - 70 GeV
                                               , {  0.627 ,  0.627 ,  0.617 ,  0.420 ,  0.335 , -9999 ,  0.417 ,  0.505 ,  0.653 , -9999 }    // 70 - 80 GeV
                                               , {  0.627 ,  0.627 ,  0.617 ,  0.420 ,  0.335 , -9999 ,  0.417 ,  0.505 ,  0.653 , -9999 } }; // 80 -    GeV
  if(DEmaxs1 < cutDEmaxs1[eTBin][etaBin]) return false;
  return  true;
}

//----------------------------------------------------------------------------------------
bool Root::TElectronMultiLeptonSelector::passDeltaEta(double deltaEta, unsigned int eTBin, unsigned int etaBin) const {
  //                                        0.0     0.1      0.6      0.8      1.15      1.37    1.52     1.81    2.01    2.37    2.47
  static const double cutDeltaEta[11][10]  = {{0.017,   0.017,   0.020,   0.020,   0.020,   0.020,   0.020,   0.020,   0.020,   0.020}, // 0 - 5 GeV   
                                              {0.012,   0.009,   0.009,   0.011,   0.013,   0.014,   0.013,   0.011,   0.011,   0.014}, // 5 - 10 GeV  
                                              {0.012,   0.009,   0.008,   0.009,   0.009,   0.013,   0.010,   0.010,   0.008,   0.013}, // 10 - 15 GeV 
                                              {0.011,   0.008,   0.007,   0.007,   0.009,   0.013,   0.009,   0.007,   0.007,   0.011}, // 15 - 20 GeV 
                                              {0.011,   0.008,   0.007,   0.007,   0.009,   0.013,   0.009,   0.007,   0.007,   0.011}, // 20 - 30 GeV 
                                              {0.011,   0.008,   0.007,   0.007,   0.009,   0.013,   0.009,   0.007,   0.007,   0.011}, // 30 - 40 GeV 
                                              {0.011,   0.008,   0.007,   0.007,   0.009,   0.013,   0.009,   0.007,   0.007,   0.011}, // 40 - 50 GeV 
                                              {0.011,   0.008,   0.007,   0.007,   0.009,   0.013,   0.009,   0.007,   0.007,   0.011}, // 50 - 60 GeV 
                                              {0.011,   0.008,   0.007,   0.007,   0.009,   0.013,   0.009,   0.007,   0.007,   0.011}, // 60 - 70 GeV 
                                              {0.011,   0.008,   0.007,   0.007,   0.009,   0.013,   0.009,   0.007,   0.007,   0.011}, // 70 - 80 GeV 
                                              {0.011,   0.008,   0.007,   0.007,   0.009,   0.013,   0.009,   0.007,   0.007,   0.011}};// 80 - inf GeV 
  if (fabs(deltaEta) > cutDeltaEta[eTBin][etaBin]) return false;  
  return true;

}

//----------------------------------------------------------------------------------------
bool Root::TElectronMultiLeptonSelector::passDeltaPhiRes(double deltaPhiRes, bool isBrem, unsigned int eTBin, unsigned int etaBin) const{

  //                                             0.0      0.1  x    0.6      0.8  x   1.15    1.37     1.52   x 1.81    2.01  x 2.37     2.47   
  static const double maxDeltaPhiResHigh[11][10]= {{0.083,   0.083,   0.071,   0.050,   0.043,   0.048,   0.047,   0.038,   0.032,   0.044}, // 0 - 5 GeV   
                                                   {0.038,   0.030,   0.028,   0.023,   0.023,   0.028,   0.023,   0.023,   0.023,   0.038}, // 5 - 10 GeV  
                                                   {0.020,   0.015,   0.013,   0.013,   0.020,   0.023,   0.020,   0.018,   0.023,   0.033}, // 10 - 15 GeV 
                                                   {0.017,   0.015,   0.013,   0.013,   0.018,   0.018,   0.018,   0.018,   0.023,   0.028}, // 15 - 20 GeV 
                                                   {0.017,   0.015,   0.013,   0.013,   0.018,   0.018,   0.018,   0.018,   0.023,   0.028}, // 20 - 30 GeV 
                                                   {0.017,   0.015,   0.013,   0.013,   0.018,   0.018,   0.018,   0.018,   0.023,   0.028}, // 30 - 40 GeV 
                                                   {0.017,   0.015,   0.013,   0.013,   0.018,   0.018,   0.018,   0.018,   0.023,   0.028}, // 40 - 50 GeV 
                                                   {0.017,   0.015,   0.013,   0.013,   0.018,   0.018,   0.018,   0.018,   0.023,   0.028}, // 50 - 60 GeV 
                                                   {0.017,   0.015,   0.013,   0.013,   0.018,   0.018,   0.018,   0.018,   0.023,   0.028}, // 60 - 70 GeV 
                                                   {0.017,   0.015,   0.013,   0.013,   0.018,   0.018,   0.018,   0.018,   0.023,   0.028}, // 70 - 80 GeV 
                                                   {0.017,   0.015,   0.013,   0.013,   0.018,   0.018,   0.018,   0.018,   0.023,   0.028}};// 80 

  //                                             0.0      0.1      0.6       0.8     1.15      1.37     1.52      1.81     2.01    2.37     2.47   
  static const double minDeltaPhiResHigh[11][10]= {{-0.100,  -0.100,  -0.100,  -0.100,  -0.115,  -0.120,  -0.090,  -0.735,  -0.050,  -0.038},// 0 - 5 GeV   
                                                   {-0.086,  -0.078,  -0.078,  -0.078,  -0.083,  -0.053,  -0.048,  -0.028,  -0.023,  -0.023},// 5 - 10 GeV  
                                                   {-0.038,  -0.038,  -0.038,  -0.043,  -0.047,  -0.038,  -0.028,  -0.023,  -0.014,  -0.017},// 10 - 15 GeV 
                                                   {-0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.029,  -0.014,  -0.014,  -0.014},// 15 - 20 GeV 
                                                   {-0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.026,  -0.014,  -0.014,  -0.014},// 20 - 30 GeV 
                                                   {-0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.026,  -0.014,  -0.014,  -0.014},// 30 - 34 GeV 
                                                   {-0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.026,  -0.014,  -0.014,  -0.014},// 34 - 50 GeV 
                                                   {-0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.026,  -0.014,  -0.014,  -0.014},// 50 - 60 GeV 
                                                   {-0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.026,  -0.014,  -0.014,  -0.014},// 60 - 70 GeV 
                                                   {-0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.026,  -0.014,  -0.014,  -0.014},// 70 - 80 GeV 
                                                   {-0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.032,  -0.026,  -0.014,  -0.014,  -0.014}};// 80
  
  //                                            0.0      0.1      0.6       0.8     1.15      1.37     1.52      1.81     2.01    2.37     2.47   
  static const double maxDeltaPhiResLow[11][10] = {{0.100,   0.100,   0.100,   0.075,   0.075,   0.080,   0.075,   0.060,   0.045,   0.070}, // 0 - 5 GeV   
                                                   {0.038,   0.038,   0.038,   0.023,   0.027,   0.038,   0.028,   0.028,   0.028,   0.068}, // 5 - 10 GeV  
                                                   {0.020,   0.017,   0.018,   0.020,   0.020,   0.026,   0.023,   0.023,   0.023,   0.062}, // 10 - 15 GeV 
                                                   {0.017,   0.017,   0.017,   0.018,   0.018,   0.018,   0.020,   0.022,   0.031,   0.053}, // 15 - 20 GeV 
                                                   {0.017,   0.017,   0.017,   0.018,   0.018,   0.018,   0.020,   0.022,   0.031,   0.053}, // 20 - 30 GeV 
                                                   {0.017,   0.017,   0.017,   0.018,   0.018,   0.018,   0.020,   0.022,   0.031,   0.053}, // 30 - 40 GeV 
                                                   {0.017,   0.017,   0.017,   0.018,   0.018,   0.018,   0.020,   0.022,   0.031,   0.053}, // 40 - 50 GeV 
                                                   {0.017,   0.017,   0.017,   0.018,   0.018,   0.018,   0.020,   0.022,   0.031,   0.053}, // 50 - 60 GeV 
                                                   {0.017,   0.017,   0.017,   0.018,   0.018,   0.018,   0.020,   0.022,   0.031,   0.053}, // 60 - 70 GeV 
                                                   {0.017,   0.017,   0.017,   0.018,   0.018,   0.018,   0.020,   0.022,   0.031,   0.053}, // 70 - 80 GeV 
                                                   {0.017,   0.017,   0.017,   0.018,   0.018,   0.018,   0.020,   0.022,   0.031,   0.053}};// 80

  //                                        0.0      0.1      0.6       0.8     1.15     1.37     1.52      1.81     2.01    2.37     2.47   
  static const double minDeltaPhiResLow[11][10]= {{-0.100,  -0.100,  -0.100,  -0.100,  -0.095,  -0.080,  -0.070,  -0.060,  -0.045,  -0.035}, // 0 - 5 GeV   
                                                  {-0.074,  -0.068,  -0.068,  -0.071,  -0.057,  -0.048,  -0.038,  -0.026,  -0.011,  -0.020}, // 5 - 10 GeV  
                                                  {-0.041,  -0.028,  -0.028,  -0.038,  -0.028,  -0.028,  -0.020,  -0.011,  -0.011,  -0.011}, // 10 - 15 GeV 
                                                  {-0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.020,  -0.011,  -0.011,  -0.011}, // 15 - 20 GeV 
                                                  {-0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.020,  -0.011,  -0.011,  -0.011}, // 20 - 25 GeV 
                                                  {-0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.020,  -0.011,  -0.011,  -0.011}, // 25 - 40 GeV 
                                                  {-0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.020,  -0.011,  -0.011,  -0.011}, // 40 - 50 GeV 
                                                  {-0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.020,  -0.011,  -0.011,  -0.011}, // 50 - 60 GeV 
                                                  {-0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.020,  -0.011,  -0.011,  -0.011}, // 60 - 70 GeV 
                                                  {-0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.020,  -0.011,  -0.011,  -0.011}, // 70 - 80 GeV 
                                                  {-0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.023,  -0.020,  -0.011,  -0.011,  -0.011}};// 80   
  
  if(isBrem){
    if(deltaPhiRes < minDeltaPhiResHigh[eTBin][etaBin] || deltaPhiRes > maxDeltaPhiResHigh[eTBin][etaBin]) return false;
  }
  if(!isBrem){
    if(deltaPhiRes < minDeltaPhiResLow[eTBin][etaBin] || deltaPhiRes > maxDeltaPhiResLow[eTBin][etaBin]) return false;
  }
  return true;
  
}

//----------------------------------------------------------------------------------------

bool Root::TElectronMultiLeptonSelector::passTR(double TRratio, double eta, unsigned int  nTRTTotal ) const {  
    if (fabs(eta)>1.37 && fabs(eta)<1.52 ){

        return (nTRTTotal >10 && TRratio > 0.1);
    }
    return true; 
}


bool Root::TElectronMultiLeptonSelector::passTightDeltaPhi(double deltaPhiRes, bool expectBlayer, int nBlayerHits,
                                                        unsigned int eTBin, unsigned int etaBin) const {  

                //                           0.0     0.1      0.6      0.8   1.15      1.37   1.52    1.81    2.01    2.37     2.47 
  static const double maxDeltaPhiResBL[11][10] = {{  0.020 , 0.020,  0.020,  0.020,  0.020,  0.016,  0.020,  0.023,  0.023,  0.032}, // 0  
						  {  0.014,  0.014,  0.014,  0.014,  0.014,  0.015,  0.020,  0.022,  0.022,  0.032}, // 5  
						  {  0.008,  0.008,  0.008,  0.009,  0.009,  0.011,  0.015,  0.015,  0.018,  0.030}, // 10 
						  {  0.006,  0.006,  0.007,  0.008,  0.008,  0.019,  0.013,  0.015,  0.017,  0.025}, // 15 
						  {  0.006,  0.006,  0.006,  0.006,  0.006,  0.008,  0.012,  0.013,  0.015,  0.021}, // 20 
						  {  0.006 , 0.006,  0.006,  0.006,  0.006,  0.007,  0.012,  0.013,  0.014,  0.020}, // 30 
						  {  0.005,  0.005,  0.005,  0.005,  0.005,  0.006,  0.012,  0.013,  0.014,  0.020}, // 40 
						  {  0.005,  0.005,  0.005,  0.005,  0.005,  0.006,  0.012,  0.013,  0.014,  0.020}, // 50 
						  {  0.005,  0.005,  0.005,  0.005,  0.005,  0.006,  0.012,  0.013,  0.014,  0.020}, // 60 
						  {  0.005,  0.005,  0.005,  0.005,  0.005,  0.006,  0.012,  0.013,  0.014,  0.020}, // 70 
						  {  0.005,  0.005,  0.005,  0.005,  0.005,  0.006,  0.012,  0.013,  0.014,  0.020}}; // 80


  if (expectBlayer && nBlayerHits<1) {	
     if(deltaPhiRes > maxDeltaPhiResBL[eTBin][etaBin]){
       return false;
     }
   }
  

  return true;
}

//============================================================================================================================================








