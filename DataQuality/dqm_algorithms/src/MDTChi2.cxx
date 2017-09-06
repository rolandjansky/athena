/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MDTChi2.cxx,v 1.0 2008/10/08 Valerio Consorti
// **********************************************************************

#include "dqm_algorithms/MDTChi2.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TClass.h>
#include <TH1.h>
#include <TAxis.h>
#include <TF1.h>
#include <TProfile.h>
#include "TMath.h"

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"

static dqm_algorithms::MDTChi2 staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

MDTChi2::MDTChi2()
  : m_name("MDTchi2")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


MDTChi2::~MDTChi2()
{
}


dqm_core::Algorithm*
MDTChi2::clone()
{
  return new MDTChi2(*this);
}


dqm_core::Result*
MDTChi2::execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
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
  double greenTh;
  double redTh;
  try {
    minstat      = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), 50);
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
  if (hist->GetNbinsX() != ref->GetNbinsX() ) {
    throw dqm_core::BadRefHist( ERS_HERE, name, "Reference VS histo: Different bin numbre in X axis!" );
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

  int i = 0;
  int count = 1;
  Double_t n     = 0;
  Double_t n_ref = 0;
  Double_t chi2  = 0;
  int max_bin = hist -> GetNbinsX();
  Double_t N     = hist -> GetEntries();
  Double_t N_ref = ref  -> GetEntries();

  if(N == 0){
    ERS_INFO("Histogram has no entries" <<hist->GetName());
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = hist->GetEntries();
    return result;
  };    

  if(N_ref == 0){
    ERS_INFO("Reference histogram has no entries" <<hist->GetName());
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientRefEntries"] = ref->GetEntries();
    return result;
  };    

  double norm=1;
  double norm_ref=1;

  if(N != 0) norm=1/N; 
  if(N_ref != 0) norm_ref=1/N_ref;

  for(i=1;i<=max_bin;i++){
    n     = hist->GetBinContent(i);
    n_ref = ref->GetBinContent(i);
    if( n != 0 || n_ref != 0) chi2+=(n*norm-n_ref*norm_ref)*(n*norm-n_ref*norm_ref)/(n*norm*norm+n_ref*norm_ref*norm_ref);
    if( n == 0 && n_ref == 0) count++;
  };
  
  Double_t prob=0;
  prob=TMath::Prob(chi2,(max_bin-count));


 //Result

  dqm_core::Result* result = new dqm_core::Result(); 
  result->tags_["chi2_prob"] = prob;

  if (prob>greenTh) {
    result->status_ = dqm_core::Result::Green; 
    ERS_DEBUG(1,"Green");
  } else if ( prob<=greenTh && prob>redTh ) {
    result->status_ = dqm_core::Result::Yellow;
    ERS_DEBUG(1,"Yellow");
  } else if (prob<=redTh) {
    result->status_ = dqm_core::Result::Red;
    ERS_DEBUG(1,"Red");
  }
  return result;
}


void 
MDTChi2::printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Perform a chi square check between input istogram and reference\n";
  message += "Mandatory Green/Red Threshold: Limits: chi square probability limits\n";
  message += "Optional Parameters: MinStat = Minimum histogram statistics needed to perform Algorithm\n";
  message += "\n";
  
  out << message;
}

} // namespace dqm_algorithms
