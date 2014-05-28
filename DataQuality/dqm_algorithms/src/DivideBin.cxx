/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file   DivideBin.cxx   calculates the ratio of two bins
 *  \author Mansoora Shamim
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/DivideBin.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/AlgorithmManager.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>

static dqm_algorithms::DivideBin myInstance;

dqm_algorithms::DivideBin::DivideBin()

{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("DivideBin", this);
}

dqm_algorithms::DivideBin * 
dqm_algorithms::DivideBin::clone()
{
  
  return new DivideBin();
}


dqm_core::Result *
dqm_algorithms::DivideBin::execute(const std::string & name,
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config)
{  
  TH1 * histogram;
  
  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)&object;
    if (histogram->GetDimension() > 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
  
 
  double gthresho;
  double rthresho;
  int xbin_num;
  int xbin_denom;
  try {
    xbin_num   = (int)dqm_algorithms::tools::GetFirstFromMap( "TestBin", config.getParameters());
    xbin_denom = (int)dqm_algorithms::tools::GetFirstFromMap( "RefBin", config.getParameters());
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  try {
    gthresho = dqm_algorithms::tools::GetFromMap( "Threshold", config.getGreenThresholds() );
    rthresho = dqm_algorithms::tools::GetFromMap( "Threshold", config.getRedThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig(ERS_HERE,name,"Parameter: 'Threshold' is mandatory, cannot continue");
    
  }
  
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  
  const double minrefentries = dqm_algorithms::tools::GetFirstFromMap( "MinRefEntries", config.getParameters(), 1 );
  
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  // Get the denominator
  double hdenom = histogram->GetBinContent(xbin_denom);  
  
  if ( minrefentries > 0 && hdenom < minrefentries) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientRefEntries"] = hdenom;
    return result;
  }
 
  // Get the numerator
  double hnum = histogram -> GetBinContent(xbin_num);
 
  double value = hnum/hdenom;

  dqm_core::Result* result = new dqm_core::Result();
  result->tags_["BinRatio"] = value;
  
if (rthresho < gthresho){
  if ( value >= gthresho ) {
    result->status_ = dqm_core::Result::Green;
  } else if (  value < rthresho ) {
    result->status_ = dqm_core::Result::Red;
  } else {
    result->status_ = dqm_core::Result::Yellow;
  }
} else {
  if ( value <= gthresho ) {
    result->status_ = dqm_core::Result::Green;
  } else if (  value > rthresho ) {
    result->status_ = dqm_core::Result::Red;
  } else {
    result->status_ = dqm_core::Result::Yellow;
  }
}
	  
  return result;
}


void dqm_algorithms::DivideBin::printDescription(std::ostream& out) {
  out<<"DivideBin : Performs the division of a particular bin by the total entries in that histogram "<< std::endl;
  out<<"Optional Parameter : MinStat : Minimum histogram statistics needed to perform Algorithm"<< std::endl;
  out<<"Optional Parameter : MinRefEntries : Minimum number of entries in reference bin needed to perform Algorithm"<< std::endl;
  out<<"Mandatory parameter: TestBin: Index of the bin in x that should be tested\n"<< std::endl;
  out<<"Mandatory parameter: RefBin: Index of the bin in x that should be used as a reference\n"<< std::endl;

}

