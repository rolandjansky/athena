/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file GetMeanWidth.h
 * @author Max Baugh, Liza Mijovic, Nora Pettersson
 **/

#include "GetMeanWidth.h"

namespace IDPVM {
  
  GetMeanWidth::GetMeanWidth() {
    m_warning="";
    m_error="";
  }

  void GetMeanWidth::setStartResult(TH1* p_input_hist, vector<float>& p_result) {
    if ( !p_result.empty() )
      p_result.clear();
	
    double mean = p_input_hist->GetMean();
    double mean_error = p_input_hist->GetMeanError();
    double RMS = p_input_hist->GetRMS();
    double RMS_error = p_input_hist->GetRMSError();
    double fout = 0.;
    double ffout = 0.;
    
    p_result.push_back(mean);
    p_result.push_back(mean_error);
    p_result.push_back(RMS);
    p_result.push_back(RMS_error);
    p_result.push_back(fout);
    p_result.push_back(ffout);
    
    return;
  }
  
  void GetMeanWidth::getFout(double p_nsig,double p_ntot,double& p_resFOut, double& p_resUncOut) {
    p_resFOut=0.;
    p_resUncOut=0.;
    if (p_ntot>0.) {
      double nout=p_ntot-p_nsig;
      if (nout / p_ntot > 0.9) {
	nout = 0.;
      }
      if (nout > 0.) {
	p_resFOut=nout/p_nsig;
	p_resUncOut=(nout / p_ntot) * TMath::Sqrt(1. / nout + 1. / p_ntot);
      }
    }
    return;
  }
  
  int GetMeanWidth::getGaussFit(TH1* p_input_hist, vector<float>& p_result){

    int ret=0;
    // fill in default results values to be results vector
    // mean RMS etc as in the input histo to be returned if fit fails
    setStartResult(p_input_hist,p_result);

    // evaluate mean and with via the Gauss fit
    TFitResultPtr r = p_input_hist->Fit("gaus", "QS0");
    if ( r.Get() and (0 == (r->Status()%1000)) ) {
      p_result[0]=r->Parameter(1);//mean
      p_result[1]=r->ParError(1);//mean_error
      p_result[2]=r->Parameter(2);//RMS
      p_result[3]=r->ParError(2);//RMS_error
    }
    else
      ret=1;

    // get fraction of events outside 3*RMS + its ~ uncertainty
    double RMS=p_result[2];
    double nSig = p_input_hist->Integral(p_input_hist->GetXaxis()->FindBin(-3.0 * RMS),
					 p_input_hist->GetXaxis()->FindBin(3.0 * RMS));
    double nTot = p_input_hist->Integral();
    double fOut=0.;
    double uncOut=0.;
    getFout(nSig,nTot,fOut,uncOut);
      
    // set Fout values in the results vector
    p_result[4]=fOut;
    p_result[5]=uncOut;

    // return fit status
    return ret;
  }

  // return # remaining iterations before hitting the max. allowed
  int GetMeanWidth::getIterativeConvergence(TH1* p_input_hist, vector<float>& p_result) {

    // set start values of the results vector
    // mean RMS etc as in the input histo prior to trimming
    setStartResult(p_input_hist,p_result);

    // evalueate  mean and RMS using iterative converfgence:
    double mean=p_input_hist->GetMean();
    double RMS = p_input_hist->GetRMS();
    
    // iteration parameters
    double itr_RMS = RMS + 1.0;
    unsigned int ntries = 0;
    unsigned int ntries_max = 100;
    double RMS_mindiff = 0.001;
    double nRMS_width = 3.0;

    // min and max range of the histogram:
    double min=0.;
    double max=0.;
    
    // iteratively cut tails untill the RMS gets stable about mean
    while ( (fabs(itr_RMS - RMS)>RMS_mindiff) && (ntries<ntries_max) ) {
      RMS = p_input_hist->GetRMS();
      min = -1.0*nRMS_width*RMS + mean;
      max = nRMS_width*RMS + mean;
      if (min < p_input_hist->GetBinLowEdge(1)) 
        min = p_input_hist->GetBinLowEdge(1);      
      if (max > p_input_hist->GetBinCenter(p_input_hist->GetNbinsX()))
        max = p_input_hist->GetBinCenter(p_input_hist->GetNbinsX());
      p_input_hist->SetAxisRange(min, max);
      mean = p_input_hist->GetMean();
      itr_RMS = p_input_hist->GetRMS();
      ++ntries;  
    }
    
    RMS = itr_RMS;
    double mean_error = p_input_hist->GetMeanError();
    double RMS_error = p_input_hist->GetRMSError();
    // set the results vector for mean and width
    p_result[0]=mean;
    p_result[1]=mean_error;
    p_result[2]=RMS;
    p_result[3]=RMS_error;   

    // get fraction of excluded events + its ~ uncertainty
    double nSig = p_input_hist->Integral(p_input_hist->GetXaxis()->FindBin(min),
					 p_input_hist->GetXaxis()->FindBin(max));
    double nTot = p_input_hist->Integral();
    double fOut=0.;
    double uncOut=0.;
    getFout(nSig,nTot,fOut,uncOut);
    p_result[4]=fOut;
    p_result[5]=uncOut;
    
    // return number of remaining allowed iteration steps
    return(ntries_max-ntries);
  }

  
  // evaluate mean and RMS for input histogram:
  // * call one of alternative ways of evaluation
  // * return <mean, mean_error, RMS, RMS_error,fout,ffout>
  // * here fout and ffout are measures of how many events were excluded from the mean/RMS
  //        <---- for consistency with old code/agreemenent, but we never actually use these...
  //        -----> TODO: request removal or use-case
  void GetMeanWidth::getResults(TH1* p_input_hist, vector<float>& p_result, m_methods p_method){
    
    if ( !p_result.empty() )
      p_result.clear();
    
    if (!p_input_hist) {
      m_error+="\nGetMeanWidth::getResults: empty input histogram passed";
      return;
    }
    
    if ( 0==p_input_hist->GetEntries() ) {
      m_warning+="\nGetMeanWidth::getResults: got empty histogram";
      int i=0;
      while (i<4) {
	p_result.push_back(0.);
	++i;
      }
      return;
    }

    if (iterative_RMS_convergence == p_method) {
      if ( !getIterativeConvergence(p_input_hist, p_result) )
	m_warning+="\nGetMeanWidth::getIterativeConvergence did not converge";
    }
    else if (Gauss_fit == p_method) {
      if ( !getGaussFit(p_input_hist, p_result) )
	m_warning+="\nGetMeanWidth::getGaussFit: fit failed";
    }
    else if (fusion_iterRMS_Gaussfit == p_method) {
      if ( !getIterativeConvergence(p_input_hist, p_result) &&
	   !getGaussFit(p_input_hist, p_result) )
	m_warning+="\nGetMeanWidth::fusion_iterRMS_Gaussfit both methods failed";
    }
    else
      m_error+="\nGetMeanWidth::getResults: method not supported.";

    return;

  }
  
  string GetMeanWidth::flushWarning(){
    string buff_warn=m_warning;
    m_warning="";
    return buff_warn;
  }
  
  string GetMeanWidth::flushError(){
    string buff_err=m_error;
    m_error="";
    return buff_err;
  }
  
}//end of namespace
