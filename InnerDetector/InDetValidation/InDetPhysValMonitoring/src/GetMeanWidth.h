/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IDPVM_GetMeanWidth_h
#define IDPVM_GetMeanWidth_h
/**
 * @file GetMeanWidth.h
 * @author Max Baugh, Liza Mijovic, Nora Pettersson
 **/

#include <iostream>
#include <vector>

#include "TH1.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TMath.h"

using std::string;
using std::vector;

/*
  Helper class to evaluate mean and RMS using dedicated methods
  useful eg for histograms with long tails etc.
*/
namespace IDPVM {
  class GetMeanWidth {
  public:
    GetMeanWidth();
    ~GetMeanWidth(){};
    
    // methods acc to which mean&RMS can be evaluated
    enum m_methods { iterative_RMS_convergence, Gauss_fit, fusion_iterRMS_Gaussfit };
    
    // wrapper: 
    // nb: some of the methods are allowed to modify input histogram
    // p_result will be filled as
    // <mean,mean_error,RMS,RMS_error>
    // +<fOut,foutUnc>, where fOut is a fraction of events out of signal region
    //                  and foutUnc its approximate uncertainty
    void getResults(TH1* p_input_hist, vector<float>& p_result, m_methods p_method);

    // return and clear accumulated warnings and errors
    string flushWarning();
    string flushError();
    
  private:
    // use gaussian fit, return 0 in case of successful fit
    int getGaussFit(TH1* p_input_hist, vector<float>& p_result);
    // iteratively change histogram range, until convergence
    // return # remaining iterations before hitting the max. allowed
    int getIterativeConvergence(TH1* p_input_hist, vector<float>& p_result);
    // evaluate the fraction of evens out of signal region and its uncertainty
    void getFout(double p_nsig,double p_ntot,double& p_resFOut, double& p_resUncOut);
    // helper to fill-in starting values of the results vector
    void setStartResult(TH1* p_input_hist, vector<float>& p_result);
      
    string m_warning;
    string m_error;
  };
}
#endif
