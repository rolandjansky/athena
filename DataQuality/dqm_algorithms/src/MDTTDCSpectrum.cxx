/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MDTTDCSpectrum.cxx,v 1.0 2008/10/08 Valerio Consorti
// **********************************************************************

#include "dqm_algorithms/MDTTDCSpectrum.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TClass.h>
#include <TH1.h>
#include <TAxis.h>
#include <TF1.h>
//#include <TProfile.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"


static dqm_algorithms::MDTTDCSpectrum staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

MDTTDCSpectrum::MDTTDCSpectrum()
  : m_name("MDTTDCspectrum")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


MDTTDCSpectrum::~MDTTDCSpectrum()
{
}


dqm_core::Algorithm*
MDTTDCSpectrum::clone()
{
  return new MDTTDCSpectrum(*this);
}


dqm_core::Result*
MDTTDCSpectrum::execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
{
  TH1 * hist;
  TH1 * ref;
  
  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    hist = (TH1*)&object;
    if (hist->GetDimension() >= 2 ){
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension >= 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }   
  
  //Get Parameters and Thresholds
  double minstat;
  double chi2;
  double greenTh;
  double redTh;
  double max_shift;
  try {
    minstat      = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), -1);
    chi2         = dqm_algorithms::tools::GetFirstFromMap("chi2", config.getParameters(), -1);
    max_shift    = dqm_algorithms::tools::GetFirstFromMap("max_shift", config.getParameters(), 999999);
    redTh        = dqm_algorithms::tools::GetFromMap( "Limits", config.getRedThresholds());
    greenTh      = dqm_algorithms::tools::GetFromMap( "Limits", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  //Get Reference Histo
 
  try {
    ref = static_cast<TH1 *>( config.getReference() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
  }
  if (hist->GetDimension() != ref->GetDimension() ) {
    throw dqm_core::BadRefHist( ERS_HERE, name, "Reference VS histo: Different dimension!" );
  } 
    
 //Check of statistics
  if (hist->GetEntries() < minstat ) {
    ERS_INFO("Histogram does not satisfy MinStat requirement " <<hist->GetName());
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = hist->GetEntries();
    return result;
  }
  ERS_DEBUG(1,"Statistics: "<< hist->GetEntries()<< " entries ");
  
  
  //Algo
  Double_t chi2_per_NDF=999999999;
  
  TF1* fit_ref = (TF1*) ref->GetFunction("TimeSpectrum");
  TF1* fit_hist= (TF1*) hist->GetFunction("TimeSpectrum");
  
  if(fit_ref == 0){
    ERS_INFO("Missing reference fit for "<<hist->GetName());
    dqm_core::Result* result = new dqm_core::Result();
    result->tags_["Fit_parameter_NOT_found"] = -999.;
    result->status_ = dqm_core::Result::Undefined;
    return result;
  };
  if(fit_hist == 0){
    ERS_INFO("Missing fit for "<<hist->GetName());
    dqm_core::Result* result = new dqm_core::Result();
    result->tags_["Fit_parameter_NOT_found"] = -999.;
    result->status_ = dqm_core::Result::Undefined;
    return result;
  };
  
  if(fit_hist->GetNDF() !=0) chi2_per_NDF = (fit_hist->GetChisquare())/(fit_hist->GetNDF());
  
  //Check of chi2
  if (chi2_per_NDF > chi2 && chi2>0) {
    ERS_INFO("Histogram does not satisfy chi_square/NDF requirement: " <<hist->GetName());
    dqm_core::Result* result = new dqm_core::Result();
    result->tags_["Chi2_over_threshold"] = chi2_per_NDF;
    result->status_ = dqm_core::Result::Undefined;
    return result;
  }

  if(fit_hist->GetNDF() == 0){
    ERS_INFO("NDF not available for histogram " <<hist->GetName());
    dqm_core::Result* result = new dqm_core::Result();
    result->tags_["Fit_parameter_NDF"] = 0;
    result->status_ = dqm_core::Result::Undefined;
    return result;
  };
  
  Double_t t0_ref=0;
  Double_t t0_err_ref=0;
  Double_t tmax_ref=0;
  Double_t tmax_err_ref=0;

  if(fit_ref !=0) t0_ref = ((fit_ref->GetParameter(4))*0.78125);
  if(fit_ref !=0) t0_err_ref = ((fit_ref->GetParError(4))*0.78125);
  if(fit_ref !=0) tmax_ref = ((fit_ref->GetParameter(5))*0.78125);
  if(fit_ref !=0) tmax_err_ref = ((fit_ref->GetParError(5))*0.78125); 
  //Lower limit for t0_err_ref and tmax_err_ref
  if (t0_err_ref<1.) {
    t0_err_ref =1.;
  }
  if (tmax_err_ref<2.) {
    tmax_err_ref =2.;
  }

  Double_t t0=-99999.;
  Double_t t0_err=0;
  Double_t tmax=0;
  Double_t tmax_err=0;

  if(fit_hist !=0) t0 = ((fit_hist->GetParameter(4))*0.78125);
  if(fit_hist !=0) t0_err = ((fit_hist->GetParError(4))*0.78125);
  if(fit_hist !=0) tmax = ((fit_hist->GetParameter(5))*0.78125);
  if(fit_hist !=0) tmax_err = ((fit_hist->GetParError(5))*0.78125);
  //Lower limit for t0_err and tmax_err
  if (t0_err<1.) {
    t0_err =1.;
  }
  if (tmax_err<2.) {
    tmax_err =2.;
  }

  Double_t Dt0=fabs(t0-t0_ref);
  Double_t Dtd=fabs(tmax-t0-tmax_ref+t0_ref);

  Double_t sigma_Dt0=sqrt(t0_err*t0_err+t0_err_ref*t0_err_ref);
  Double_t sigma_Dtmax=sqrt(tmax_err*tmax_err+tmax_err_ref*tmax_err_ref);
  Double_t sigma_Dtd= sqrt(tmax_err*tmax_err+t0_err*t0_err+tmax_err_ref*tmax_err_ref+t0_err_ref*t0_err_ref);

  if( sigma_Dt0 ==0 || sigma_Dtmax==0 ){
    ERS_DEBUG(1,"ERROR: fit parameter errors not found!");
      dqm_core::Result* result = new dqm_core::Result();
      result->tags_["Fit_parameter_NOT_found"] = -999.;
      result->status_ = dqm_core::Result::Undefined;
      return result;
  };


 //Result

  dqm_core::Result* result = new dqm_core::Result(); 
  result->tags_["01-t0(ns)"] = t0;

  result->tags_["02-sigma_t0(ns)"] = t0_err;
  result->tags_["03-t_drift(ns)"] = (tmax-t0);
  result->tags_["04-sigma_t_drift(ns)"] = sqrt(tmax_err*tmax_err+t0_err*t0_err);
 
  int flag_count=0;

   if( Dtd*0.78125>max_shift ){
    flag_count+=1000;
    result->tags_["05-ERROR_(td-td_Ref)"] = Dtd;
  };
  
  if(Dt0>redTh*sigma_Dt0){
    flag_count+=3;
    result->tags_["07-ERROR_(t0-t0_Ref)"] = Dt0;
    result->tags_["08-ERROR_sigma(t0-t0_Ref)"] = sigma_Dt0;
  }else if(Dt0>greenTh*sigma_Dt0){
    flag_count+=1;
    result->tags_["07-ERROR_(t0-t0_Ref)"] = Dt0;
    result->tags_["08-ERROR_sigma(t0-t0_Ref)"] = sigma_Dt0;
  }; 
  
  
  if(Dtd>redTh*sigma_Dtd){
    flag_count+=3;
    result->tags_["05-ERROR_(td-td_Ref)"] = Dtd;
    result->tags_["06-ERROR_sigma(td-td_Ref)"] = sigma_Dtd;
  } else if(Dtd>greenTh*sigma_Dtd){
    flag_count+=1;
    result->tags_["05-ERROR_(td-td_Ref)"] = Dtd;
    result->tags_["06-ERROR_sigma(td-td_Ref)"] = sigma_Dtd;
  };
  
  
  if (flag_count>2) {
    result->status_ = dqm_core::Result::Red; 
    ERS_DEBUG(1,"Red");
  } else if (flag_count>=1 && flag_count<=2) {
    result->status_ = dqm_core::Result::Yellow;
    ERS_DEBUG(1,"Yellow");
  } else if (flag_count<1) {
    result->status_ = dqm_core::Result::Green;
    ERS_DEBUG(1,"Green");
  }
  return result;
}


void 
MDTTDCSpectrum::printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Check if the histogram t0 tmax and t_drift are compatible with the reference\n";
  message += "Mandatory Parameters: Green/Red Threshold: Limits:  warning: number of standard deviation from mean\n"; 
  message += "                                                    error\n";
  message += "Optional Parameters: MinStat = Minimum histogram statistics needed to perform Algorithm\n";
  message += "                     chi2 = chi square limit for histogram fit\n";
  message += "                     max_shift = maximum difference between t_drif of the histograns and reference\n";
  message += "\n";
  
  out << message;
}

} // namespace dqm_algorithms
