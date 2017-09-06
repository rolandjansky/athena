/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MDTADCSpectrum.cxx,v 1.0 2008/10/08 Valerio Consorti
// **********************************************************************


#include "dqm_algorithms/MDTADCSpectrum.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TClass.h>
#include <TH1.h>
#include <TF1.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"


static dqm_algorithms::MDTADCSpectrum staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

MDTADCSpectrum::MDTADCSpectrum()
  : m_name("MDTADCspectrum")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


MDTADCSpectrum::~MDTADCSpectrum()
{
}


dqm_core::Algorithm*
MDTADCSpectrum::clone()
{
  return new MDTADCSpectrum(*this);
}


dqm_core::Result*
MDTADCSpectrum::execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
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
  double thresh;
  double greenTh;
  double redTh;
  try {
    thresh        = dqm_algorithms::tools::GetFirstFromMap( "thresh", config.getParameters(), 50);
    minstat       = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), 0);
    redTh         = dqm_algorithms::tools::GetFromMap( "Limits", config.getRedThresholds());
    greenTh       = dqm_algorithms::tools::GetFromMap( "Limits", config.getGreenThresholds() );
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

  Double_t n_min_50=0;
  Double_t n_min_50_ref=0;

  Double_t N     = hist->GetEntries();
  Double_t N_ref = ref ->GetEntries();

  int i=0;

  do{
    i++;
    n_min_50 += hist->GetBinContent(i);
    if(i>hist->GetNbinsX()){
      ERS_INFO("bin limit!"<<i);
      dqm_core::Result* result = new dqm_core::Result();
      result->status_ = dqm_core::Result::Undefined;
      return result;
    };
  }while(hist->GetBinCenter(i)<thresh);

  i=0;

  do{
    i++;
    n_min_50_ref += ref->GetBinContent(i);
    if(i>ref->GetNbinsX()){
      ERS_INFO("bin limit!"<<i);
      dqm_core::Result* result = new dqm_core::Result();
      result->status_ = dqm_core::Result::Undefined;
      return result;
    };
  }while(ref->GetBinCenter(i)<thresh);

  Double_t f  = -1;
  Double_t f_r=  1;
  Double_t sigma_hist = -1;
  Double_t sigma_ref  = -1;
  Double_t sigma      = -1;
  Double_t F = -1;
  

  if(N == 0 && N_ref == 0) {
      ERS_INFO("null N entries found");
      dqm_core::Result* result = new dqm_core::Result();
      result->tags_["ERROR_null_entries_found"] = -999.;
      result->status_ = dqm_core::Result::Undefined;
      return result; 
  };

  if(N !=0 && N_ref !=0){
    f  = n_min_50/N;
    f_r = n_min_50_ref/N_ref;

    if(f*f_r == 0 || (1-f)<0 || (1-f_r)<0 ){
      ERS_DEBUG(1,"*) f   = "<<f<<"     f_r = "<<f_r);
      dqm_core::Result* result = new dqm_core::Result();
      result->tags_["ERROR_problem_in_fraction_computation"] = -999.;
      result->tags_["n_min_50/N"] = f;
      result->tags_["n_min_50_ref/N_ref"] = f_r;
      result->status_ = dqm_core::Result::Undefined;
      return result;
    };

    //Bernulli distribution

    sigma_hist= sqrt(f*(1-f)/N);
    sigma_ref= sqrt(f_r*(1-f_r)/N_ref); 

    F=f/f_r;

    sigma = F*sqrt((sigma_hist/f)*(sigma_hist/f)+(sigma_ref/f_r)*(sigma_ref/f_r));


  };


 //Result

  dqm_core::Result* result = new dqm_core::Result(); 
  result->tags_["00-Pedestal_fraction"] = f;
  result->tags_["1-Pedestal_over_Ped_ref"] = (1-F);
  result->tags_["sigma_1-Ped_over_Ped_ref"] = sigma;

  if(f<= greenTh || (f>greenTh && f_r>greenTh)) {
    result->status_ = dqm_core::Result::Green;
  }
  else if( f>redTh && f_r<greenTh ) {
    result->tags_["Difference_against_Ref-Pedestal_fraction_Ref"] = f_r;
    result->status_ = dqm_core::Result::Red;
  }
  else {
    result->status_ = dqm_core::Result::Yellow;
  }
  
  // Return the result
  return result;
}


void
MDTADCSpectrum::printDescription(std::ostream& out){
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Checks if the ratio of hits under treshold is below the limits\n";
  message += "             Comparison with reference is performed \n";
  message += "Mandatory Parameters: Green/Red Threshold: Limits:  warning: ratio limits \n"; 
  message += "                                                    error\n";
  message += "Optional Parameters: MinStat = Minimum histogram statistics needed to perform Algorithm\n";
  message += "                     thresh  = ADC cut on pedestal N counts (def 50)\n";
   message += "\n";
  
  out << message;
}

} // namespace dqm_algorithms
