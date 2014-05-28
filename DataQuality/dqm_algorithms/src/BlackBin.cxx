/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BlackBin.cxx checks an individual bin of a 2D histogram wrt to a threshold value and returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BlackBin.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>


#include <dqm_core/AlgorithmManager.h>

static dqm_algorithms::BlackBin myInstance;

dqm_algorithms::BlackBin::BlackBin()

{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("BlackBin", this);
}

dqm_algorithms::BlackBin * 
dqm_algorithms::BlackBin::clone()
{
  
  return new BlackBin();
}


dqm_core::Result *
dqm_algorithms::BlackBin::execute(const std::string & name,
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config)
{  
  TH2 * histogram;
  
  if( object.IsA()->InheritsFrom( "TH2" ) ) {
    histogram = (TH2*)&object;
    if (histogram->GetDimension() > 3 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 3 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH2" );
  }
  
  // Unused, so commented out: (Why does this algorithm not take a threshold?)
  //  double bin_threshold;
  //  double gthreshold;
  //  double rthreshold;
  int xbin;
  int ybin;
  try {
    xbin = (int)dqm_algorithms::tools::GetFirstFromMap( "XBin", config.getParameters());
    ybin = (int)dqm_algorithms::tools::GetFirstFromMap( "YBin", config.getParameters());
  }
  catch ( dqm_core::Exception & ex ) {
    //out << "Some of the parameters are missing!" << std::endl;
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  //Get the bin content of the histogram
  double binContents= histogram -> GetBinContent(xbin,ybin);
  
  dqm_core::Result* result = new dqm_core::Result();
  result->tags_["BinContent"] = binContents;
  if(binContents>0){
    result->status_ = dqm_core::Result::Disabled;
  }
  else {
    result->status_ = dqm_core::Result::Green;
  }
  
  return result;
}

void
dqm_algorithms::BlackBin::printDescription(std::ostream& out)
{
  
  out<<"Black bin: Returns status -1 (black) if bin has no contents\n" << std::endl;
  
  out<<"Mandatory parameter: XBin: The label of the X bin that you would like to check\n"<<std::endl;
  
  out<<"Mandatory parameter: YBin: The label of the Y bin that you would like to check\n"<<std::endl;
  
}

