#ifndef CORR_HV_EMECPS_H
#define CORR_HV_EMECPS_H
#include "TH2D.h"
#include "TFile.h"

/**
  @class corr_HV_EMECPS
  @brief Correction for wrong HV EMEC presampler correction for 20.7 processed data (2015,2016)

  Perform correction of E0_RAW (EMEC PS) as function of run number and phi

*/

class corr_HV_EMECPS {

 public:
   /** @brief constructor (initialization done there reading a root file for the HV maps per period */
   corr_HV_EMECPS();
   ~corr_HV_EMECPS();
  
  /** @brief get correction factor to apply to raw EMECPS energy : corrected raw EMECPS energy = correction factor * raw uncorrected EMECPS energy */
   /** @brief inputs: run number, eta and phi */
   float getCorr(int run, float eta, float phi) const;

  private:

   // store HV corrections  in 2D hist (phi-runnumber) for EMECA and EMECC separately
   TH2D* m_HV[2];
   TFile* m_file;

};
#endif
