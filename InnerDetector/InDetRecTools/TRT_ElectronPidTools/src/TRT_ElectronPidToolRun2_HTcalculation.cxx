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
  BaseTRTPIDCalculator(parent,(SIZE_OF_HEADER +
                         SIZE_PAR_SET * 3 +
                         SIZE_STRAW_CONSTANTS ),"HT"),
  StrawConstants( (float*)( Blob + OFF_STRAW_CONSTANTS)),

  ParsBarrel( (float*)( Blob + OFF_PAR_SET_BARREL)),
  ParsEndcapA( (float*)( Blob + OFF_PAR_SET_ENDCAP_A)),
  ParsEndcapB( (float*)( Blob + OFF_PAR_SET_ENDCAP_B))
{
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

// The function for calculating pHT should have the following info:
//
//    pHTel = HTcalc.getProbHT( pTrk, Trk::electron, TrtPart, SL[TrtPart], ZRpos[TrtPart], rTrkWire, Occ[TrtPart]);
//    pHTpi = HTcalc.getProbHT( pTrk, Trk::pion	   , TrtPart, SL[TrtPart], ZRpos[TrtPart], rTrkWire, Occ[TrtPart]);


double InDet::TRT_ElectronPidToolRun2::HTcalculator::getProbHT( double pTrk, Trk::ParticleHypothesis hypothesis, int TrtPart, int StrawLayer, double ZRpos, double rTrkWire, double Occupancy) {
  checkInitialization();
   
  double pHT = 1.0;       // Default/unit value, which ensures that unusable hits does not change probability product!


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
  double mass = m_particlemasses.mass[hypothesis];
  //double x = log10( pTrk / mass );

  if (fabs(mass-0.511) < 0.1) {      // Electron!
    double pHTel = pHTvsP(TrtPart, pTrk, mass);
    pHTel = pHTel * Corr_el_SL(TrtPart, StrawLayer)* Corr_el_SP(TrtPart, ZRpos ) * Corr_el_RT(TrtPart, rTrkWire) * Corr_el_OC(TrtPart, Occupancy);
    return pHTel;
  } else {
    double pHTmu = pHTvsP(TrtPart, pTrk, mass);
    pHTmu = pHTmu * Corr_mu_SL(TrtPart, StrawLayer)* Corr_mu_SP(TrtPart, ZRpos ) * Corr_mu_RT(TrtPart, rTrkWire) * Corr_mu_OC(TrtPart, Occupancy);
    return pHTmu;
  }


  // Troels P (Aug 2014): Old comments, but interesting regarding the splitting into dE/dx and TR parts...
  //
  // Calcualte the probablility based on a logistic sigmoid with constants from the database
  // for retreival of database constants m_gamma_TR_size etc. see the update() function
  // double prob_TR    = par[TR_SIZE] / (1.0 + exp( - ( x - par[TR_MEAN] ) / par[TR_WIDTH] ) );
  // double prob_dEdx  = par[DEDX_C] + x * par[DEDX_M];

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

// Onset functions obtained from 2012 Zee and Zmumu data:
/*
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::pHTvsP_el(int etaBin, double p) {
    double par0[5] = {0.161, 0.164, 0.172, 0.072, 0.062};
    double par1[5] = {0.018, 0.024, 0.021, 0.047, 0.062};
    return par0[etaBin] + par1[etaBin]*log10(p);
  }

  double InDet::TRT_ElectronPidToolRun2::HTcalculator::pHTvsP_mu(bool isMuons, int etaBin, double p) {
    if (isMuons) {
      double par0[5] = {0.067, 0.083, 0.069, 0.070, 0.067};
      double par1[5] = {0.138, 0.136, 0.151, 0.167, 0.212};
      double par2[5] = {5.157, 5.201, 5.241, 5.183, 5.214};
      double par3[5] = {0.194, 0.143, 0.163, 0.150, 0.211};
      return par0[etaBin] + par1[etaBin] / (1.0 + exp(-(log10(p) - par2[etaBin])/par3[etaBin]));
    } else {
      double par0[5] = {0.055, 0.075, 0.055, 0.057, 0.065};
      double par1[5] = {0.133, 0.065, 0.104, 0.237, 0.186};
      double par2[5] = {5.230, 4.554, 4.803, 5.404, 5.199};
      double par3[5] = {0.765, 0.251, 0.425, 0.470, 0.328};
      return par0[etaBin] + par1[etaBin] / (1.0 + exp(-(log10(p) - par2[etaBin])/par3[etaBin]));
    }
  }
*/



double InDet::TRT_ElectronPidToolRun2::HTcalculator::pHTvsP(int TrtPart, double p, double mass) {

  double gamma = sqrt(p*p + mass*mass) / mass;

  // Parameters obtained from fitting B,EA,EB on 2012 Zee,Zmm data:
  // Note: par[0] = -1.5  as fixed (low gamma part consistant with being constant!)
  double par[3][6] = {{-1.5000, 3.8822, 0.0744, 0.1551, 3.2011, 0.2236},
		      {-1.5000, 3.9137, 0.0702, 0.1654, 3.3043, 0.2093},
		      {-1.5000, 3.5856, 0.0736, 0.1827, 3.1540, 0.1683}};

  // TR onset curve part (main part):
  double exp_term = exp(-(log10(gamma) - par[TrtPart][4])/par[TrtPart][5]);
  double pHT_TR   = par[TrtPart][2] + par[TrtPart][3]/(1.0 + exp_term);

  // dE/dx part (low gamma) - IGNORED FOR NOW (as data was consistent with this!):
  double exp_term0 = exp(-(par[TrtPart][0] - par[TrtPart][4])/par[TrtPart][5]);
  double alpha0 = par[TrtPart][2] + par[TrtPart][3]/(1.0 + exp_term0);
  double beta0 = par[TrtPart][3] / ((1.0 + exp_term0)*(1.0 + exp_term0)) * exp_term0 / par[TrtPart][5];
  double pHT_dEdx = alpha0 + beta0*(log10(gamma) - par[TrtPart][0]);

  // High-gamma part:
  double exp_term1 = exp(-(par[TrtPart][1] - par[TrtPart][4])/par[TrtPart][5]);
  double alpha1 = par[TrtPart][2] + par[TrtPart][3]/(1.0 + exp_term1);
  double beta1 = par[TrtPart][3] / ((1.0 + exp_term1)*(1.0 + exp_term1)) * exp_term1 / par[TrtPart][5];
  double pHT_HG   = alpha1 + beta1*(log10(gamma) - par[TrtPart][1]);

  if (log10(gamma)      < par[TrtPart][0]) return pHT_dEdx;
  else if (log10(gamma) > par[TrtPart][1]) return pHT_HG;
  else                                     return pHT_TR;
}




    /*
// Onset functions obtained from 2011 MC (official plots):
double InDet::TRT_ElectronPidToolRun2::HTcalculator::pHTvsP_el(int etaBin, double p) {

  double m_elec = 0.511;
  double gamma = sqrt(p*p + m_elec*m_elec) / m_elec;
  double pHT = 0.5;
  
  // A bit of an ugly function, but a simpler one is hard to come about (Troels):
  if (etaBin == 0) {
    if (gamma < 5011.87) {
      double par[4] = {4.82574e-002, 1.62958e-001, 3.10000e+000, 2.34773e-001};
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else {
      double par[2] = {9.91398e-002, 2.70978e-002};
      pHT = par[0]+par[1]*log10(gamma);
    }      
    
    
  } else if (etaBin == 1) {
    if (gamma < 5011.87) {
      double par[4] = {5.65901e-002, 1.45264e-001, 3.02967e+000, 1.49026e-001};
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else if (5011.87 < gamma && gamma < 25118.86) {
      double par[3] = {1.96000e-001,  3.22000e+000,  2.40000e-002};
      pHT = par[0]+(par[2]*(log10(gamma)-par[1])*(log10(gamma)-par[1]));
    } else {
      double par[2] = {-2.69317e-002, 5.80976e-002};
      pHT = par[0]+par[1]*log10(gamma);
    }
    
    
  } else if (etaBin == 2) {
    if (gamma < 31622.78) {
      double par[4] = {6.03126e-002, 1.68392e-001, 3.10841e+000, 1.23134e-001};
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else if (31622.78 < gamma && gamma < 79432.82) {
      double par[3] = {2.27800e-001, 4.41600e+000, 4.53180e-002};
      pHT = par[0]+(par[2]*(log10(gamma)-par[1])*(log10(gamma)-par[1]));
    } else {
      double par[2] = {-1.48591e-001, 7.91818e-002};
      pHT = par[0]+par[1]*log10(gamma);
    }      


  } else if (etaBin == 3) {
    if (gamma < 31622.78) {
      double par[4] = {5.45365e-002, 1.90762e-001, 3.11068e+000, 1.38416e-001};
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else if (31622.78 < gamma && gamma < 79432.82) {
      double par[3] = { 2.45000e-001,  4.60917e+000,  5.81999e-002};
      pHT = par[0]+(par[2]*(log10(gamma)-par[1])*(log10(gamma)-par[1]));
    } else {
      double par[2] = {-3.51417e-001, 1.22328e-001};
      pHT = par[0]+par[1]*log10(gamma);
    }      
    

  } else if (etaBin == 4) {
    if (gamma < 31622.78) {
      double par[4] = {5.38652e-002 , 2.34635e-001 , 3.11902e+000 , 1.50858e-001 };
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else if (31622.78 < gamma && gamma < 79432.82) {
      double par[3] = {2.88566e-001, 4.51096e+000, 1.60000e-001};
      pHT = par[0]+(par[2]*(log10(gamma)-par[1])*(log10(gamma)-par[1]));
    } else {
      double par[2] = {-3.15981e-001, 1.28435e-001};
      pHT = par[0]+par[1]*log10(gamma);
    }      
  } else {
    // ATH_MSG_WARNING("EtaBin not in allowed range! Obvious mistake!");      
  }

  return pHT;
}
    */



    /*
// Onset functions obtained from 2011 MC (official plots):
double InDet::TRT_ElectronPidToolRun2::HTcalculator::pHTvsP_pi(int etaBin, double p) {

  double m_pion = 139.57;    // Use pion mass?
  double gamma = sqrt(p*p + m_pion*m_pion) / m_pion;
  double pHT = 0.5;
  
  // Really ugly function, but this was how it was passed to me (Troels):
  if (etaBin == 0) {
    if (gamma < 5011.87) {
      double par[4] = {4.82574e-002, 1.62958e-001, 3.10000e+000, 2.34773e-001};
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else {
      double par[2] = {9.91398e-002, 2.70978e-002};
      pHT = par[0]+par[1]*log10(gamma);
    }      
    
    
  } else if (etaBin == 1) {
    if (gamma < 5011.87) {
      double par[4] = {5.65901e-002, 1.45264e-001, 3.02967e+000, 1.49026e-001};
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else if (5011.87 < gamma && gamma < 25118.86) {
      double par[3] = {1.96000e-001,  3.22000e+000,  2.40000e-002};
      pHT = par[0]+(par[2]*(log10(gamma)-par[1])*(log10(gamma)-par[1]));
    } else {
      double par[2] = {-2.69317e-002, 5.80976e-002};
      pHT = par[0]+par[1]*log10(gamma);
    }      


  } else if (etaBin == 2) {
    if (gamma < 31622.78) {
      double par[4] = {6.03126e-002, 1.68392e-001, 3.10841e+000, 1.23134e-001};
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else if (31622.78 < gamma && gamma < 79432.82) {
      double par[3] = {2.27800e-001, 4.41600e+000, 4.53180e-002};
      pHT = par[0]+(par[2]*(log10(gamma)-par[1])*(log10(gamma)-par[1]));
    } else {
      double par[2] = {-1.48591e-001, 7.91818e-002};
      pHT = par[0]+par[1]*log10(gamma);
    }      

    
  } else if (etaBin == 3) {
    if (gamma < 31622.78) {
      double par[4] = {5.45365e-002, 1.90762e-001, 3.11068e+000, 1.38416e-001};
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else if (31622.78 < gamma && gamma < 79432.82) {
      double par[3] = { 2.45000e-001,  4.60917e+000,  5.81999e-002};
      pHT = par[0]+(par[2]*(log10(gamma)-par[1])*(log10(gamma)-par[1]));
    } else {
      double par[2] = {-3.51417e-001, 1.22328e-001};
      pHT = par[0]+par[1]*log10(gamma);
    }      

    
  } else if (etaBin == 4) {
    if (gamma < 31622.78) {
      double par[4] = {5.38652e-002 , 2.34635e-001 , 3.11902e+000 , 1.50858e-001 };
      pHT = par[0]+(par[1]/(1.0+exp(-((log10(gamma)-par[2])/par[3]))));
    } else if (31622.78 < gamma && gamma < 79432.82) {
      double par[3] = {2.88566e-001, 4.51096e+000, 1.60000e-001};
      pHT = par[0]+(par[2]*(log10(gamma)-par[1])*(log10(gamma)-par[1]));
    } else {
      double par[2] = {-3.15981e-001, 1.28435e-001};
      pHT = par[0]+par[1]*log10(gamma);
    }      
  } else {
    // ATH_MSG_WARNING("EtaBin not in allowed range! Obvious mistake!");      
  }

  return pHT;
}

    */
















  // ------------------------------------------------------------------------------------------------------------ //
  // NEW and IMPROVED parametrization of pHT for electrons and muons:
  // ------------------------------------------------------------------------------------------------------------ //

  // -------------------------------------------------------- //
  // Electrons:
  // -------------------------------------------------------- //

  // Straw Layer (SL):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_SL(int part, double SL) {
    int SLbin = int(SL);
    if (part == 0) {
      double CpHT_B_Zee_SL[73] = { 0.654, 0.889, 0.957, 1.025, 1.036, 0.988, 1.107, 1.053, 1.058, 0.948, 0.910, 1.006, 0.953, 0.978, 0.991, 0.947, 1.018, 0.987, 1.001, 0.832, 0.949, 0.967, 0.996, 0.996, 1.016, 0.957, 1.049, 1.001, 1.029, 1.017, 0.970, 1.060, 1.017, 1.034, 1.033, 0.979, 1.053, 1.017, 1.029, 1.049, 0.988, 1.051, 1.024, 0.831, 1.009, 0.973, 1.043, 1.024, 0.965, 1.068, 1.016, 1.046, 1.039, 0.987, 1.072, 1.025, 1.049, 1.043, 0.991, 1.073, 1.021, 1.040, 1.045, 0.982, 1.073, 1.046, 1.035, 1.043, 0.977, 1.065, 1.030, 1.024, 1.044}; 
      if (SLbin < 0 || SLbin > 72) printf("  ERROR: SLbin = %5d \n", SLbin); else return CpHT_B_Zee_SL[SLbin];
    }
    if (part == 1) {
      double CpHT_EA_Zee_SL[16] = { 0.854, 0.926, 0.966, 0.979, 1.011, 1.025, 1.021, 1.023, 1.037, 1.032, 1.033, 1.022, 1.036, 1.046, 1.032, 1.034}; 
      if (SLbin < 0 || SLbin > 15) printf("  ERROR: SLbin = %5d \n", SLbin); else return CpHT_EA_Zee_SL[SLbin];
    }
    if (part == 2) {
      double CpHT_EB_Zee_SL[8] = { 0.833, 0.956, 1.022, 1.046, 1.026, 1.059, 1.065, 1.077}; 
      if (SLbin < 0 || SLbin > 7) printf("  ERROR: SLbin = %5d \n", SLbin); else return CpHT_EB_Zee_SL[SLbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // ZR-position (ZR - Z in Barrel, R in Endcaps):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_SP(int part, double SP) {
    if (part == 0) {
      double CpHT_B_Zee_ZRpos[36] = { 0.877, 0.909, 0.915, 0.922, 0.927, 0.932, 0.933, 0.937, 0.939, 0.945, 0.953, 0.916, 0.929, 0.966, 0.974, 0.991, 0.992, 1.003, 1.013, 1.006, 0.992, 1.007, 1.017, 0.990, 1.039, 1.069, 1.090, 1.107, 1.126, 1.144, 1.156, 1.183, 1.193, 1.210, 1.206, 1.029}; 
      int SPbin = int(SP/720.0*36.0);
      if (SPbin < 0 || SPbin > 35) printf("  ERROR: SPbin = %5d \n", SPbin); else return CpHT_B_Zee_ZRpos[SPbin];
    }
    if (part == 1) {
      double CpHT_EA_Zee_ZRpos[50] = { 0.472, 0.608, 0.688, 0.722, 0.779, 0.807, 0.844, 0.874, 0.882, 0.915, 0.956, 0.973, 0.984, 1.002, 0.998, 1.008, 1.019, 1.030, 1.014, 1.026, 1.039, 1.042, 1.040, 1.049, 1.055, 1.059, 1.069, 1.076, 1.082, 1.078, 1.084, 1.101, 1.105, 1.106, 1.101, 1.098, 1.101, 1.107, 1.104, 1.080, 1.092, 1.081, 1.060, 1.041, 1.021, 0.988, 0.965, 0.939, 0.871, 0.740}; 
      int SPbin = int((SP-640.0)/500.0*50.0);
      if (SPbin < 0 || SPbin > 49) printf("  ERROR: SPbin = %5d \n", SPbin); else return CpHT_EA_Zee_ZRpos[SPbin];
    }
    if (part == 2) {
      double CpHT_EB_Zee_ZRpos[50] = { 0.500, 0.654, 0.863, 0.907, 0.944, 0.972, 0.974, 0.986, 1.005, 1.033, 1.050, 1.059, 1.055, 1.061, 1.050, 1.068, 1.063, 1.066, 1.052, 1.082, 1.057, 1.061, 1.049, 1.055, 1.060, 1.055, 1.046, 1.058, 1.056, 1.067, 1.086, 1.060, 1.056, 1.051, 1.040, 0.993, 0.961, 0.935, 0.919, 0.892, 0.882, 0.846, 0.849, 0.796, 0.784, 0.719, 0.623, 0.471, 0.275, 0.331}; 
      int SPbin = int((SP-640.0)/500.0*50.0);
      if (SPbin < 0 || SPbin > 49) printf("  ERROR: SPbin = %5d \n", SPbin); else return CpHT_EB_Zee_ZRpos[SPbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Track-to-Wire distance (TWdist):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_RT(int part, double RT) {
    int RTbin = int(RT/2.2*22.0);
    if (part == 0) {
      double CpHT_B_Zee_Rtrk[22] = { 1.294, 1.331, 1.341, 1.334, 1.326, 1.317, 1.299, 1.276, 1.242, 1.203, 1.166, 1.121, 1.071, 1.013, 0.944, 0.868, 0.775, 0.674, 0.577, 0.536, 0.622, 0.689}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return CpHT_B_Zee_Rtrk[RTbin];
    }
    if (part == 1) {
      double CpHT_EA_Zee_Rtrk[22] = { 1.315, 1.347, 1.351, 1.358, 1.350, 1.343, 1.322, 1.309, 1.281, 1.250, 1.216, 1.168, 1.118, 1.058, 0.982, 0.887, 0.783, 0.671, 0.557, 0.495, 0.539, 0.718}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return CpHT_EA_Zee_Rtrk[RTbin];
    }
    if (part == 2) {
      double CpHT_EB_Zee_Rtrk[22] = { 1.272, 1.302, 1.312, 1.317, 1.293, 1.305, 1.290, 1.269, 1.256, 1.226, 1.201, 1.165, 1.116, 1.060, 0.997, 0.909, 0.809, 0.722, 0.611, 0.530, 0.498, 0.594}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return CpHT_EB_Zee_Rtrk[RTbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Pile-UP (PU):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_PU(int part, double PU) {
    int PUbin = int(PU);
    if (PUbin > 39) PUbin = 39;       // NOTE: Simply put a maximum by hand (to avoid multiple printouts)!!!
    if (part == 0) {
      double CpHT_B_Zee_PU[40] = { 1.004, 1.004, 1.004, 1.004, 1.004, 1.003, 1.003, 1.003, 1.003, 1.003, 1.003, 1.002, 1.002, 1.002, 1.002, 1.002, 1.002, 1.001, 1.001, 1.001, 1.001, 1.001, 1.001, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 0.999, 0.999, 0.999, 0.999, 0.999, 0.999, 0.999, 0.998, 0.998, 0.998, 0.998}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return CpHT_B_Zee_PU[PUbin];
    }
    if (part == 1) {
      double CpHT_EA_Zee_PU[40] = { 1.064, 1.061, 1.059, 1.056, 1.054, 1.052, 1.049, 1.047, 1.044, 1.042, 1.039, 1.037, 1.034, 1.032, 1.029, 1.027, 1.024, 1.022, 1.019, 1.017, 1.014, 1.012, 1.009, 1.007, 1.004, 1.002, 1.000, 0.997, 0.995, 0.992, 0.990, 0.987, 0.985, 0.982, 0.980, 0.977, 0.975, 0.972, 0.970, 0.967}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return CpHT_EA_Zee_PU[PUbin];
    }
    if (part == 2) {
      double CpHT_EB_Zee_PU[40] = { 1.061, 1.058, 1.056, 1.054, 1.051, 1.049, 1.047, 1.044, 1.042, 1.040, 1.037, 1.035, 1.033, 1.030, 1.028, 1.025, 1.023, 1.021, 1.018, 1.016, 1.014, 1.011, 1.009, 1.007, 1.004, 1.002, 1.000, 0.997, 0.995, 0.993, 0.990, 0.988, 0.985, 0.983, 0.981, 0.978, 0.976, 0.974, 0.971, 0.969}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return CpHT_EB_Zee_PU[PUbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Occupancy (OC):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_el_OC(int part, double OC) {
    int OCbin = int(OC*100.0);
    if (OCbin > 59) OCbin = 59;       // NOTE: Simply put a maximum by hand (to avoid multiple printouts)!!!
    if (part == 0) {
      double CpHT_B_Zee_Occ[60] = { 0.917, 0.919, 0.922, 0.925, 0.928, 0.930, 0.933, 0.936, 0.939, 0.942, 0.944, 0.947, 0.950, 0.953, 0.956, 0.958, 0.961, 0.964, 0.967, 0.970, 0.972, 0.975, 0.978, 0.981, 0.983, 0.986, 0.989, 0.992, 0.995, 0.997, 1.000, 1.003, 1.006, 1.009, 1.011, 1.014, 1.017, 1.020, 1.023, 1.025, 1.028, 1.031, 1.034, 1.036, 1.039, 1.042, 1.045, 1.048, 1.050, 1.053, 1.056, 1.059, 1.062, 1.064, 1.067, 1.070, 1.073, 1.076, 1.078, 1.081}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return CpHT_B_Zee_Occ[OCbin];
    }
    if (part == 1) {
      double CpHT_EA_Zee_Occ[60] = { 0.980, 0.981, 0.981, 0.982, 0.983, 0.984, 0.984, 0.985, 0.986, 0.986, 0.987, 0.988, 0.989, 0.989, 0.990, 0.991, 0.991, 0.992, 0.993, 0.994, 0.994, 0.995, 0.996, 0.996, 0.997, 0.998, 0.999, 0.999, 1.000, 1.001, 1.001, 1.002, 1.003, 1.004, 1.004, 1.005, 1.006, 1.006, 1.007, 1.008, 1.009, 1.009, 1.010, 1.011, 1.011, 1.012, 1.013, 1.014, 1.014, 1.015, 1.016, 1.016, 1.017, 1.018, 1.019, 1.019, 1.020, 1.021, 1.021, 1.022}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return CpHT_EA_Zee_Occ[OCbin];
    }
    if (part == 2) {
      double CpHT_EB_Zee_Occ[60] = { 0.984, 0.985, 0.985, 0.986, 0.986, 0.987, 0.988, 0.988, 0.989, 0.989, 0.990, 0.990, 0.991, 0.992, 0.992, 0.993, 0.993, 0.994, 0.994, 0.995, 0.995, 0.996, 0.997, 0.997, 0.998, 0.998, 0.999, 0.999, 1.000, 1.001, 1.001, 1.002, 1.002, 1.003, 1.003, 1.004, 1.004, 1.005, 1.006, 1.006, 1.007, 1.007, 1.008, 1.008, 1.009, 1.010, 1.010, 1.011, 1.011, 1.012, 1.012, 1.013, 1.014, 1.014, 1.015, 1.015, 1.016, 1.016, 1.017, 1.017}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return CpHT_EB_Zee_Occ[OCbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }





  // -------------------------------------------------------- //
  // Muons:
  // -------------------------------------------------------- //

  // Straw Layer (SL):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_SL(int part, double SL) {
    int SLbin = int(SL);
    if (part == 0) {
      double CpHT_B_Zmm_SL[73] = { 1.068, 1.130, 1.150, 1.180, 1.129, 1.116, 1.146, 1.102, 1.073, 1.102, 1.083, 1.095, 1.071, 1.058, 1.075, 1.071, 1.068, 1.026, 1.026, 1.020, 1.030, 1.051, 1.067, 1.050, 1.049, 1.037, 1.052, 1.028, 1.027, 1.028, 1.007, 1.043, 1.024, 1.013, 1.005, 0.994, 1.004, 0.986, 0.982, 0.995, 0.978, 0.972, 0.954, 0.940, 0.976, 0.984, 0.997, 0.987, 0.957, 0.982, 0.984, 0.975, 0.981, 0.960, 0.981, 0.952, 0.965, 0.951, 0.941, 0.952, 0.928, 0.936, 0.936, 0.917, 0.933, 0.932, 0.896, 0.900, 0.892, 0.898, 0.885, 0.880, 0.874}; 
      if (SLbin < 0 || SLbin > 72) printf("  ERROR: SLbin = %5d \n", SLbin); else return CpHT_B_Zmm_SL[SLbin];
    }
    if (part == 1) {
      double CpHT_EA_Zmm_SL[16] = { 0.966, 0.969, 0.979, 0.969, 0.986, 1.005, 0.997, 1.023, 1.020, 1.002, 1.018, 1.002, 1.013, 1.054, 1.013, 1.013}; 
      if (SLbin < 0 || SLbin > 15) printf("  ERROR: SLbin = %5d \n", SLbin); else return CpHT_EA_Zmm_SL[SLbin];
    }
    if (part == 2) {
      double CpHT_EB_Zmm_SL[8] = { 0.926, 0.995, 1.026, 1.028, 0.997, 1.005, 1.013, 1.038}; 
      if (SLbin < 0 || SLbin > 7) printf("  ERROR: SLbin = %5d \n", SLbin); else return CpHT_EB_Zmm_SL[SLbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // ZR-position (ZR - Z in Barrel, R in Endcaps):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_SP(int part, double SP) {
    if (part == 0) {
      double CpHT_B_Zmm_ZRpos[36] = { 0.884, 0.902, 0.906, 0.911, 0.902, 0.908, 0.922, 0.912, 0.914, 0.922, 0.922, 0.921, 0.933, 0.952, 0.957, 0.959, 0.971, 0.980, 0.992, 0.994, 1.003, 1.014, 1.017, 1.024, 1.055, 1.064, 1.086, 1.116, 1.134, 1.156, 1.186, 1.197, 1.217, 1.251, 1.249, 1.042}; 
      int SPbin = int(SP/720.0*36.0);
      if (SPbin < 0 || SPbin > 35) printf("  ERROR: SPbin = %5d \n", SPbin); else return CpHT_B_Zmm_ZRpos[SPbin];
    }
    if (part == 1) {
      double CpHT_EA_Zmm_ZRpos[50] = { 0.680, 0.756, 0.809, 0.808, 0.864, 0.872, 0.888, 0.907, 0.923, 0.938, 0.932, 0.960, 0.980, 0.972, 0.984, 0.979, 0.986, 0.980, 0.998, 0.999, 0.996, 1.014, 1.009, 1.005, 1.025, 1.020, 1.030, 1.020, 1.048, 1.049, 1.050, 1.045, 1.044, 1.049, 1.059, 1.064, 1.047, 1.065, 1.076, 1.069, 1.073, 1.076, 1.066, 1.080, 1.080, 1.070, 1.066, 1.038, 1.088, 1.012}; 
      int SPbin = int((SP-640.0)/500.0*50.0);
      if (SPbin < 0 || SPbin > 49) printf("  ERROR: SPbin = %5d \n", SPbin); else return CpHT_EA_Zmm_ZRpos[SPbin];
    }
    if (part == 2) {
      double CpHT_EB_Zmm_ZRpos[50] = { 0.500, 0.673, 1.117, 0.988, 0.813, 1.029, 0.845, 0.912, 0.963, 0.867, 1.083, 0.955, 0.953, 1.051, 1.089, 0.963, 0.976, 0.908, 1.090, 1.033, 1.034, 1.074, 1.004, 1.004, 1.055, 1.062, 1.003, 1.003, 1.028, 1.043, 1.014, 1.000, 1.012, 1.018, 1.037, 1.064, 1.024, 0.990, 0.971, 0.981, 1.022, 0.974, 0.936, 0.916, 0.984, 0.900, 0.933, 0.858, 0.687, 0.907}; 
      int SPbin = int((SP-640.0)/500.0*50.0);
      if (SPbin < 0 || SPbin > 49) printf("  ERROR: SPbin = %5d \n", SPbin); else return CpHT_EB_Zmm_ZRpos[SPbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Track-to-Wire distance (TWdist):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_RT(int part, double RT) {
    int RTbin = int(RT/2.2*22.0);
    if (part == 0) {
      double CpHT_B_Zmm_Rtrk[22] = { 1.143, 1.133, 1.143, 1.152, 1.157, 1.151, 1.138, 1.139, 1.119, 1.094, 1.067, 1.034, 1.002, 0.953, 0.899, 0.841, 0.787, 0.734, 0.727, 0.882, 1.336, 1.434}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return CpHT_B_Zmm_Rtrk[RTbin];
    }
    if (part == 1) {
      double CpHT_EA_Zmm_Rtrk[22] = { 1.236, 1.235, 1.260, 1.274, 1.266, 1.270, 1.260, 1.253, 1.225, 1.201, 1.154, 1.113, 1.045, 0.993, 0.905, 0.809, 0.716, 0.626, 0.582, 0.666, 1.048, 1.083}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return CpHT_EA_Zmm_Rtrk[RTbin];
    }
    if (part == 2) {
      double CpHT_EB_Zmm_Rtrk[22] = { 1.159, 1.208, 1.297, 1.188, 1.248, 1.228, 1.181, 1.205, 1.201, 1.149, 1.128, 1.089, 1.038, 0.942, 0.884, 0.843, 0.781, 0.666, 0.653, 0.672, 0.979, 0.888}; 
      if (RTbin < 0 || RTbin > 21) printf("  ERROR: RTbin = %5d \n", RTbin); else return CpHT_EB_Zmm_Rtrk[RTbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Pile-UP (PU):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_PU(int part, double PU) {
    int PUbin = int(PU);
    if (PUbin > 39) PUbin = 39;       // NOTE: Simply put a maximum by hand (to avoid multiple printouts)!!!
    if (part == 0) {
      double CpHT_B_Zmm_PU[40] = { 0.761, 0.773, 0.786, 0.798, 0.810, 0.822, 0.834, 0.846, 0.859, 0.871, 0.883, 0.895, 0.907, 0.919, 0.932, 0.944, 0.956, 0.968, 0.980, 0.992, 1.005, 1.017, 1.029, 1.041, 1.053, 1.065, 1.078, 1.090, 1.102, 1.114, 1.126, 1.138, 1.151, 1.163, 1.175, 1.187, 1.199, 1.211, 1.224, 1.236}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return CpHT_B_Zmm_PU[PUbin];
    }
    if (part == 1) {
      double CpHT_EA_Zmm_PU[40] = { 0.894, 0.899, 0.905, 0.910, 0.915, 0.921, 0.926, 0.931, 0.937, 0.942, 0.947, 0.953, 0.958, 0.963, 0.969, 0.974, 0.979, 0.985, 0.990, 0.995, 1.001, 1.006, 1.011, 1.016, 1.022, 1.027, 1.032, 1.038, 1.043, 1.048, 1.054, 1.059, 1.064, 1.070, 1.075, 1.080, 1.086, 1.091, 1.096, 1.102}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return CpHT_EA_Zmm_PU[PUbin];
    }
    if (part == 2) {
      double CpHT_EB_Zmm_PU[40] = { 0.828, 0.837, 0.845, 0.854, 0.862, 0.871, 0.880, 0.888, 0.897, 0.905, 0.914, 0.922, 0.931, 0.940, 0.948, 0.957, 0.965, 0.974, 0.982, 0.991, 0.999, 1.008, 1.017, 1.025, 1.034, 1.042, 1.051, 1.059, 1.068, 1.077, 1.085, 1.094, 1.102, 1.111, 1.119, 1.128, 1.136, 1.145, 1.154, 1.162}; 
      if (PUbin < 0 || PUbin > 39) printf("  ERROR: PUbin = %5d \n", PUbin); else return CpHT_EB_Zmm_PU[PUbin];
    }
    printf("  ERROR: Either part is not in [0,2] or binning is wrong! \n");
    return 1.0;
  }


  // Occupancy (OC):
  double InDet::TRT_ElectronPidToolRun2::HTcalculator::Corr_mu_OC(int part, double OC) {
    int OCbin = int(OC*100.0);
    if (OCbin > 59) OCbin = 59;       // NOTE: Simply put a maximum by hand (to avoid multiple printouts)!!!
    if (part == 0) {
      double CpHT_B_Zmm_Occ[60] = { 0.633, 0.648, 0.663, 0.678, 0.693, 0.709, 0.724, 0.739, 0.754, 0.769, 0.785, 0.800, 0.815, 0.830, 0.845, 0.860, 0.876, 0.891, 0.906, 0.921, 0.936, 0.952, 0.967, 0.982, 0.997, 1.012, 1.028, 1.043, 1.058, 1.073, 1.088, 1.103, 1.119, 1.134, 1.149, 1.164, 1.179, 1.195, 1.210, 1.225, 1.240, 1.255, 1.270, 1.286, 1.301, 1.316, 1.331, 1.346, 1.362, 1.377, 1.392, 1.407, 1.422, 1.437, 1.453, 1.468, 1.483, 1.498, 1.513, 1.529}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return CpHT_B_Zmm_Occ[OCbin];
    }
    if (part == 1) {
      double CpHT_EA_Zmm_Occ[60] = { 0.807, 0.815, 0.824, 0.832, 0.841, 0.849, 0.858, 0.866, 0.875, 0.883, 0.892, 0.900, 0.909, 0.917, 0.926, 0.934, 0.943, 0.951, 0.960, 0.968, 0.977, 0.985, 0.994, 1.002, 1.011, 1.019, 1.028, 1.036, 1.045, 1.053, 1.062, 1.070, 1.079, 1.087, 1.096, 1.104, 1.113, 1.121, 1.130, 1.138, 1.147, 1.155, 1.164, 1.172, 1.181, 1.189, 1.198, 1.206, 1.215, 1.223, 1.232, 1.240, 1.249, 1.257, 1.266, 1.274, 1.283, 1.291, 1.300, 1.308}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return CpHT_EA_Zmm_Occ[OCbin];
    }
    if (part == 2) {
      double CpHT_EB_Zmm_Occ[60] = { 0.733, 0.744, 0.756, 0.767, 0.779, 0.791, 0.802, 0.814, 0.825, 0.837, 0.849, 0.860, 0.872, 0.883, 0.895, 0.906, 0.918, 0.930, 0.941, 0.953, 0.964, 0.976, 0.987, 0.999, 1.011, 1.022, 1.034, 1.045, 1.057, 1.069, 1.080, 1.092, 1.103, 1.115, 1.126, 1.138, 1.150, 1.161, 1.173, 1.184, 1.196, 1.207, 1.219, 1.231, 1.242, 1.254, 1.265, 1.277, 1.289, 1.300, 1.312, 1.323, 1.335, 1.346, 1.358, 1.370, 1.381, 1.393, 1.404, 1.416}; 
      if (OCbin < 0 || OCbin > 59) printf("  ERROR: OCbin = %5d \n", OCbin); else return CpHT_EB_Zmm_Occ[OCbin];
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

  /*
  //For MC:
  ParsBarrel[DEDX_C]=4.12398e-02;
  ParsBarrel[DEDX_M]=2.92087e-03;
  ParsBarrel[TR_SIZE]=1.47385e-01;
  ParsBarrel[TR_MEAN]=3.31603e+00;
  ParsBarrel[TR_WIDTH]=1.84435e-01;

  ParsEndcapA[DEDX_C]=3.84573e-02;
  ParsEndcapA[DEDX_M]=6.13286e-03;
  ParsEndcapA[TR_SIZE]=1.03985e-01;
  ParsEndcapA[TR_MEAN]=3.25720e+00;
  ParsEndcapA[TR_WIDTH]=1.15939e-01;

  ParsEndcapB[DEDX_C]=2.81769e-02;
  ParsEndcapB[DEDX_M]=1.10635e-02;
  ParsEndcapB[TR_SIZE]=1.74251e-01;
  ParsEndcapB[TR_MEAN]=3.33191e+00;
  ParsEndcapB[TR_WIDTH]=1.55063e-01;
  */
  
  //For data we found the following:

  ParsBarrel[DEDX_C]=3.65026e-2;
  ParsBarrel[DEDX_M]=5.45505e-3;
  ParsBarrel[TR_SIZE]=1.55901e-1;
  ParsBarrel[TR_MEAN]=3.28898;
  ParsBarrel[TR_WIDTH]=2.06788e-1;

  ParsEndcapA[DEDX_C]=3.38951e-2;
  ParsEndcapA[DEDX_M]=6.40528e-3;
  ParsEndcapA[TR_SIZE]=1.41382e-1;
  ParsEndcapA[TR_MEAN]=3.28648;
  ParsEndcapA[TR_WIDTH]=1.57271e-1;

  ParsEndcapB[DEDX_C]=3.64409e-02;
  ParsEndcapB[DEDX_M]=2.83820e-03;
  ParsEndcapB[TR_SIZE]=2.28494e-01;
  ParsEndcapB[TR_MEAN]=3.33994e+00;
  ParsEndcapB[TR_WIDTH]=2.43090e-01;
  

  StrawConstants[ 0 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 0 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 1 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 1 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 2 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 2 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 3 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 3 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 4 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 4 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 5 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 5 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 6 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 6 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 7 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 7 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 8 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 8 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 9 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 9 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 10 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 10 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 11 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 11 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 12 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 12 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 13 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 13 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 14 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 14 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 15 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 15 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 16 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 16 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 17 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 17 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 18 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 18 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 19 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 19 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 20 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 20 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 21 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 21 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 22 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 22 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 23 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 23 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 24 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 24 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 25 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 25 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 26 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 26 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 27 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 27 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 28 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 28 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 29 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 29 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 30 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 30 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 31 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 31 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 32 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 32 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 33 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 33 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 34 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 34 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 35 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 35 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 36 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 36 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 37 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 37 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 38 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 38 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 39 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 39 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 40 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 40 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 41 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 41 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 42 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 42 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 43 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 43 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 44 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 44 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 45 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 45 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 46 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 46 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 47 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 47 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 48 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 48 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 49 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 49 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 50 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 50 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 51 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 51 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 52 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 52 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 53 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 53 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 54 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 54 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 55 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 55 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 56 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 56 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 57 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 57 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 58 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 58 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 59 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 59 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 60 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 60 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 61 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 61 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 62 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 62 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 63 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 63 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 64 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 64 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 65 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 65 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 66 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 66 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 67 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 67 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 68 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 68 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 69 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 69 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 70 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 70 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 71 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 71 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 72 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 72 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 73 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 73 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 74 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 74 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 75 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 75 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 76 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 76 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 77 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 77 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 78 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 78 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 79 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 79 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 80 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 80 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 81 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 81 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 82 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 82 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 83 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 83 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 84 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 84 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 85 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 85 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 86 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 86 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 87 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 87 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 88 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 88 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 89 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 89 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 90 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 90 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 91 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 91 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 92 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 92 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 93 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 93 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 94 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 94 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 95 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 95 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 96 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 96 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 97 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 97 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 98 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 98 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 99 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 99 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 100 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 100 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 101 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 101 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 102 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 102 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 103 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 103 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 104 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 104 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 105 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 105 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 106 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 106 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 107 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 107 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 108 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 108 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 109 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 109 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 110 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 110 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 111 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 111 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 112 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 112 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 113 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 113 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 114 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 114 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 115 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 115 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 116 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 116 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 117 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 117 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 118 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 118 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 119 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 119 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 120 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 120 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 121 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 121 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 122 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 122 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 123 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 123 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 124 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 124 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 125 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 125 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 126 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 126 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 127 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 127 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 128 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 128 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 129 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 129 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 130 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 130 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 131 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 131 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 132 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 132 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 133 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 133 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 134 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 134 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 135 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 135 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 136 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 136 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 137 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 137 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 138 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 138 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 139 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 139 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 140 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 140 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 141 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 141 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 142 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 142 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 143 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 143 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 144 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 144 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 145 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 145 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 146 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 146 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 147 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 147 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 148 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 148 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 149 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 149 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 150 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 150 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 151 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 151 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 152 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 152 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 153 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 153 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 154 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 154 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 155 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 155 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 156 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 156 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 157 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 157 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 158 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 158 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 159 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 159 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 160 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 160 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 161 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 161 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 162 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 162 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 163 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 163 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 164 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 164 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 165 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 165 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 166 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 166 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 167 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 167 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 168 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 168 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 169 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 169 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 170 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 170 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 171 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 171 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 172 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 172 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 173 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 173 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 174 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 174 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 175 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 175 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 176 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 176 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 177 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 177 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 178 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 178 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 179 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 179 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 180 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 180 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 181 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 181 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 182 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 182 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 183 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 183 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 184 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 184 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 185 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 185 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 186 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 186 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 187 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 187 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 188 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 188 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 189 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 189 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 190 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 190 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 191 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 191 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 192 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 192 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 193 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 193 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 194 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 194 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 195 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 195 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 196 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 196 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 197 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 197 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 198 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 198 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 199 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 199 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 200 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 200 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 201 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 201 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 202 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 202 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 203 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 203 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 204 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 204 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 205 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 205 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 206 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 206 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 207 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 207 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 208 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 208 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 209 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 209 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 210 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 210 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 211 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 211 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 212 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 212 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 213 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 213 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 214 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 214 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 215 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 215 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 216 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 216 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 217 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 217 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 218 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 218 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 219 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 219 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 220 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 220 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 221 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 221 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 222 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 222 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 223 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 223 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 224 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 224 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 225 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 225 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 226 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 226 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 227 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 227 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 228 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 228 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 229 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 229 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 230 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 230 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 231 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 231 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 232 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 232 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;

}

#endif
#endif
