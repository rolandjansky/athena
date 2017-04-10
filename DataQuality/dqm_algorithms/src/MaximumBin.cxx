/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "dqm_algorithms/MaximumBin.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "TH1.h"

static dqm_algorithms::MaximumBin myInstance;

dqm_algorithms::MaximumBin::MaximumBin()
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("MaximumBin", this);
}

dqm_algorithms::MaximumBin * 
dqm_algorithms::MaximumBin::clone()
{
  return new MaximumBin();
}


dqm_core::Result *
dqm_algorithms::MaximumBin::execute(const std::string & name,
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
  
  int refbin; // bin number to be compared to the maximum bin
  try {
    refbin = (int)dqm_algorithms::tools::GetFirstFromMap( "RefBin", config.getParameters());
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  double maxbin = histogram->GetMaximumBin();

  dqm_core::Result* result = new dqm_core::Result();
  result->tags_["MaximumBin"] = maxbin;
  
  if (maxbin==refbin) result->status_ = dqm_core::Result::Green;
  else result->status_ = dqm_core::Result::Red;
  
  return result;
}


void dqm_algorithms::MaximumBin::printDescription(std::ostream& out) {
  out<<"MaximumBin : Finds a bin with the maximum content and checks if the bin number equals RefBin"<< std::endl;
  out<<"Optional Parameter : MinStat : Minimum histogram statistics needed to perform Algorithm"<< std::endl;
  out<<"Mandatory parameter: RefBin: Bin number where the maximum is expected\n"<< std::endl;
}

