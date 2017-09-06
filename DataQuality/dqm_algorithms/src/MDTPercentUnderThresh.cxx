/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MDTPercentUnderThresh.cxx,v 1.0 2009/06/10 Valerio Consorti
// **********************************************************************


#include "dqm_algorithms/MDTPercentUnderThresh.h"

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


static dqm_algorithms::MDTPercentUnderThresh staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

MDTPercentUnderThresh::MDTPercentUnderThresh()
  : m_name("MDTpercent_under_thresh")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


MDTPercentUnderThresh::~MDTPercentUnderThresh()
{
}


dqm_core::Algorithm*
MDTPercentUnderThresh::clone()
{
  return new MDTPercentUnderThresh(*this);
}


dqm_core::Result*
MDTPercentUnderThresh::execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
{
  TH1 * hist;
  
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
    thresh        = dqm_algorithms::tools::GetFirstFromMap( "Thresh", config.getParameters());
    minstat       = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), 0);
    redTh         = dqm_algorithms::tools::GetFromMap( "Percent", config.getRedThresholds());
    greenTh       = dqm_algorithms::tools::GetFromMap( "Percent", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
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

  Double_t hit_under_th=0;

  Double_t N     = hist->GetEntries();

  if(N == 0) {
      dqm_core::Result* result = new dqm_core::Result();
      result->tags_["Empty_Histogram_found_N_Entries"] = N;
      result->status_ = dqm_core::Result::Undefined;
      return result;
  };

  int i=0;

  do{
    i++;
    if(i>hist->GetNbinsX()){
      dqm_core::Result* result = new dqm_core::Result();
      result->tags_["Config_error_maximum_bin_exceed_looking_for_bin_number"] = i;
      result->status_ = dqm_core::Result::Undefined;
      return result;
    };
    hit_under_th += hist->GetBinContent(i);
  }while(hist->GetBinCenter(i)<thresh);
  
  double percent = 100 - 100*((double) hit_under_th)/((double) N);

 //Result

  dqm_core::Result* result = new dqm_core::Result(); 
  result->tags_["Entries_above_thresh(%)"] = percent;

  if( percent >= greenTh ) {
    result->status_ = dqm_core::Result::Green;
  }
  else if( percent <greenTh && percent>redTh ) {
    result->status_ = dqm_core::Result::Yellow;
  }
  else {
    result->status_ = dqm_core::Result::Red;
  }
  
  // Return the result
  return result;
}


void
MDTPercentUnderThresh::printDescription(std::ostream& out){
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Compute the percent of entries above a threshold and and compare it with user defined green and red Threshold\n";
  message += "Mandatory Parameters: Green/Red Threshold: Percent:  Percent of the entries above threshold requested"; 
  message += "Optional Parameters: MinStat = Minimum histogram statistics needed to perform Algorithm\n";
  message += "                     thresh  = threshold\n";
  message += "\n";
  
  out << message;
}

} // namespace dqm_algorithms
