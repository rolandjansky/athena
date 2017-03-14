/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file GetMeanWidth.h
 * @author Max Baugh, Liza Mijovic, Nora Pettersson
 **/

#include "GetMeanWidth.h"

namespace IDPVM {
  
  GetMeanWidth::GetMeanWidth() :
    m_mean(0.),
    m_meanError(0.),
    m_RMS(0.),
    m_RMSError(0.),
    m_FracOut(0.),
    m_FracOutUnc(0.),
    m_inHistName("") {
    //nop
  }
  
  bool GetMeanWidth::initialize(TH1* p_input_hist) {

    if ( !m_infos.empty())
      m_infos.clear();      
    if ( !m_warnings.empty())
      m_warnings.clear();
    if ( !m_errors.empty())
      m_errors.clear();
       
    if (!p_input_hist) {
      m_errors.push_back("GetMeanWidth::initialize: empty input histogram passed: ");
       m_mean=m_meanError=m_RMS=m_RMSError=m_FracOut=m_FracOutUnc=0.;
      return false;
    }
    
    m_inHistName = p_input_hist->GetName();
    
    if ( 0==p_input_hist->GetEntries() ) {
      m_infos.push_back("GetMeanWidth::initialize: got input histogram with 0 entries: "+ m_inHistName);
      m_mean=m_meanError=m_RMS=m_RMSError=m_FracOut=m_FracOutUnc=0.;
      return false;
    }

    m_RMS = p_input_hist->GetRMS();
    m_RMSError = p_input_hist->GetRMSError();
    m_mean = p_input_hist->GetMean();
    m_meanError = p_input_hist->GetMeanError();   
    m_FracOut = 0.;
    m_FracOutUnc = 0.;
    
    return true;
  }
  
  void GetMeanWidth::setFout(double p_nsig,double p_ntot) {
    m_FracOut=0.;
    m_FracOutUnc=0.;
    if (p_ntot>0.) {
      double nout=p_ntot-p_nsig;
      if (nout / p_ntot > 0.9) {
	nout = 0.;
      }
      if (nout > 0.) {
	m_FracOut=nout/p_nsig;
	m_FracOutUnc=(nout / p_ntot) * TMath::Sqrt(1. / nout + 1. / p_ntot);
      }
    }
    return;
  }
  
  int GetMeanWidth::setGaussFit(TH1* p_input_hist){

    int ret=0;

    // evaluate mean and with via the Gauss fit
    TFitResultPtr r = p_input_hist->Fit("gaus", "QS0");
    if ( r.Get() and (0 == (r->Status()%1000)) ) {
      m_RMS=r->Parameter(2);//RMS
      m_RMSError=r->ParError(2);//RMS_error
      m_mean=r->Parameter(1);//mean
      m_meanError=r->ParError(1);//mean_error
    }
    else
      ret=1;

    // get fraction of events outside 3*RMS + its ~ uncertainty
    double nSig = p_input_hist->Integral(p_input_hist->GetXaxis()->FindBin(-3.0 * m_RMS),
					 p_input_hist->GetXaxis()->FindBin(3.0 * m_RMS));
    double nTot = p_input_hist->Integral();
    setFout(nSig,nTot);

    // return fit status
    return ret;
  }
  
  // return # remaining iterations before hitting the max. allowed
  int GetMeanWidth::setIterativeConvergence(TH1* p_input_hist) {
    
    // evaluate  mean and RMS using iterative converfgence:
    double mean=p_input_hist->GetMean();
    double RMS = p_input_hist->GetRMS();
    
    // iteration parameters:
    // max iteration steps
    unsigned int ntries_max = 100;
    // width of cutting range in [RMS]
    double nRMS_width = 3.0;
    
    // iteration counters and helpers: 
    // min and max range of the histogram:
    double min=0.;
    double max=0.;
    // min and max bins of the histogram in previous iteration
    // 0-th iteration: range of the original histogram
    int binmin_was = 1;
    int binmax_was = p_input_hist->GetNbinsX();
    // initial number of iteration steps
    unsigned int ntries = 0;
    
    // iteratively cut tails untill the RMS gets stable about mean
    // RMS stable: when input histogram range after cutting by 
    // +- 3*RMS is same as the range before cutting
    while ( ntries<ntries_max ) {
      ++ntries;
      RMS = p_input_hist->GetRMS();
      mean = p_input_hist->GetMean();
      min = -1.0*nRMS_width*RMS + mean;
      max = nRMS_width*RMS + mean;
      // find bins corresponding to new range
      int binmin=p_input_hist->GetXaxis()->FindFixBin(min);
      int binmax=p_input_hist->GetXaxis()->FindFixBin(max);
      // end iteration if these are same bins as in prev. iteration
      if ( binmin_was==binmin && binmax_was==binmax ) {
	break;
      }
      else {
	// else set the new range and continue iteration 
	p_input_hist->GetXaxis()->SetRange(binmin,binmax);
	binmin_was=binmin;
	binmax_was=binmax;
      }
    } // end of ( ntries<ntries_max ) ; iterative convergence loop
    
    // set the iteration results that are accessible to clients: 
    m_RMS=RMS;
    m_RMSError= p_input_hist->GetRMSError();    
    m_mean=mean;
    m_meanError=p_input_hist->GetMeanError();
    
    // get fraction of excluded events + its ~ uncertainty
    double nSig = p_input_hist->Integral(p_input_hist->GetXaxis()->FindBin(min),
					 p_input_hist->GetXaxis()->FindBin(max));
    double nTot = p_input_hist->Integral();
    setFout(nSig,nTot);
    
    // return number of remaining allowed iteration steps
    return(ntries_max-ntries);
  }

  // evaluate results for input histogram
  // * mean and RMS and fraction of events in tails
  // * call one of alternative ways of evaluation
  void GetMeanWidth::setResults(TH1* p_input_hist, methods p_method){

    // set start values of the results vector
    // mean RMS etc as in the input histo prior to trimming
    if (!initialize(p_input_hist))
      return;
    
    if (iterRMS_convergence == p_method) {
      if ( !setIterativeConvergence(p_input_hist) ) {
	m_warnings.push_back("GetMeanWidth::setIterativeConvergence did not converge for "+ m_inHistName);
      }
      return;
    }
    else if (Gauss_fit == p_method) {
      if ( !setGaussFit(p_input_hist) ) {
	m_warnings.push_back("GetMeanWidth::setGaussFit: fit failed for "+ m_inHistName);
      }
      return;
    }
    else if (fusion_iterRMS_Gaussfit == p_method) {
      if ( !setIterativeConvergence(p_input_hist) &&
	   !setGaussFit(p_input_hist) ) {
	m_warnings.push_back("GetMeanWidth::fusion_iterRMS_Gaussfit both methods failed for "+ m_inHistName);
      }
      return;
    }
    else {
      m_errors.push_back("GetMeanWidth::setResults: method not supported. No evaluation for "+ m_inHistName);
      return;
    }

    // should never come here
    return;
  }

}//end of namespace
