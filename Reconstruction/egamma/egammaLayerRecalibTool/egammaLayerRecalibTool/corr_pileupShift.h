#ifndef CORR_PILEUPHIFT_H
#define CORR_PILEUPHIFT_H
#include "TH1D.h"
#include "TFile.h"


/**
  @class corr_pileupShift
  @brief Correction for pileup induced energy shit as function of mu per layer for 2016 data
  @author: Guillaume Unal
*/

class corr_pileupShift {

 public:
   /** @brief constructor (initialization done there reading a root file for the HV maps per period */
   corr_pileupShift();
   ~corr_pileupShift();
  
   /** @brief get shift to subtract to raw layer energy :  corrected layer energy = raw energy - shift  */
   /** @brief inputs: layer (0 to 3), run (run number), mu (average number of interactions per crossing) and eta */
   float getCorr(int layer, int run, float mu, float eta) const;

  private:
 
   // run range to apply the correction: 2016 data 296939 to 311563
   int m_runMin, m_runMax;

   // store corrections/mu  vs eta per layer
   TH1D* m_haverage[4];
   TFile* m_file;

};
#endif
