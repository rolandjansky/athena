/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file GetMeanWidth.h
 * @author Max Baugh, Liza Mijovic, Nora Pettersson
 **/

#include "GetMeanWidth.h"
#include "TH1.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace IDPVM {
  
  GetMeanWidth::GetMeanWidth() :
    m_mean(0.),
    m_meanError(0.),
    m_RMS(0.),
    m_RMSError(0.),
    m_FracOut(0.),
    m_FracOutUnc(0.),
    m_FracUOflow(0.),
    m_inHistName(""),
    m_largeErrorFact(10.),
    m_maxUOflowFrac(0.001), 
    m_binsWithinRes(0.),
    m_minBinsWithinRes(3.) {
    //nop
  }
  
  bool GetMeanWidth::initialize(TH1* p_input_hist) {

    if ( !m_debugs.empty())
      m_debugs.clear();     
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
      m_debugs.push_back("GetMeanWidth::initialize: got input histogram with 0 entries: "+ m_inHistName);
      m_mean=m_meanError=m_RMS=m_RMSError=m_FracOut=m_FracOutUnc=0.;
      return false;
    }

    m_RMS = p_input_hist->GetRMS();
    m_RMSError = p_input_hist->GetRMSError();
    m_mean = p_input_hist->GetMean();
    m_meanError = p_input_hist->GetMeanError();   
    m_FracOut = 0.;
    m_FracOutUnc = 0.;
    double nExclUOflow = p_input_hist->Integral(1,p_input_hist->GetNbinsX());
    double nInclUOflow = p_input_hist->Integral(0,p_input_hist->GetNbinsX()+1);
    double fUOflow = (nExclUOflow>0.) ? (nInclUOflow-nExclUOflow)/nExclUOflow : -1.;
    m_FracUOflow = (fUOflow>m_maxUOflowFrac) ? fUOflow : -1;
    
    return true;
  }

  void GetMeanWidth::setLargeError() {
    std::ostringstream debugl;
    debugl << __FILE__ << "\t\t" << m_inHistName
	   << ": scaling mean and RMS errors by factor: " << m_largeErrorFact;
    m_debugs.push_back(debugl.str());
    m_meanError*=m_largeErrorFact;
    m_RMSError*=m_largeErrorFact;
  }

  std::string GetMeanWidth::reportUOBinVal(std::string p_histName, std::vector< std::pair<unsigned int,double> > p_vecBinVal) {
    std::ostringstream reportl;
    if (!p_vecBinVal.empty())  {
      reportl << "Errors scaled up for resol. hist. with more than " << m_maxUOflowFrac*100. << "% of events in over- and under-flow: "
	      << p_histName<<": ";
      for ( auto it : p_vecBinVal )
	reportl << "bin"<<it.first << ": " << std::setprecision(2) << it.second*100. << "%, ";
    }
    return reportl.str();
  }


  std::string GetMeanWidth::reportBinsWithinRes(std::string p_histName, std::vector< std::pair<unsigned int, double> > p_vecBinNRes) {
    std::ostringstream reportl;
    if (!p_vecBinNRes.empty())  {
      reportl << "Errors scaled up for resol. hist. with # bins covering RMS less than " << m_minBinsWithinRes <<": " 
              << p_histName<<": ";
      for ( auto it : p_vecBinNRes )
        reportl << "bin"<<it.first << ": " << std::setprecision(2) << it.second << " , ";
    }
    return reportl.str();    
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
	m_FracOutUnc=(nout / p_ntot) * std::sqrt(1. / nout + 1. / p_ntot);
      }
    }
    return;
  }
  
  double GetMeanWidth::calcBinsWithinRes(TH1* p_input_hist,double p_RMS, double p_mean) {

    // 0 or 1 entry hisrograms: RMS && RMSError is set to 0 by ROOT
    if (2 > p_input_hist->GetEntries()) {
      return(-1.);
    }
    
    // get # bins within +- RMS of mean
    unsigned int _bhigh = p_input_hist->GetXaxis()->FindBin(p_mean+p_RMS);
    unsigned int _blow = p_input_hist->GetXaxis()->FindBin(p_mean-p_RMS);
    double _nbins = _bhigh - _blow;
   
    // return # of bins covering RMS
    return(_nbins/2.);
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
    double nTot = p_input_hist->Integral(1,p_input_hist->GetNbinsX());
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
    double xmin=0.;
    double xmax=0.;
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
      xmin = -1.0*nRMS_width*RMS + mean;
      xmax = nRMS_width*RMS + mean;
      // find bins corresponding to new range, disregard underflow
      int binmin=std::max(1,p_input_hist->GetXaxis()->FindFixBin(xmin));
      // find bins corresponding to new range, disregard overflow
      int binmax=std::min(p_input_hist->GetNbinsX(),p_input_hist->GetXaxis()->FindFixBin(xmax));
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
    double nSig = p_input_hist->Integral(p_input_hist->GetXaxis()->FindBin(xmin),
					 p_input_hist->GetXaxis()->FindBin(xmax));
    // disregard under- and over- flow
    double nTot = p_input_hist->Integral(1,p_input_hist->GetNbinsX());
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
      if ( !setIterativeConvergence(p_input_hist) ) 
	m_warnings.push_back("\t\t\t* GetMeanWidth::setIterativeConvergence did not converge for "+ m_inHistName);
    }
    else if (Gauss_fit == p_method) {
      if ( !setGaussFit(p_input_hist) ) 
	m_warnings.push_back("\t\t\t* GetMeanWidth::setGaussFit: fit failed for "+ m_inHistName);
    }
    else if (fusion_iterRMS_Gaussfit == p_method) {
      if ( !setIterativeConvergence(p_input_hist) &&
	   !setGaussFit(p_input_hist) ) 
	m_warnings.push_back("\t\t\t* GetMeanWidth::fusion_iterRMS_Gaussfit both methods failed for "+ m_inHistName);
    }
    else {
      m_errors.push_back("\t\t\t* GetMeanWidth::setResults: method not supported. No evaluation for "+ m_inHistName);
    }

    // check if large fraction of events was in over- and under-flow
    if ( m_FracUOflow > 0. ) {
      std::ostringstream debugl;
      debugl << "\tGetMeanWidth::setResults: too large fraction of out-of-range events for histogram ";
      debugl << m_inHistName << ": " << m_FracUOflow << " > " << m_maxUOflowFrac;
      m_debugs.push_back(debugl.str());      
      setLargeError();
      m_debugs.push_back("\t\t\t* GetMeanWidth::setResults: scaling errors up for "
			 +m_inHistName+". Too many under- and over- flows.");
    }

    m_binsWithinRes = calcBinsWithinRes(p_input_hist, m_RMS, m_mean);    
    if ( 0.<m_binsWithinRes && m_binsWithinRes<m_minBinsWithinRes ) {
      std::ostringstream debugl;
      debugl << "\tGetMeanWidth::setResults: too few bins covering RMS in histogram ";
      debugl << m_inHistName << ": "<< m_binsWithinRes << " bins seen, min " <<  m_minBinsWithinRes << " bins required " << std::endl;
      m_debugs.push_back(debugl.str());
      setLargeError();
      m_debugs.push_back("\t\t\t* GetMeanWidth::setResults: scaling errors up for "
			 +m_inHistName+". Too few bins covering RMS.");
    }
    else {
      // set to dummy value, to suppress printout of warnings
      m_binsWithinRes = -1.;
    }
    // reset range metadata to state prior to iteration
    // (this gets changed in iterative or fusion)
    p_input_hist->GetXaxis()->SetRange(1,p_input_hist->GetNbinsX());
    
    return;
  }
}//end of namespace
