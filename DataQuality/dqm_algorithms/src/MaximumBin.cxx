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
  // get the histogram 
  TH1 * histogram;
  
  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)&object;
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
  
  int dim = histogram->GetDimension();
  
  // get the reference maximum bin indices
  int refBinX=-1, refBinY=-1, refBinZ=-1; // bin x,y,z-index to be compared to the maximum bin
  try {
    refBinX = (int)dqm_algorithms::tools::GetFirstFromMap("RefBinX", config.getParameters());
    if (dim>1) refBinY = (int)dqm_algorithms::tools::GetFirstFromMap("RefBinY", config.getParameters());
    if (dim>2) refBinZ = (int)dqm_algorithms::tools::GetFirstFromMap("RefBinZ", config.getParameters());
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  // check MinStat
  const double minstat = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), -1);
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  // find the maximum bin indices
  int maxBinX=-1, maxBinY=-1, maxBinZ=-1;
  histogram->GetMaximumBin(maxBinX,maxBinY,maxBinZ);
  
  // publish the maximum bin indices
  dqm_core::Result* result = new dqm_core::Result();
  result->tags_["MaxBin_xIndex"] = maxBinX;
  if (dim>1) result->tags_["MaxBin_yIndex"] = maxBinY;
  if (dim>2) result->tags_["MaxBin_zIndex"] = maxBinZ;
  
  // check if the conditions are met
  bool match = (maxBinX == refBinX);
  if (dim>1) match &= (maxBinY == refBinY);
  if (dim>2) match &= (maxBinZ == refBinZ);
  
  // set the flag and return the result
  if (match) result->status_ = dqm_core::Result::Green;
  else result->status_ = dqm_core::Result::Red;
  
  return result;
}


void dqm_algorithms::MaximumBin::printDescription(std::ostream& out) {
  out<<"MaximumBin : Finds a bin with the maximum content and checks if the bin index matches the expectation"<< std::endl;
  out<<"Mandatory parameter : RefBinX : Bin x-index where the maximum is expected"<< std::endl;
  out<<"Optional parameter  : RefBinY : Bin y-index where the maximum is expected"<< std::endl;
  out<<"Optional parameter  : RefBinZ : Bin z-index where the maximum is expected"<< std::endl;
  out<<"Optional Parameter  : MinStat : Minimum histogram statistics needed to perform Algorithm\n"<< std::endl;
}

