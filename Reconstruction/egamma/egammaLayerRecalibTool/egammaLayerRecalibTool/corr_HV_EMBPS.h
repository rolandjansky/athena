/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORR_HV_EMBPS_H
#define CORR_HV_EMBPS_H
#include "TProfile2D.h"
#include "TFile.h"

/**
  @class corr_HV_EMBPS
  @brief Correction for time dependent HV effect in barrel presampler scale in 2012

  Perform correction of E0_RAW (EMPBS) as function of run number, eta, phi to recover
  effective scale at 1600V at the beginning of 2012
  Data are divided intwo 6 time snapshots

   runs 200804-204910   (nominal 1600V for most EMBPS, 2000V EMECPS)
   runs 204932-205071   (20 EMBPS lines lowered to 1400V)
   runs 205112-211620   (22 EMBPS lines lowered from 1400 to 1200V)
   runs 211670-212272   (all EMBPS lowered to 1200V)    (+changed in OFC / Mphys/Mcal)   [main change for EMPBS]
   runs 212619-212742   (14 EMBPS lines lowered to 800V, all EMECPS lowered to 1600V)  [main change for EMECPS]
   runs 212809-216432   ( 6 EMBPS lines lowered to 800V, 7 EMECPS lines lowered to 1200V)


  The 1200->1600 and 800->1600 HV scale factors for the >211670 runs are derived from data
  For run<211670 an extra small correction is applied on top to account for the change in OFC / MphysMcal computation
  at run 211670 (using drift time for HV=1200V instead of old drift time for HV=2000V)

*/

class corr_HV_EMBPS {

 public:
   /** @brief constructor (initialization done there reading a root file for the HV maps per period */
   corr_HV_EMBPS();
   ~corr_HV_EMBPS();
  
   /** @brief get correction factor to apply to raw EMBPS energy : corrected raw EMBPS energy = correction factor * raw uncorrected EMBPS energy */
   /** @brief inputs: run number, eta and phi */
   float getCorr(int run, float eta, float phi) const;

  private:

   // response factor as computed in reconstruction
   //   (E measured = E nominalHV * response factor(HV)
   float getRecoCorrection(float hv,float eta) const;
   // functions to computed scale factor as in reconstruction
   float Respo(float e, float e_nominal,float tempe) const;
   float InvCharge(float e) const;
   float vdrift(float e, float tempe) const;

   // extra response factor derive from latest data 
   //   (E measured data) = (E corrected by reconstruction)* extra response factor (HV)  for run >=211670
   float getDataCorrection(float hv,float eta) const;

   // change to extra response factor to apply to data before run 211670 for HV<1600 to account for the fact that
   // OFC and Mphys/Mcal where changed at run 211670 to be computed with HV=1200 instead of HV=2000
   float getExtraScaleFactor() const;

   // store HV corrections for 6 periods, and 2 sides
   // each histogram has 8 bins in eta (deltaEta=0.4) and 32 bins in phi (2pi/32)
   TProfile2D* hHV[6][2];
   TFile* m_file;

};
#endif
