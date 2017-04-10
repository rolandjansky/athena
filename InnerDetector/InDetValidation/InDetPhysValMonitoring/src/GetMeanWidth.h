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
    ~GetMeanWidth() {
      // nop
    };
    
    // methods acc to which mean&RMS can be evaluated
    enum methods { iterRMS_convergence, Gauss_fit, fusion_iterRMS_Gaussfit };
    
    // wrapper to set mean,rms,and fraction of events in tails
    // nb: some of the methods are allowed to modify input histogram
    void setResults(TH1* p_input_hist, methods p_method);

    // results getters
    double getMean() { return m_mean; };
    double getMeanError() { return m_meanError; };
    double getRMS() { return m_RMS; };
    double getRMSError() { return m_RMSError; };
    double getFracOut() { return m_FracOut; };
    double getFracOutUnc() { return m_FracOutUnc; };
    // return and clear accumulated warnings and errors
    vector<string> getInfos() { return m_infos; };
    vector<string> getWarnings() { return m_warnings; };
    vector<string> getErrors() { return m_errors; };
    
  private:
    // use gaussian fit, return 0 in case of successful fit
    int setGaussFit(TH1* p_input_hist);
    // iteratively change histogram range, until convergence
    // return # remaining iterations before hitting the max. allowed
    int setIterativeConvergence(TH1* p_input_hist);
    // evaluate the fraction of evens out of signal region and its uncertainty
    void setFout(double p_nsig,double p_ntot);
    // helper to fill-in starting values of the results vector
    bool initialize(TH1* p_input_hist);


    // results/outputs:
    double m_mean,m_meanError,m_RMS,m_RMSError,m_FracOut,m_FracOutUnc;  
    vector<string> m_infos;
    vector<string> m_warnings;
    vector<string> m_errors;
    
    //helpers 
    string m_inHistName;   
  };
}
#endif
