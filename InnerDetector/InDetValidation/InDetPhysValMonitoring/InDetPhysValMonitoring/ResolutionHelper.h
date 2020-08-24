/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IDPVM_ResolutionHelper_h
#define IDPVM_ResolutionHelper_h
/**
 * @file ResolutionHelper.h
 * @author Max Baugh, Liza Mijovic, Nora Pettersson
 **/

#include <vector>
#include <string>
#include <utility>

#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "CxxUtils/checker_macros.h"

class TH1;
class TH2;
class TH1D;
/*
  Helper class to evaluate mean and RMS using dedicated methods
  useful eg for histograms with long tails etc.
*/
namespace IDPVM {
  
  class ResolutionHelper {
  public:
    ResolutionHelper();
    ~ResolutionHelper() {
      // nop
    };
    
    /// methods acc to which mean&RMS can be evaluated
    enum methods { iterRMS_convergence, Gauss_fit, fusion_iterRMS_Gaussfit };

    /// package the output of a single bin resolution measurement
    struct resolutionResultInBin{
      double width{0.};     ///< the residual / pull width (either RMS or fit outcome)
      double widthError{0.};  ///< uncertainty on the residual / pull width
      double mean{0.};    ///< mean of the residual / pull
      double meanError{0.};  ///< uncertainty on the mean 
      double outlierFrac{0.};   ///<fraction of the population in outlier bins. Used to detect possible binning issues and warn the user 
      double outlierFracError{0.};  ///< uncertainty on the outlier fraction. Mainly for book-keeping at this time. 
    };

    /// extract 1D resolution plots from a 2D "residual vs observable" histogram. 
    /// @param[in] h_input2D: input histogram (x = observable, y = residuals) 
    /// @param[out] hwidth: output histogram, will be filled with the width of the residuals (-> resolution) 
    /// @param[out] hmean: output histogram, will be filled with the mean of the residuals (-> bias) 
    /// @param[out] hproj: array of histograms, will be populated by an array of 1-dimensional projections of Y within bins in X 
    /// @param[in] saveProjections: steers the filling of hpoj
    /// @param[in] theMethod: the method used to extract the width and mean. Default is iterative RMS.  
    void makeResolutions(const TH2* h_input2D, TH1* hwidth, TH1* hmean, TH1* hproj[], bool saveProjections, IDPVM::ResolutionHelper::methods theMethod=IDPVM::ResolutionHelper::iterRMS_convergence);
    
    /// extract 1D resolution plots from a 2D "residual vs observable" histogram. 
    /// @param[in] h_input2D: input histogram (x = observable, y = residuals) 
    /// @param[out] hwidth: output histogram, will be filled with the width of the residuals (-> resolution) 
    /// @param[out] hmean: output histogram, will be filled with the mean of the residuals (-> bias) 
    /// @param[out] hproj: array of histograms, will be populated by an array of 1-dimensional projections of Y within bins in X 
    /// @param[in] saveProjections: steers the filling of hpoj
    /// @param[in] theMethod: the method used to extract the width and mean. Default is iterative RMS.  
    void makeResolutions(const TH2* h_input2D, TH1* hwidth, TH1* hmean, IDPVM::ResolutionHelper::methods theMethod=IDPVM::ResolutionHelper::iterRMS_convergence);

    /// single-bin resolution evaluation, also internally used by makeResolutions
    /// @param p_input_hist: input histogram, 1D distribution of the residual  
    /// @param theMethod: method to use (default: iterative RMS)
    /// @return resolutionResultInBin object packaging the mean and width of the distribution determined using the specified method as well as outlier bin fractions. 
    resolutionResultInBin ResolutionHelperResultsModUnits(TH1D* p_input_hist, IDPVM::ResolutionHelper::methods theMethod=IDPVM::ResolutionHelper::iterRMS_convergence);

    
    /// wrapper to set mean,rms,and fraction of events in tails
    /// nb: some of the methods are allowed to modify input histogram
    void setResults(TH1* p_input_hist, methods p_method);

    /// results getters
    double getMean() { return m_mean; };
    double getMeanError() { return m_meanError; };
    double getRMS() { return m_RMS; };
    double getRMSError() { return m_RMSError; };
    /** fraction of events that is within the range of input histogram,
     * but goes out-of range during width and mean evaluation
     * if this is large, you should refine the method for width and mean evaluation
     * can be done during post-processing, does not require new InDetPVM run 
     **/
    double getFracOut() { return m_FracOut; };
    double getFracOutUnc() { return m_FracOutUnc; };
    /** fraction of events in under- and over-flow bins of input histogram
     * if this is large, the input histogram range needs to be increased
     * requires InDetPVM code-change and re-run 
     **/
    double getFracUOflow() { return m_FracUOflow; };
    /** helper to report bin and fraction of under-/over- flow events,
     * as accumulated by top-level pull or resolution histogram 
     **/
    std::string reportUOBinVal(std::string p_histName, std::vector< std::pair<unsigned int,double> > p_vecBinVal);
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
    double m_mean{0.};
    double m_meanError{0.};
    double m_RMS{0.};
    double m_RMSError{0.};
    double m_FracOut{0.};
    double m_FracOutUnc{0.};
    double m_FracUOflow{0.};

    std::vector<std::string> m_debugs;
    std::vector<std::string> m_infos;
    std::vector<std::string> m_warnings;
    std::vector<std::string> m_errors;
    void cloneHistogram(TH1D* h, TH1* hcopy);
    //@}
    
    //helpers 
    std::string m_inHistName{""};
    // increase mean and RMS errors by this factor in case of ambiguous evaluation
    // ... eg in case we eg. exclude too many events during evaluation
    double m_largeErrorFact{10.};
    // maximum fraction of Under- and Overflow events we tolerate
    double m_maxUOflowFrac{0.05};



    /// Log a message using the Athena controlled logging system
    MsgStream&
    msg(MSG::Level lvl) const {
      return m_msg.get() << lvl;
    }

    /// Check whether the logging system is active at the provided verbosity level
    bool
    msgLvl(MSG::Level lvl) {
      return m_msg.get().level() <= lvl;
    }

    /// Private message stream member
    mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;


  };
} // end of namespace
#endif
