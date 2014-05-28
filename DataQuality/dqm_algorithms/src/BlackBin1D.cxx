/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BlackBin.cxx checks an individual bin of a 2D histogram wrt to a threshold value and returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BlackBin1D.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>


#include <dqm_core/AlgorithmManager.h>

static dqm_algorithms::BlackBin1D myInstance;

dqm_algorithms::BlackBin1D::BlackBin1D()

{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("BlackBin1D", this);
}

dqm_algorithms::BlackBin1D * 
dqm_algorithms::BlackBin1D::clone()
{
  
  return new BlackBin1D();
}


dqm_core::Result *
dqm_algorithms::BlackBin1D::execute(const std::string & name,
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
  
  //Unused so commented out:
  //double bin_threshold;
  //double gthreshold;
  //double rthreshold;
  int xbin;
 
  try {
    xbin = (int)dqm_algorithms::tools::GetFirstFromMap( "XBin", config.getParameters());
 
  }
  catch ( dqm_core::Exception & ex ) {
    //out << "Some of the parameters are missing!" << std::endl;
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  //Get the bin content of the histogram
  double binContents= histogram -> GetBinContent(xbin);
  
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
dqm_algorithms::BlackBin1D::printDescription(std::ostream& out)
{
  
  out<<"Black bin 1D: Returns status -1 (black) if bin has no contents\n" << std::endl;
  
  out<<"Mandatory parameter: XBin: The label of the X bin that you would like to check\n"<<std::endl;
  
  
}

