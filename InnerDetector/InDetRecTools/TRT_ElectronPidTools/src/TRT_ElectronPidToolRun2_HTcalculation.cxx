/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidToolRun2_HTcalculation.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifdef INDETTRT_ELECTRONPIDTOOLRUN2_H
#ifndef TRT_ELECTRONPIDRUN2_HTCALCULATION_CXX
#define TRT_ELECTRONPIDRUN2_HTCALCULATION_CXX

/*****************************************************************************\

  This file contains the implementation of the class HTcalculator.

  It is intended to contain all the code that is used for the calculation of
  the likelyhood of a TRT hit being from an electron based on the time over 
  threshold.

  Author: Troels C. Petersen (petersen@nbi.dk)

\*****************************************************************************/

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Default Constructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::HTcalculator::HTcalculator(AthAlgTool & parent):
  BaseTRTPIDCalculator(parent, (SIZE_OF_HEADER+SIZE_OF_BLOB),"HT")
//StrawConstants( (float*)( Blob + OFF_STRAW_CONSTANTS)),
//ParsBarrel( (float*)( Blob + OFF_PAR_SET_BARREL)),
//ParsEndcapA( (float*)( Blob + OFF_PAR_SET_ENDCAP_A)),
//ParsEndcapB( (float*)( Blob + OFF_PAR_SET_ENDCAP_B))
{
  m_datainplace = false;
  CurrentVersion = my_CurrentVersion;
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Default Destructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::HTcalculator::~HTcalculator(){
  //Nothing here yet
}


/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Get The Pobability of this hit being a Hight THreshold hit  %%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

float InDet::TRT_ElectronPidToolRun2::HTcalculator::getProbHT( float pTrk, Trk::ParticleHypothesis hypothesis, int TrtPart, int StrawLayer, float ZRpos, float rTrkWire, float Occupancy, bool UseOccupancy) {
  checkInitialization();

//FIXME: This setup the Troels constants
  setDefaultCalibrationConstants();
  float pHT = 1.0;       // Default/unit value, which ensures that unusable hits does not change probability product!

  // Make sure that the information passed makes sense:
  // --------------------------------------------------
  if (pTrk < 250.0 || pTrk > 7000000.0) return pHT;

  if (TrtPart < 0 || TrtPart > 2) return pHT;

  if ((TrtPart == 0 && (StrawLayer < 0 || StrawLayer > 72)) ||
      (TrtPart == 1 && (StrawLayer < 0 || StrawLayer > 15)) ||
      (TrtPart == 2 && (StrawLayer < 0 || StrawLayer >  8))) return pHT;

  if ((TrtPart == 0 && (ZRpos > 720.0)) ||
      (TrtPart  > 0 && (ZRpos < 640.0 || ZRpos > 1140.0))) return pHT;

  if (rTrkWire < 0.0 || rTrkWire > 2.2) return pHT;


  // Calculate the High Threshold probability, pHT:
  // ----------------------------------------------
  float mass = m_particlemasses.mass[hypothesis];
  //float x = log10( pTrk / mass );

  if (fabs(mass-0.511) < 0.1) {      // Electron!
    float pHTel = pHTvsP(TrtPart, pTrk, mass);
    if (UseOccupancy)
      pHTel = pHTel * Corr_el_SL(TrtPart, StrawLayer) * Corr_el_SP(TrtPart, ZRpos )
	            * Corr_el_RT(TrtPart, rTrkWire)   * Corr_el_OC(TrtPart, Occupancy);
    else
      pHTel = pHTel * Corr_el_SL(TrtPart, StrawLayer) * Corr_el_SP(TrtPart, ZRpos )
                    * Corr_el_RT(TrtPart, rTrkWire);
    return pHTel;
  } else {                           // Non-electron!
    float pHTmu = pHTvsP(TrtPart, pTrk, mass);
    if (UseOccupancy)
      pHTmu = pHTmu * Corr_mu_SL(TrtPart, StrawLayer) * Corr_mu_SP(TrtPart, ZRpos )
	            * Corr_mu_RT(TrtPart, rTrkWire)   * Corr_mu_OC(TrtPart, Occupancy);
    else
      pHTmu = pHTmu * Corr_mu_SL(TrtPart, StrawLayer) * Corr_mu_SP(TrtPart, ZRpos )
	            * Corr_mu_RT(TrtPart, rTrkWire);
    return pHTmu;
  }


  // Troels P (Aug 2014): Old comments, but interesting regarding the splitting into dE/dx and TR parts...
  //
  // Calcualte the probablility based on a logistic sigmoid with constants from the database
  // for retreival of database constants m_gamma_TR_size etc. see the update() function
  // float prob_TR    = par[TR_SIZE] / (1.0 + exp( - ( x - par[TR_MEAN] ) / par[TR_WIDTH] ) );
  // float prob_dEdx  = par[DEDX_C] + x * par[DEDX_M];

  //and apply a correction based on the straw layer
  // const float * corr = StrawConstants + resolveIndex(TrtPart,Layer,Strawlayer);
  // pHT = prob_dEdx * corr[DEDX_CORR] + prob_TR * corr[TR_CORR];// * probfactor_depth * probfactor_angle * probfactor_distwire * probfactor_phi;

  //ATH_MSG_INFO("Determined Eprob from HT to be "<<pHT<<" in part "<<part_i);

}





  // ------------------------------------------------------------------------------------------------------------ //
  // Parametrization of pHT for electrons and muons:
  // ------------------------------------------------------------------------------------------------------------ //

  // ------------------------------------------------------------------------------------------------------------ //
  // Momentum dependence functions:
  // ------------------------------------------------------------------------------------------------------------ //


float InDet::TRT_ElectronPidToolRun2::HTcalculator::pHTvsP(int TrtPart, float p, float mass) {

  float gamma = sqrt(p*p + mass*mass) / mass;

  // Parameters obtained from fitting B,EA,EB on 2012 Zee,Zmm data:
  // Note: par[0] = -1.5  as fixed (low gamma part consistant with being constant!)
//  float par[3][6] = {{-1.5000, 3.8822, 0.0744, 0.1551, 3.2011, 0.2236},
//		      {-1.5000, 3.9137, 0.0702, 0.1654, 3.3043, 0.2093},
//		      {-1.5000, 3.5856, 0.0736, 0.1827, 3.1540, 0.1683}};

  // TR onset curve part (main part):
  float exp_term = exp(-(log10(gamma) - m_par_pHTvsP[TrtPart][4])/m_par_pHTvsP[TrtPart][5]);
  float pHT_TR   = m_par_pHTvsP[TrtPart][2] + m_par_pHTvsP[TrtPart][3]/(1.0 + exp_term);

  // dE/dx m_par_pHTvsPt (low gamma) - IGNORED FOR NOW (as data was consistent with this!):
  float exp_term0 = exp(-(m_par_pHTvsP[TrtPart][0] - m_par_pHTvsP[TrtPart][4])/m_par_pHTvsP[TrtPart][5]);
  float alpha0 = m_par_pHTvsP[TrtPart][2] + m_par_pHTvsP[TrtPart][3]/(1.0 + exp_term0);
  float beta0 = m_par_pHTvsP[TrtPart][3] / ((1.0 + exp_term0)*(1.0 + exp_term0)) * exp_term0 / m_par_pHTvsP[TrtPart][5];
  float pHT_dEdx = alpha0 + beta0*(log10(gamma) - m_par_pHTvsP[TrtPart][0]);

  // High-gamma m_par_pHTvsPt:
  float exp_term1 = exp(-(m_par_pHTvsP[TrtPart][1] - m_par_pHTvsP[TrtPart][4])/m_par_pHTvsP[TrtPart][5]);
  float alpha1 = m_par_pHTvsP[TrtPart][2] + m_par_pHTvsP[TrtPart][3]/(1.0 + exp_term1);
  float beta1 = m_par_pHTvsP[TrtPart][3] / ((1.0 + exp_term1)*(1.0 + exp_term1)) * exp_term1 / m_par_pHTvsP[TrtPart][5];
  float pHT_HG   = alpha1 + beta1*(log10(gamma) - m_par_pHTvsP[TrtPart][1]);

  if (log10(gamma)      < m_par_pHTvsP[TrtPart][0]) return pHT_dEdx;
  else if (log10(gamma) > m_par_pHTvsP[TrtPart][1]) return pHT_HG;
  else                                     return pHT_TR;
}


  // ------------------------------------------------------------------------------------------------------------ //
  // NEW and IMPROVED parametrization of pHT for electrons and muons:
  // ------------------------------------------------------------------------------------------------------------ //

  // -------------------------------------------------------- //
  // Electrons:
  // -------------------------------------------------------- //

  // Straw Layer (SL):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_SL(int part, float SL) {
    int SLbin = int(SL);
    if (part == 0) {
  //    float CpHT_B_Zee_SL[73] = { 0.654, 0.889, 0.957, 1.025, 1.036, 0.988, 1.107, 1.053, 1.058, 0.948, 0.910, 1.006, 0.953, 0.978, 0.991, 0.947, 1.018, 0.987, 1.001, 0.832, 0.949, 0.967, 0.996, 0.996, 1.016, 0.957, 1.049, 1.001, 1.029, 1.017, 0.970, 1.060, 1.017, 1.034, 1.033, 0.979, 1.053, 1.017, 1.029, 1.049, 0.988, 1.051, 1.024, 0.831, 1.009, 0.973, 1.043, 1.024, 0.965, 1.068, 1.016, 1.046, 1.039, 0.987, 1.072, 1.025, 1.049, 1.043, 0.991, 1.073, 1.021, 1.040, 1.045, 0.982, 1.073, 1.046, 1.035, 1.043, 0.977, 1.065, 1.030, 1.024, 1.044}; 
      if (SLbin < 0 || SLbin > 72) printf("  ERROR: SLbin = %5d \n", SLbin); else return m_CpHT_B_Zee_SL[SLbin];
    }
    if (part == 1) {
    //  float CpHT_EA_Zee_SL[16] = { 0.854, 0.926, 0.966, 0.979, 1.011, 1.025, 1.021, 1.023, 1.037, 1.032, 1.033, 1.022, 1.036, 1.046, 1.032, 1.034}; 
      if (SLbin < 0 || SLbin > 15) printf("  ERROR: SLbin = %5d \n", SLbin); else return m_CpHT_EA_Zee_SL[SLbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zee_SL[8] = { 0.833, 0.956, 1.022, 1.046, 1.026, 1.059, 1.065, 1.077}; 
      if (SLbin < 0 || SLbin > 7) printf("  ERROR: SLbin = %5d \n", SLbin); else return m_CpHT_EB_Zee_SL[SLbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // ZR-position (ZR - Z in Barrel, R in Endcaps):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_SP(int part, float SP) {
    if (part == 0) {
      //float CpHT_B_Zee_ZRpos[36] = { 0.877, 0.909, 0.915, 0.922, 0.927, 0.932, 0.933, 0.937, 0.939, 0.945, 0.953, 0.916, 0.929, 0.966, 0.974, 0.991, 0.992, 1.003, 1.013, 1.006, 0.992, 1.007, 1.017, 0.990, 1.039, 1.069, 1.090, 1.107, 1.126, 1.144, 1.156, 1.183, 1.193, 1.210, 1.206, 1.029}; 
      int SPbin = int(SP/720.0*36.0);
      if (SPbin < 0 || SPbin > 35) printf("  ERROR: SPbin = %5d \n", SPbin); else return m_CpHT_B_Zee_ZRpos[SPbin];
    }
    if (part == 1) {
      //float CpHT_EA_Zee_ZRpos[50] = { 0.472, 0.608, 0.688, 0.722, 0.779, 0.807, 0.844, 0.874, 0.882, 0.915, 0.956, 0.973, 0.984, 1.002, 0.998, 1.008, 1.019, 1.030, 1.014, 1.026, 1.039, 1.042, 1.040, 1.049, 1.055, 1.059, 1.069, 1.076, 1.082, 1.078, 1.084, 1.101, 1.105, 1.106, 1.101, 1.098, 1.101, 1.107, 1.104, 1.080, 1.092, 1.081, 1.060, 1.041, 1.021, 0.988, 0.965, 0.939, 0.871, 0.740}; 
      int SPbin = int((SP-640.0)/500.0*50.0);
      if (SPbin < 0 || SPbin > 49) printf("  ERROR: SPbin = %5d \n", SPbin); else return m_CpHT_EA_Zee_ZRpos[SPbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zee_ZRpos[50] = { 0.500, 0.654, 0.863, 0.907, 0.944, 0.972, 0.974, 0.986, 1.005, 1.033, 1.050, 1.059, 1.055, 1.061, 1.050, 1.068, 1.063, 1.066, 1.052, 1.082, 1.057, 1.061, 1.049, 1.055, 1.060, 1.055, 1.046, 1.058, 1.056, 1.067, 1.086, 1.060, 1.056, 1.051, 1.040, 0.993, 0.961, 0.935, 0.919, 0.892, 0.882, 0.846, 0.849, 0.796, 0.784, 0.719, 0.623, 0.471, 0.275, 0.331}; 
      int SPbin = int((SP-640.0)/500.0*50.0);
      if (SPbin < 0 || SPbin > 49) printf("  ERROR: SPbin = %5d \n", SPbin); else return m_CpHT_EB_Zee_ZRpos[SPbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Track-to-Wire distance (TWdist):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_RT(int part, float RT) {
    int RTbin = int(RT/2.2*22.0);
    if (part == 0) {
      //float CpHT_B_Zee_Rtrk[22] = { 1.294, 1.331, 1.341, 1.334, 1.326, 1.317, 1.299, 1.276, 1.242, 1.203, 1.166, 1.121, 1.071, 1.013, 0.944, 0.868, 0.775, 0.674, 0.577, 0.536, 0.622, 0.689}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return m_CpHT_B_Zee_Rtrk[RTbin];
    }
    if (part == 1) {
      //float CpHT_EA_Zee_Rtrk[22] = { 1.315, 1.347, 1.351, 1.358, 1.350, 1.343, 1.322, 1.309, 1.281, 1.250, 1.216, 1.168, 1.118, 1.058, 0.982, 0.887, 0.783, 0.671, 0.557, 0.495, 0.539, 0.718}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return m_CpHT_EA_Zee_Rtrk[RTbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zee_Rtrk[22] = { 1.272, 1.302, 1.312, 1.317, 1.293, 1.305, 1.290, 1.269, 1.256, 1.226, 1.201, 1.165, 1.116, 1.060, 0.997, 0.909, 0.809, 0.722, 0.611, 0.530, 0.498, 0.594}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return m_CpHT_EB_Zee_Rtrk[RTbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Pile-UP (PU):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_PU(int part, float PU) {
    int PUbin = int(PU);
    if (PUbin > 39) PUbin = 39;       // NOTE: Simply put a maximum by hand (to avoid multiple printouts)!!!
    if (part == 0) {
      //float CpHT_B_Zee_PU[40] = { 1.004, 1.004, 1.004, 1.004, 1.004, 1.003, 1.003, 1.003, 1.003, 1.003, 1.003, 1.002, 1.002, 1.002, 1.002, 1.002, 1.002, 1.001, 1.001, 1.001, 1.001, 1.001, 1.001, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 0.999, 0.999, 0.999, 0.999, 0.999, 0.999, 0.999, 0.998, 0.998, 0.998, 0.998}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return m_CpHT_B_Zee_PU[PUbin];
    }
    if (part == 1) {
      //float CpHT_EA_Zee_PU[40] = { 1.064, 1.061, 1.059, 1.056, 1.054, 1.052, 1.049, 1.047, 1.044, 1.042, 1.039, 1.037, 1.034, 1.032, 1.029, 1.027, 1.024, 1.022, 1.019, 1.017, 1.014, 1.012, 1.009, 1.007, 1.004, 1.002, 1.000, 0.997, 0.995, 0.992, 0.990, 0.987, 0.985, 0.982, 0.980, 0.977, 0.975, 0.972, 0.970, 0.967}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return m_CpHT_EA_Zee_PU[PUbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zee_PU[40] = { 1.061, 1.058, 1.056, 1.054, 1.051, 1.049, 1.047, 1.044, 1.042, 1.040, 1.037, 1.035, 1.033, 1.030, 1.028, 1.025, 1.023, 1.021, 1.018, 1.016, 1.014, 1.011, 1.009, 1.007, 1.004, 1.002, 1.000, 0.997, 0.995, 0.993, 0.990, 0.988, 0.985, 0.983, 0.981, 0.978, 0.976, 0.974, 0.971, 0.969}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return m_CpHT_EB_Zee_PU[PUbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Occupancy (OC):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_OC(int part, float OC) {
    int OCbin = int(OC*100.0);
    if (OCbin > 59) OCbin = 59;       // NOTE: Simply put a maximum by hand (to avoid multiple printouts)!!!
    if (part == 0) {
      //float CpHT_B_Zee_Occ[60] = { 0.917, 0.919, 0.922, 0.925, 0.928, 0.930, 0.933, 0.936, 0.939, 0.942, 0.944, 0.947, 0.950, 0.953, 0.956, 0.958, 0.961, 0.964, 0.967, 0.970, 0.972, 0.975, 0.978, 0.981, 0.983, 0.986, 0.989, 0.992, 0.995, 0.997, 1.000, 1.003, 1.006, 1.009, 1.011, 1.014, 1.017, 1.020, 1.023, 1.025, 1.028, 1.031, 1.034, 1.036, 1.039, 1.042, 1.045, 1.048, 1.050, 1.053, 1.056, 1.059, 1.062, 1.064, 1.067, 1.070, 1.073, 1.076, 1.078, 1.081}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return m_CpHT_B_Zee_Occ[OCbin];
    }
    if (part == 1) {
      //float CpHT_EA_Zee_Occ[60] = { 0.980, 0.981, 0.981, 0.982, 0.983, 0.984, 0.984, 0.985, 0.986, 0.986, 0.987, 0.988, 0.989, 0.989, 0.990, 0.991, 0.991, 0.992, 0.993, 0.994, 0.994, 0.995, 0.996, 0.996, 0.997, 0.998, 0.999, 0.999, 1.000, 1.001, 1.001, 1.002, 1.003, 1.004, 1.004, 1.005, 1.006, 1.006, 1.007, 1.008, 1.009, 1.009, 1.010, 1.011, 1.011, 1.012, 1.013, 1.014, 1.014, 1.015, 1.016, 1.016, 1.017, 1.018, 1.019, 1.019, 1.020, 1.021, 1.021, 1.022}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return m_CpHT_EA_Zee_Occ[OCbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zee_Occ[60] = { 0.984, 0.985, 0.985, 0.986, 0.986, 0.987, 0.988, 0.988, 0.989, 0.989, 0.990, 0.990, 0.991, 0.992, 0.992, 0.993, 0.993, 0.994, 0.994, 0.995, 0.995, 0.996, 0.997, 0.997, 0.998, 0.998, 0.999, 0.999, 1.000, 1.001, 1.001, 1.002, 1.002, 1.003, 1.003, 1.004, 1.004, 1.005, 1.006, 1.006, 1.007, 1.007, 1.008, 1.008, 1.009, 1.010, 1.010, 1.011, 1.011, 1.012, 1.012, 1.013, 1.014, 1.014, 1.015, 1.015, 1.016, 1.016, 1.017, 1.017}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return m_CpHT_EB_Zee_Occ[OCbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }





  // -------------------------------------------------------- //
  // Muons:
  // -------------------------------------------------------- //

  // Straw Layer (SL):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_SL(int part, float SL) {
    int SLbin = int(SL);
    if (part == 0) {
    //  float CpHT_B_Zmm_SL[73] = { 1.068, 1.130, 1.150, 1.180, 1.129, 1.116, 1.146, 1.102, 1.073, 1.102, 1.083, 1.095, 1.071, 1.058, 1.075, 1.071, 1.068, 1.026, 1.026, 1.020, 1.030, 1.051, 1.067, 1.050, 1.049, 1.037, 1.052, 1.028, 1.027, 1.028, 1.007, 1.043, 1.024, 1.013, 1.005, 0.994, 1.004, 0.986, 0.982, 0.995, 0.978, 0.972, 0.954, 0.940, 0.976, 0.984, 0.997, 0.987, 0.957, 0.982, 0.984, 0.975, 0.981, 0.960, 0.981, 0.952, 0.965, 0.951, 0.941, 0.952, 0.928, 0.936, 0.936, 0.917, 0.933, 0.932, 0.896, 0.900, 0.892, 0.898, 0.885, 0.880, 0.874}; 
      if (SLbin < 0 || SLbin > 72) printf("  ERROR: SLbin = %5d \n", SLbin); else return m_CpHT_B_Zmm_SL[SLbin];
    }
    if (part == 1) {
      //float CpHT_EA_Zmm_SL[16] = { 0.966, 0.969, 0.979, 0.969, 0.986, 1.005, 0.997, 1.023, 1.020, 1.002, 1.018, 1.002, 1.013, 1.054, 1.013, 1.013}; 
      if (SLbin < 0 || SLbin > 15) printf("  ERROR: SLbin = %5d \n", SLbin); else return m_CpHT_EA_Zmm_SL[SLbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zmm_SL[8] = { 0.926, 0.995, 1.026, 1.028, 0.997, 1.005, 1.013, 1.038}; 
      if (SLbin < 0 || SLbin > 7) printf("  ERROR: SLbin = %5d \n", SLbin); else return m_CpHT_EB_Zmm_SL[SLbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // ZR-position (ZR - Z in Barrel, R in Endcaps):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_SP(int part, float SP) {
    if (part == 0) {
      //float CpHT_B_Zmm_ZRpos[36] = { 0.884, 0.902, 0.906, 0.911, 0.902, 0.908, 0.922, 0.912, 0.914, 0.922, 0.922, 0.921, 0.933, 0.952, 0.957, 0.959, 0.971, 0.980, 0.992, 0.994, 1.003, 1.014, 1.017, 1.024, 1.055, 1.064, 1.086, 1.116, 1.134, 1.156, 1.186, 1.197, 1.217, 1.251, 1.249, 1.042}; 
      int SPbin = int(SP/720.0*36.0);
      if (SPbin < 0 || SPbin > 35) printf("  ERROR: SPbin = %5d \n", SPbin); else return m_CpHT_B_Zmm_ZRpos[SPbin];
    }
    if (part == 1) {
      //float CpHT_EA_Zmm_ZRpos[50] = { 0.680, 0.756, 0.809, 0.808, 0.864, 0.872, 0.888, 0.907, 0.923, 0.938, 0.932, 0.960, 0.980, 0.972, 0.984, 0.979, 0.986, 0.980, 0.998, 0.999, 0.996, 1.014, 1.009, 1.005, 1.025, 1.020, 1.030, 1.020, 1.048, 1.049, 1.050, 1.045, 1.044, 1.049, 1.059, 1.064, 1.047, 1.065, 1.076, 1.069, 1.073, 1.076, 1.066, 1.080, 1.080, 1.070, 1.066, 1.038, 1.088, 1.012}; 
      int SPbin = int((SP-640.0)/500.0*50.0);
      if (SPbin < 0 || SPbin > 49) printf("  ERROR: SPbin = %5d \n", SPbin); else return m_CpHT_EA_Zmm_ZRpos[SPbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zmm_ZRpos[50] = { 0.500, 0.673, 1.117, 0.988, 0.813, 1.029, 0.845, 0.912, 0.963, 0.867, 1.083, 0.955, 0.953, 1.051, 1.089, 0.963, 0.976, 0.908, 1.090, 1.033, 1.034, 1.074, 1.004, 1.004, 1.055, 1.062, 1.003, 1.003, 1.028, 1.043, 1.014, 1.000, 1.012, 1.018, 1.037, 1.064, 1.024, 0.990, 0.971, 0.981, 1.022, 0.974, 0.936, 0.916, 0.984, 0.900, 0.933, 0.858, 0.687, 0.907}; 
      int SPbin = int((SP-640.0)/500.0*50.0);
      if (SPbin < 0 || SPbin > 49) printf("  ERROR: SPbin = %5d \n", SPbin); else return m_CpHT_EB_Zmm_ZRpos[SPbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Track-to-Wire distance (TWdist):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_RT(int part, float RT) {
    int RTbin = int(RT/2.2*22.0);
    if (part == 0) {
      //float CpHT_B_Zmm_Rtrk[22] = { 1.143, 1.133, 1.143, 1.152, 1.157, 1.151, 1.138, 1.139, 1.119, 1.094, 1.067, 1.034, 1.002, 0.953, 0.899, 0.841, 0.787, 0.734, 0.727, 0.882, 1.336, 1.434}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return m_CpHT_B_Zmm_Rtrk[RTbin];
    }
    if (part == 1) {
      //float CpHT_EA_Zmm_Rtrk[22] = { 1.236, 1.235, 1.260, 1.274, 1.266, 1.270, 1.260, 1.253, 1.225, 1.201, 1.154, 1.113, 1.045, 0.993, 0.905, 0.809, 0.716, 0.626, 0.582, 0.666, 1.048, 1.083}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return m_CpHT_EA_Zmm_Rtrk[RTbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zmm_Rtrk[22] = { 1.159, 1.208, 1.297, 1.188, 1.248, 1.228, 1.181, 1.205, 1.201, 1.149, 1.128, 1.089, 1.038, 0.942, 0.884, 0.843, 0.781, 0.666, 0.653, 0.672, 0.979, 0.888}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return m_CpHT_EB_Zmm_Rtrk[RTbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Pile-UP (PU):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_PU(int part, float PU) {
    int PUbin = int(PU);
    if (PUbin > 39) PUbin = 39;       // NOTE: Simply put a maximum by hand (to avoid multiple printouts)!!!
    if (part == 0) {
      //float CpHT_B_Zmm_PU[40] = { 0.761, 0.773, 0.786, 0.798, 0.810, 0.822, 0.834, 0.846, 0.859, 0.871, 0.883, 0.895, 0.907, 0.919, 0.932, 0.944, 0.956, 0.968, 0.980, 0.992, 1.005, 1.017, 1.029, 1.041, 1.053, 1.065, 1.078, 1.090, 1.102, 1.114, 1.126, 1.138, 1.151, 1.163, 1.175, 1.187, 1.199, 1.211, 1.224, 1.236}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return m_CpHT_B_Zmm_PU[PUbin];
    }
    if (part == 1) {
      //float CpHT_EA_Zmm_PU[40] = { 0.894, 0.899, 0.905, 0.910, 0.915, 0.921, 0.926, 0.931, 0.937, 0.942, 0.947, 0.953, 0.958, 0.963, 0.969, 0.974, 0.979, 0.985, 0.990, 0.995, 1.001, 1.006, 1.011, 1.016, 1.022, 1.027, 1.032, 1.038, 1.043, 1.048, 1.054, 1.059, 1.064, 1.070, 1.075, 1.080, 1.086, 1.091, 1.096, 1.102}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return m_CpHT_EA_Zmm_PU[PUbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zmm_PU[40] = { 0.828, 0.837, 0.845, 0.854, 0.862, 0.871, 0.880, 0.888, 0.897, 0.905, 0.914, 0.922, 0.931, 0.940, 0.948, 0.957, 0.965, 0.974, 0.982, 0.991, 0.999, 1.008, 1.017, 1.025, 1.034, 1.042, 1.051, 1.059, 1.068, 1.077, 1.085, 1.094, 1.102, 1.111, 1.119, 1.128, 1.136, 1.145, 1.154, 1.162}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return m_CpHT_EB_Zmm_PU[PUbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Occupancy (OC):
  float InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_OC(int part, float OC) {
    int OCbin = int(OC*100.0);
    if (OCbin > 59) OCbin = 59;       // NOTE: Simply put a maximum by hand (to avoid multiple printouts)!!!
    if (part == 0) {
      //float CpHT_B_Zmm_Occ[60] = { 0.633, 0.648, 0.663, 0.678, 0.693, 0.709, 0.724, 0.739, 0.754, 0.769, 0.785, 0.800, 0.815, 0.830, 0.845, 0.860, 0.876, 0.891, 0.906, 0.921, 0.936, 0.952, 0.967, 0.982, 0.997, 1.012, 1.028, 1.043, 1.058, 1.073, 1.088, 1.103, 1.119, 1.134, 1.149, 1.164, 1.179, 1.195, 1.210, 1.225, 1.240, 1.255, 1.270, 1.286, 1.301, 1.316, 1.331, 1.346, 1.362, 1.377, 1.392, 1.407, 1.422, 1.437, 1.453, 1.468, 1.483, 1.498, 1.513, 1.529}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return m_CpHT_B_Zmm_Occ[OCbin];
    }
    if (part == 1) {
      //float CpHT_EA_Zmm_Occ[60] = { 0.807, 0.815, 0.824, 0.832, 0.841, 0.849, 0.858, 0.866, 0.875, 0.883, 0.892, 0.900, 0.909, 0.917, 0.926, 0.934, 0.943, 0.951, 0.960, 0.968, 0.977, 0.985, 0.994, 1.002, 1.011, 1.019, 1.028, 1.036, 1.045, 1.053, 1.062, 1.070, 1.079, 1.087, 1.096, 1.104, 1.113, 1.121, 1.130, 1.138, 1.147, 1.155, 1.164, 1.172, 1.181, 1.189, 1.198, 1.206, 1.215, 1.223, 1.232, 1.240, 1.249, 1.257, 1.266, 1.274, 1.283, 1.291, 1.300, 1.308}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return m_CpHT_EA_Zmm_Occ[OCbin];
    }
    if (part == 2) {
      //float CpHT_EB_Zmm_Occ[60] = { 0.733, 0.744, 0.756, 0.767, 0.779, 0.791, 0.802, 0.814, 0.825, 0.837, 0.849, 0.860, 0.872, 0.883, 0.895, 0.906, 0.918, 0.930, 0.941, 0.953, 0.964, 0.976, 0.987, 0.999, 1.011, 1.022, 1.034, 1.045, 1.057, 1.069, 1.080, 1.092, 1.103, 1.115, 1.126, 1.138, 1.150, 1.161, 1.173, 1.184, 1.196, 1.207, 1.219, 1.231, 1.242, 1.254, 1.265, 1.277, 1.289, 1.300, 1.312, 1.323, 1.335, 1.346, 1.358, 1.370, 1.381, 1.393, 1.404, 1.416}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return m_CpHT_EB_Zmm_Occ[OCbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }









/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  turn the BEC, strawlayer and layer information into a linear index  %%*|
|*%%%  that can be used on a flat array.  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

/*
int InDet::TRT_ElectronPidToolRun2::HTcalculator::resolveIndex( int BEC,  int Layer, int Strawlayer){

  int part=abs(BEC)-1;

  
  // Do a consistency check:
  static const int strawsPerBEC[2][14]={{19,24,30, 0, 0, 0,0,0,0,0,0,0,0,0},
                                        {16,16,16,16,16,16,8,8,8,8,8,8,8,8}};

  //if(Strawlayer >= strawsPerBEC[part][Layer])
  //  std::cout<<"TRT_ElectronPidToolRun2   ERROR TRT part "<<BEC<<" Layer "<<Layer<<" only has "<<strawsPerBEC[part][Layer]<<" straws. Found index "<<Strawlayer<<std::endl;

  // the following array will contain the number that has to be added for each Layer. It is derived as the integral of the array above.
  // the initialization looks unnecessarily complicated but these computations are carried out at compile time, so no worries ;)
  static const int strawsBeforeLayer[2][14]={{0, strawsPerBEC[0][0]+strawsBeforeLayer[0][0], strawsPerBEC[0][1]+strawsBeforeLayer[0][1],
                                              strawsPerBEC[0][2]+strawsBeforeLayer[0][2], 0,  0,  0,   0,   0,   0,   0,   0,   0,   0},
                                             {strawsBeforeLayer[0][3], //last from Barrel
                                              strawsPerBEC[1][0]+strawsBeforeLayer[1][0],
                                              strawsPerBEC[1][1]+strawsBeforeLayer[1][1], strawsPerBEC[1][2]+strawsBeforeLayer[1][2],
                                              strawsPerBEC[1][3]+strawsBeforeLayer[1][3], strawsPerBEC[1][4]+strawsBeforeLayer[1][4],
                                              strawsPerBEC[1][5]+strawsBeforeLayer[1][5], strawsPerBEC[1][6]+strawsBeforeLayer[1][6],
                                              strawsPerBEC[1][7]+strawsBeforeLayer[1][7], strawsPerBEC[1][8]+strawsBeforeLayer[1][8],
                                              strawsPerBEC[1][9]+strawsBeforeLayer[1][9], strawsPerBEC[1][10]+strawsBeforeLayer[1][10],
                                              strawsPerBEC[1][11]+strawsBeforeLayer[1][11], strawsPerBEC[1][12]+strawsBeforeLayer[1][12]}};
  
  // std::cout<<"TRT_ElectronPidToolRun2::HTcalculator      INFO now returning index "<<Strawlayer + strawsBeforeLayer[part][Layer]<<" Part: "<<part<<" Layer: "<<Layer<<" SL: " <<Strawlayer<<std::endl;
  int glob_straw_index = Strawlayer + strawsBeforeLayer[part][Layer];
  int offset = glob_straw_index * N_CONSTS_PER_STRAW;
  if(not (offset < N_STRAW_CONSTS))
    return 0;
  return offset;
}
*/

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Hard-coded HT Calibration Constants  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

void InDet::TRT_ElectronPidToolRun2::HTcalculator::setDefaultCalibrationConstants(){
  /*****************************************************************************\

    This code is never called in production. It is used to set all
    constants in the positions in the HTBlob where they are needed, and 
    finally print out the blob as an array of numbers. This is far easier and 
    less error prone than having a separate setter-script which might itself 
    have a version mismatch with this code.

    PLEASE REMEMBER to increment the version number precisely when you change 
    the addresses of the various arrays inside the HTBlob, and NEVER otherwise!
    
  \*****************************************************************************/
	//FIXME
  if (m_datainplace) return;  // Just to load 1 time
  parent.msg(MSG::INFO) << "Set hard-coded PID calibration constants. Derived from Run1 Data Zee and Zmumu 50 ns. "<<endmsg;

  HasBeenInitialized=1;

  Blob[_Version]=CurrentVersion;
    {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    Blob[_Day]=timeinfo->tm_mday;
    Blob[_Month]=timeinfo->tm_mon+1;
    Blob[_Year]=(timeinfo->tm_year)%100;
  }

  UpperLimit=1.0;
  LowerLimit=0.0;

  //For data we found the following:
//  m_par_pHTvsP= new float*[3];
//  for (int i =0; i<3; ++i){
//    m_par_pHTvsP[i] = new float[6];
//  }

  float par[N_DET][N_PAR] = {{-1.5000, 3.8822, 0.0744, 0.1551, 3.2011, 0.2236},
		      {-1.5000, 3.9137, 0.0702, 0.1654, 3.3043, 0.2093},
		      {-1.5000, 3.5856, 0.0736, 0.1827, 3.1540, 0.1683}};


  for (int i = 0 ; i < N_DET; i++)
	for (int j = 0; j < N_PAR ; j++)
		m_par_pHTvsP[i][j] = par[i][j];


      float CpHT_B_Zee_SL[N_SL_B] = { 0.654, 0.889, 0.957, 1.025, 1.036, 0.988, 1.107, 1.053, 1.058, 0.948, 0.910, 1.006, 0.953, 0.978, 0.991, 0.947, 1.018, 0.987, 1.001, 0.832, 0.949, 0.967, 0.996, 0.996, 1.016, 0.957, 1.049, 1.001, 1.029, 1.017, 0.970, 1.060, 1.017, 1.034, 1.033, 0.979, 1.053, 1.017, 1.029, 1.049, 0.988, 1.051, 1.024, 0.831, 1.009, 0.973, 1.043, 1.024, 0.965, 1.068, 1.016, 1.046, 1.039, 0.987, 1.072, 1.025, 1.049, 1.043, 0.991, 1.073, 1.021, 1.040, 1.045, 0.982, 1.073, 1.046, 1.035, 1.043, 0.977, 1.065, 1.030, 1.024, 1.044}; 
      float CpHT_EA_Zee_SL[N_SL_EA] = { 0.854, 0.926, 0.966, 0.979, 1.011, 1.025, 1.021, 1.023, 1.037, 1.032, 1.033, 1.022, 1.036, 1.046, 1.032, 1.034}; 
      float CpHT_EB_Zee_SL[N_SL_EB] = { 0.833, 0.956, 1.022, 1.046, 1.026, 1.059, 1.065, 1.077}; 

  // ZR-position (ZR - Z in Barrel, R in Endcaps):
      float CpHT_B_Zee_ZRpos[N_ZRpos_B] = { 0.877, 0.909, 0.915, 0.922, 0.927, 0.932, 0.933, 0.937, 0.939, 0.945, 0.953, 0.916, 0.929, 0.966, 0.974, 0.991, 0.992, 1.003, 1.013, 1.006, 0.992, 1.007, 1.017, 0.990, 1.039, 1.069, 1.090, 1.107, 1.126, 1.144, 1.156, 1.183, 1.193, 1.210, 1.206, 1.029}; 
      float CpHT_EA_Zee_ZRpos[N_ZRpos_EA] = { 0.472, 0.608, 0.688, 0.722, 0.779, 0.807, 0.844, 0.874, 0.882, 0.915, 0.956, 0.973, 0.984, 1.002, 0.998, 1.008, 1.019, 1.030, 1.014, 1.026, 1.039, 1.042, 1.040, 1.049, 1.055, 1.059, 1.069, 1.076, 1.082, 1.078, 1.084, 1.101, 1.105, 1.106, 1.101, 1.098, 1.101, 1.107, 1.104, 1.080, 1.092, 1.081, 1.060, 1.041, 1.021, 0.988, 0.965, 0.939, 0.871, 0.740}; 
      float CpHT_EB_Zee_ZRpos[N_ZRpos_EB] = { 0.500, 0.654, 0.863, 0.907, 0.944, 0.972, 0.974, 0.986, 1.005, 1.033, 1.050, 1.059, 1.055, 1.061, 1.050, 1.068, 1.063, 1.066, 1.052, 1.082, 1.057, 1.061, 1.049, 1.055, 1.060, 1.055, 1.046, 1.058, 1.056, 1.067, 1.086, 1.060, 1.056, 1.051, 1.040, 0.993, 0.961, 0.935, 0.919, 0.892, 0.882, 0.846, 0.849, 0.796, 0.784, 0.719, 0.623, 0.471, 0.275, 0.331}; 

  // Track-to-Wire distance (TWdist):
      float CpHT_B_Zee_Rtrk[N_Rtrk_B] = { 1.294, 1.331, 1.341, 1.334, 1.326, 1.317, 1.299, 1.276, 1.242, 1.203, 1.166, 1.121, 1.071, 1.013, 0.944, 0.868, 0.775, 0.674, 0.577, 0.536, 0.622, 0.689}; 
      float CpHT_EA_Zee_Rtrk[N_Rtrk_EA] = { 1.315, 1.347, 1.351, 1.358, 1.350, 1.343, 1.322, 1.309, 1.281, 1.250, 1.216, 1.168, 1.118, 1.058, 0.982, 0.887, 0.783, 0.671, 0.557, 0.495, 0.539, 0.718}; 
      float CpHT_EB_Zee_Rtrk[N_Rtrk_EB] = { 1.272, 1.302, 1.312, 1.317, 1.293, 1.305, 1.290, 1.269, 1.256, 1.226, 1.201, 1.165, 1.116, 1.060, 0.997, 0.909, 0.809, 0.722, 0.611, 0.530, 0.498, 0.594}; 


  // Pile-UP (PU):
      float CpHT_B_Zee_PU[N_PU_B] = { 1.004, 1.004, 1.004, 1.004, 1.004, 1.003, 1.003, 1.003, 1.003, 1.003, 1.003, 1.002, 1.002, 1.002, 1.002, 1.002, 1.002, 1.001, 1.001, 1.001, 1.001, 1.001, 1.001, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 0.999, 0.999, 0.999, 0.999, 0.999, 0.999, 0.999, 0.998, 0.998, 0.998, 0.998}; 
      float CpHT_EA_Zee_PU[N_PU_EA] = { 1.064, 1.061, 1.059, 1.056, 1.054, 1.052, 1.049, 1.047, 1.044, 1.042, 1.039, 1.037, 1.034, 1.032, 1.029, 1.027, 1.024, 1.022, 1.019, 1.017, 1.014, 1.012, 1.009, 1.007, 1.004, 1.002, 1.000, 0.997, 0.995, 0.992, 0.990, 0.987, 0.985, 0.982, 0.980, 0.977, 0.975, 0.972, 0.970, 0.967}; 
      float CpHT_EB_Zee_PU[N_PU_EB] = { 1.061, 1.058, 1.056, 1.054, 1.051, 1.049, 1.047, 1.044, 1.042, 1.040, 1.037, 1.035, 1.033, 1.030, 1.028, 1.025, 1.023, 1.021, 1.018, 1.016, 1.014, 1.011, 1.009, 1.007, 1.004, 1.002, 1.000, 0.997, 0.995, 0.993, 0.990, 0.988, 0.985, 0.983, 0.981, 0.978, 0.976, 0.974, 0.971, 0.969}; 


  // Occupancy (OC):
      float CpHT_B_Zee_Occ[N_Occ_B] = { 0.917, 0.919, 0.922, 0.925, 0.928, 0.930, 0.933, 0.936, 0.939, 0.942, 0.944, 0.947, 0.950, 0.953, 0.956, 0.958, 0.961, 0.964, 0.967, 0.970, 0.972, 0.975, 0.978, 0.981, 0.983, 0.986, 0.989, 0.992, 0.995, 0.997, 1.000, 1.003, 1.006, 1.009, 1.011, 1.014, 1.017, 1.020, 1.023, 1.025, 1.028, 1.031, 1.034, 1.036, 1.039, 1.042, 1.045, 1.048, 1.050, 1.053, 1.056, 1.059, 1.062, 1.064, 1.067, 1.070, 1.073, 1.076, 1.078, 1.081}; 
      float CpHT_EA_Zee_Occ[N_Occ_EA] = { 0.980, 0.981, 0.981, 0.982, 0.983, 0.984, 0.984, 0.985, 0.986, 0.986, 0.987, 0.988, 0.989, 0.989, 0.990, 0.991, 0.991, 0.992, 0.993, 0.994, 0.994, 0.995, 0.996, 0.996, 0.997, 0.998, 0.999, 0.999, 1.000, 1.001, 1.001, 1.002, 1.003, 1.004, 1.004, 1.005, 1.006, 1.006, 1.007, 1.008, 1.009, 1.009, 1.010, 1.011, 1.011, 1.012, 1.013, 1.014, 1.014, 1.015, 1.016, 1.016, 1.017, 1.018, 1.019, 1.019, 1.020, 1.021, 1.021, 1.022}; 
      float CpHT_EB_Zee_Occ[N_Occ_EB] = { 0.984, 0.985, 0.985, 0.986, 0.986, 0.987, 0.988, 0.988, 0.989, 0.989, 0.990, 0.990, 0.991, 0.992, 0.992, 0.993, 0.993, 0.994, 0.994, 0.995, 0.995, 0.996, 0.997, 0.997, 0.998, 0.998, 0.999, 0.999, 1.000, 1.001, 1.001, 1.002, 1.002, 1.003, 1.003, 1.004, 1.004, 1.005, 1.006, 1.006, 1.007, 1.007, 1.008, 1.008, 1.009, 1.010, 1.010, 1.011, 1.011, 1.012, 1.012, 1.013, 1.014, 1.014, 1.015, 1.015, 1.016, 1.016, 1.017, 1.017}; 





      float CpHT_B_Zmm_SL[N_SL_B] = { 1.068, 1.130, 1.150, 1.180, 1.129, 1.116, 1.146, 1.102, 1.073, 1.102, 1.083, 1.095, 1.071, 1.058, 1.075, 1.071, 1.068, 1.026, 1.026, 1.020, 1.030, 1.051, 1.067, 1.050, 1.049, 1.037, 1.052, 1.028, 1.027, 1.028, 1.007, 1.043, 1.024, 1.013, 1.005, 0.994, 1.004, 0.986, 0.982, 0.995, 0.978, 0.972, 0.954, 0.940, 0.976, 0.984, 0.997, 0.987, 0.957, 0.982, 0.984, 0.975, 0.981, 0.960, 0.981, 0.952, 0.965, 0.951, 0.941, 0.952, 0.928, 0.936, 0.936, 0.917, 0.933, 0.932, 0.896, 0.900, 0.892, 0.898, 0.885, 0.880, 0.874}; 
      float CpHT_EA_Zmm_SL[N_SL_EA] = { 0.966, 0.969, 0.979, 0.969, 0.986, 1.005, 0.997, 1.023, 1.020, 1.002, 1.018, 1.002, 1.013, 1.054, 1.013, 1.013}; 
      float CpHT_EB_Zmm_SL[N_SL_EB] = { 0.926, 0.995, 1.026, 1.028, 0.997, 1.005, 1.013, 1.038}; 


  // ZR-position (ZR - Z in Barrel, R in Endcaps):
      float CpHT_B_Zmm_ZRpos[N_ZRpos_B] = { 0.884, 0.902, 0.906, 0.911, 0.902, 0.908, 0.922, 0.912, 0.914, 0.922, 0.922, 0.921, 0.933, 0.952, 0.957, 0.959, 0.971, 0.980, 0.992, 0.994, 1.003, 1.014, 1.017, 1.024, 1.055, 1.064, 1.086, 1.116, 1.134, 1.156, 1.186, 1.197, 1.217, 1.251, 1.249, 1.042}; 
      float CpHT_EA_Zmm_ZRpos[N_ZRpos_EA] = { 0.680, 0.756, 0.809, 0.808, 0.864, 0.872, 0.888, 0.907, 0.923, 0.938, 0.932, 0.960, 0.980, 0.972, 0.984, 0.979, 0.986, 0.980, 0.998, 0.999, 0.996, 1.014, 1.009, 1.005, 1.025, 1.020, 1.030, 1.020, 1.048, 1.049, 1.050, 1.045, 1.044, 1.049, 1.059, 1.064, 1.047, 1.065, 1.076, 1.069, 1.073, 1.076, 1.066, 1.080, 1.080, 1.070, 1.066, 1.038, 1.088, 1.012}; 
      float CpHT_EB_Zmm_ZRpos[N_ZRpos_EB] = { 0.500, 0.673, 1.117, 0.988, 0.813, 1.029, 0.845, 0.912, 0.963, 0.867, 1.083, 0.955, 0.953, 1.051, 1.089, 0.963, 0.976, 0.908, 1.090, 1.033, 1.034, 1.074, 1.004, 1.004, 1.055, 1.062, 1.003, 1.003, 1.028, 1.043, 1.014, 1.000, 1.012, 1.018, 1.037, 1.064, 1.024, 0.990, 0.971, 0.981, 1.022, 0.974, 0.936, 0.916, 0.984, 0.900, 0.933, 0.858, 0.687, 0.907}; 


  // Track-to-Wire distance (TWdist):
      float CpHT_B_Zmm_Rtrk[N_Rtrk_B] = { 1.143, 1.133, 1.143, 1.152, 1.157, 1.151, 1.138, 1.139, 1.119, 1.094, 1.067, 1.034, 1.002, 0.953, 0.899, 0.841, 0.787, 0.734, 0.727, 0.882, 1.336, 1.434}; 
      float CpHT_EA_Zmm_Rtrk[N_Rtrk_EA] = { 1.236, 1.235, 1.260, 1.274, 1.266, 1.270, 1.260, 1.253, 1.225, 1.201, 1.154, 1.113, 1.045, 0.993, 0.905, 0.809, 0.716, 0.626, 0.582, 0.666, 1.048, 1.083}; 
      float CpHT_EB_Zmm_Rtrk[N_Rtrk_EB] = { 1.159, 1.208, 1.297, 1.188, 1.248, 1.228, 1.181, 1.205, 1.201, 1.149, 1.128, 1.089, 1.038, 0.942, 0.884, 0.843, 0.781, 0.666, 0.653, 0.672, 0.979, 0.888}; 


  // Pile-UP (PU):
      float CpHT_B_Zmm_PU[N_PU_B] = { 0.761, 0.773, 0.786, 0.798, 0.810, 0.822, 0.834, 0.846, 0.859, 0.871, 0.883, 0.895, 0.907, 0.919, 0.932, 0.944, 0.956, 0.968, 0.980, 0.992, 1.005, 1.017, 1.029, 1.041, 1.053, 1.065, 1.078, 1.090, 1.102, 1.114, 1.126, 1.138, 1.151, 1.163, 1.175, 1.187, 1.199, 1.211, 1.224, 1.236}; 
      float CpHT_EA_Zmm_PU[N_PU_EA] = { 0.894, 0.899, 0.905, 0.910, 0.915, 0.921, 0.926, 0.931, 0.937, 0.942, 0.947, 0.953, 0.958, 0.963, 0.969, 0.974, 0.979, 0.985, 0.990, 0.995, 1.001, 1.006, 1.011, 1.016, 1.022, 1.027, 1.032, 1.038, 1.043, 1.048, 1.054, 1.059, 1.064, 1.070, 1.075, 1.080, 1.086, 1.091, 1.096, 1.102}; 
      float CpHT_EB_Zmm_PU[N_PU_EB] = { 0.828, 0.837, 0.845, 0.854, 0.862, 0.871, 0.880, 0.888, 0.897, 0.905, 0.914, 0.922, 0.931, 0.940, 0.948, 0.957, 0.965, 0.974, 0.982, 0.991, 0.999, 1.008, 1.017, 1.025, 1.034, 1.042, 1.051, 1.059, 1.068, 1.077, 1.085, 1.094, 1.102, 1.111, 1.119, 1.128, 1.136, 1.145, 1.154, 1.162}; 


  // Occupancy (OC):
      float CpHT_B_Zmm_Occ[N_Occ_B] = { 0.633, 0.648, 0.663, 0.678, 0.693, 0.709, 0.724, 0.739, 0.754, 0.769, 0.785, 0.800, 0.815, 0.830, 0.845, 0.860, 0.876, 0.891, 0.906, 0.921, 0.936, 0.952, 0.967, 0.982, 0.997, 1.012, 1.028, 1.043, 1.058, 1.073, 1.088, 1.103, 1.119, 1.134, 1.149, 1.164, 1.179, 1.195, 1.210, 1.225, 1.240, 1.255, 1.270, 1.286, 1.301, 1.316, 1.331, 1.346, 1.362, 1.377, 1.392, 1.407, 1.422, 1.437, 1.453, 1.468, 1.483, 1.498, 1.513, 1.529}; 
      float CpHT_EA_Zmm_Occ[N_Occ_EA] = { 0.807, 0.815, 0.824, 0.832, 0.841, 0.849, 0.858, 0.866, 0.875, 0.883, 0.892, 0.900, 0.909, 0.917, 0.926, 0.934, 0.943, 0.951, 0.960, 0.968, 0.977, 0.985, 0.994, 1.002, 1.011, 1.019, 1.028, 1.036, 1.045, 1.053, 1.062, 1.070, 1.079, 1.087, 1.096, 1.104, 1.113, 1.121, 1.130, 1.138, 1.147, 1.155, 1.164, 1.172, 1.181, 1.189, 1.198, 1.206, 1.215, 1.223, 1.232, 1.240, 1.249, 1.257, 1.266, 1.274, 1.283, 1.291, 1.300, 1.308}; 
      float CpHT_EB_Zmm_Occ[N_Occ_EB] = { 0.733, 0.744, 0.756, 0.767, 0.779, 0.791, 0.802, 0.814, 0.825, 0.837, 0.849, 0.860, 0.872, 0.883, 0.895, 0.906, 0.918, 0.930, 0.941, 0.953, 0.964, 0.976, 0.987, 0.999, 1.011, 1.022, 1.034, 1.045, 1.057, 1.069, 1.080, 1.092, 1.103, 1.115, 1.126, 1.138, 1.150, 1.161, 1.173, 1.184, 1.196, 1.207, 1.219, 1.231, 1.242, 1.254, 1.265, 1.277, 1.289, 1.300, 1.312, 1.323, 1.335, 1.346, 1.358, 1.370, 1.381, 1.393, 1.404, 1.416}; 



  for (int i = 0; i < N_SL_B; i++){
  	m_CpHT_B_Zee_SL[i]	=   CpHT_B_Zee_SL[i]    ;
  	m_CpHT_B_Zmm_SL[i]     	=   CpHT_B_Zmm_SL[i]    ;
  }

  for (int i = 0; i < N_SL_EA; i++){
  	m_CpHT_EA_Zee_SL[i]	=   CpHT_EA_Zee_SL[i]    ;
  	m_CpHT_EA_Zmm_SL[i]     =   CpHT_EA_Zmm_SL[i]    ;
  }

  for (int i = 0; i < N_SL_EB; i++){
  	m_CpHT_EB_Zee_SL[i]	=   CpHT_EB_Zee_SL[i]    ;
  	m_CpHT_EB_Zmm_SL[i]     =   CpHT_EB_Zmm_SL[i]    ;
  }

  for (int i = 0; i < N_ZRpos_B; i++){
  	m_CpHT_B_Zee_ZRpos[i]	=   CpHT_B_Zee_ZRpos[i]    ;
  	m_CpHT_B_Zmm_ZRpos[i]   =   CpHT_B_Zmm_ZRpos[i]    ;
  }

  for (int i = 0; i < N_ZRpos_EA; i++){
  	m_CpHT_EA_Zee_ZRpos[i]	=   CpHT_EA_Zee_ZRpos[i]    ;
  	m_CpHT_EA_Zmm_ZRpos[i]  =   CpHT_EA_Zmm_ZRpos[i]    ;
  	m_CpHT_EB_Zee_ZRpos[i]	=   CpHT_EB_Zee_ZRpos[i]    ;
  	m_CpHT_EB_Zmm_ZRpos[i]  =   CpHT_EB_Zmm_ZRpos[i]    ;
  }


  for (int i = 0; i < N_Rtrk_B; i++){
  	m_CpHT_B_Zee_Rtrk[i]	=   CpHT_B_Zee_Rtrk[i]    ;
  	m_CpHT_B_Zmm_Rtrk[i]    =   CpHT_B_Zmm_Rtrk[i]    ;
  	m_CpHT_EA_Zee_Rtrk[i]	=   CpHT_EA_Zee_Rtrk[i]    ;
  	m_CpHT_EA_Zmm_Rtrk[i]   =   CpHT_EA_Zmm_Rtrk[i]    ;
  	m_CpHT_EB_Zee_Rtrk[i]	=   CpHT_EB_Zee_Rtrk[i]    ;
  	m_CpHT_EB_Zmm_Rtrk[i]   =   CpHT_EB_Zmm_Rtrk[i]    ;
  }
        
  for (int i = 0; i < N_PU_B; i++){
  	m_CpHT_B_Zee_PU[i]	=   CpHT_B_Zee_PU[i]    ;
  	m_CpHT_B_Zmm_PU[i]    	=   CpHT_B_Zmm_PU[i]    ;
  	m_CpHT_EA_Zee_PU[i]	=   CpHT_EA_Zee_PU[i]    ;
  	m_CpHT_EA_Zmm_PU[i]   	=   CpHT_EA_Zmm_PU[i]    ;
  	m_CpHT_EB_Zee_PU[i]	=   CpHT_EB_Zee_PU[i]    ;
  	m_CpHT_EB_Zmm_PU[i]   	=   CpHT_EB_Zmm_PU[i]    ;
  }
  
  for (int i = 0; i < N_Occ_B; i++){
  	m_CpHT_B_Zee_Occ[i]	=   CpHT_B_Zee_Occ[i]    ;
  	m_CpHT_B_Zmm_Occ[i]    	=   CpHT_B_Zmm_Occ[i]    ;
  	m_CpHT_EA_Zee_Occ[i]	=   CpHT_EA_Zee_Occ[i]    ;
  	m_CpHT_EA_Zmm_Occ[i]   	=   CpHT_EA_Zmm_Occ[i]    ;
  	m_CpHT_EB_Zee_Occ[i]	=   CpHT_EB_Zee_Occ[i]    ;
  	m_CpHT_EB_Zmm_Occ[i]   	=   CpHT_EB_Zmm_Occ[i]    ;
  }
 
  m_datainplace = true;

}

#endif
#endif
