/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IDPVM_GetMeanWidth_h
#define IDPVM_GetMeanWidth_h
/**
 * @file GetMeanWidth.h
 * @author Max Baugh, Liza Mijovic, Nora Pettersson
 **/




#include <vector>
#include <string>
#include <utility>

class TH1;
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
    
    /// methods acc to which mean&RMS can be evaluated
    enum methods { iterRMS_convergence, Gauss_fit, fusion_iterRMS_Gaussfit };
    
    // wrapper to set mean,rms,and fraction of events in tails
    // nb: some of the methods are allowed to modify input histogram
    void setResults(TH1* p_input_hist, methods p_method);

    // results getters
    double getMean() { return m_mean; };
    double getMeanError() { return m_meanError; };
    double getRMS() { return m_RMS; };
    double getRMSError() { return m_RMSError; };

    
    //=================================================================================
    // helpers for catching all that can go wrong:
    //--------------------------------------
    // a) problems with iterative method
    //--------------------------------------
    /** fraction of events that is within the range of input histogram,
     * but goes out-of range during width and mean evaluation
     * if this is large, you should refine the method for width and mean evaluation
     * can be done during post-processing, does not require new InDetPVM run 
     **/
    double getFracOut() { return m_FracOut; };
    double getFracOutUnc() { return m_FracOutUnc; };
  
    //--------------------------------------
    // b) problems with input histograms
    //--------------------------------------
    /** fraction of events in under- and over-flow bins of input histogram
     * if this is large, the input histogram range needs to be increased
     * requires InDetPVM code-change and re-run 
     **/
    double getFracUOflow() { return m_FracUOflow; };
    /** helper to report bin and large fraction of under-/over- flow events,
     * ... as accumulated by top-level pull or resolution histogram 
     **/
    std::string reportUOBinVal(std::string p_histName, std::vector< std::pair<unsigned int,double> > p_vecBinVal);

    /** Too few bins covering RMS
     * requires InDetPVM code-change and re-run 
     **/
    double calcBinsWithinRes(TH1* p_input_hist, double p_RMS, double p_mean);
    double getBinsWithinRes(){ return m_binsWithinRes; };
    //  helper to report bin and too small number of bins covering resolution
    //  ... as accumulateed by top-level pull or resolution histogram
    std::string reportBinsWithinRes(std::string p_histName, std::vector< std::pair<unsigned int, double> > p_vecBinNRes);

    //=================================================================================



    ///return accumulated messages
    //@{
    std::vector<std::string> getDebugs() { return m_debugs; };
    std::vector<std::string> getInfos() { return m_infos; };
    std::vector<std::string> getWarnings() { return m_warnings; };
    std::vector<std::string> getErrors() { return m_errors; };
    //@}
  private:
    // use gaussian fit, return 0 in case of successful fit
    int setGaussFit(TH1* p_input_hist);
    /// iteratively change histogram range, until convergence
    /// return # remaining iterations before hitting the max. allowed
    int setIterativeConvergence(TH1* p_input_hist);
    /// evaluate the fraction of evens out of signal region and its uncertainty
    void setFout(double p_nsig,double p_ntot);    
    /// set large mean and RMS errors in case we eg. exclude too many events during evaluation
    void setLargeError();   
    /// helper to fill-in starting values of the results vector
    bool initialize(TH1* p_input_hist);


    /// results/outputs:
    //@{
    double m_mean,m_meanError,m_RMS,m_RMSError,m_FracOut,m_FracOutUnc,m_FracUOflow;
    std::vector<std::string> m_debugs;
    std::vector<std::string> m_infos;
    std::vector<std::string> m_warnings;
    std::vector<std::string> m_errors;
    //@}
    
    //helpers 
    std::string m_inHistName;
    // increase mean and RMS errors by this factor in case of ambiguous evaluation
    // ... eg in case we eg. exclude too many events during evaluation
    double m_largeErrorFact;
    // maximum fraction of Under- and Overflow events we tolerate
    double m_maxUOflowFrac;
    // actual and minimum required number of bins within resolution
    double m_binsWithinRes, m_minBinsWithinRes;

  };
} // end of namespace
#endif
