/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MDTTubeCheckError.cxx,v 1.0 2008/31/07 Elena Solfaroli Camillocci
// **********************************************************************

#include "dqm_algorithms/MDTTubeCheckError.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TClass.h>
#include <TH1.h>
#include <TAxis.h>
//#include <TF1.h>
//#include <TProfile.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"


static dqm_algorithms::MDTTubeCheckError staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

MDTTubeCheckError::MDTTubeCheckError()
  : m_name("MDTTubeCheckError")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


MDTTubeCheckError::~MDTTubeCheckError()
{
}


dqm_core::Algorithm*
MDTTubeCheckError::clone()
{
  return new MDTTubeCheckError(*this);
}


dqm_core::Result*
MDTTubeCheckError::execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
{
  TH1* histogram;
  TH1* refhist=0;
  
  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)&object;
    if (histogram->GetDimension() >= 2 ){
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension >= 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }   
  
  //Get Parameters and Thresholds
  double minstat; 
  double bin_threshold;
  double nErr;
  double LowStatErr;
  double LowStatThre;
  double refcheck;
  double greenTh;
  double redTh;
  try {
    bin_threshold = dqm_algorithms::tools::GetFirstFromMap( "BinThreshold", config.getParameters());
    nErr          = dqm_algorithms::tools::GetFirstFromMap( "nErrBin", config.getParameters());
    LowStatErr    = dqm_algorithms::tools::GetFirstFromMap( "LowStatErr", config.getParameters(), 99999.);
    LowStatThre   = dqm_algorithms::tools::GetFirstFromMap( "LowStatThre", config.getParameters(), 0.);
    refcheck      = dqm_algorithms::tools::GetFirstFromMap( "ReferenceCheck", config.getParameters());
    minstat       = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), -1);
    redTh         = dqm_algorithms::tools::GetFromMap( "NBins", config.getRedThresholds());
    greenTh       = dqm_algorithms::tools::GetFromMap( "NBins", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  //Get Reference Histo
  if (refcheck>0) {
    try {
      refhist = static_cast<TH1 *>( config.getReference() );
    }
    catch ( dqm_core::Exception & ex ) {
      throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
    }
    if (histogram->GetDimension() != refhist->GetDimension() ) {
      throw dqm_core::BadRefHist( ERS_HERE, name, "Reference VS histo: Different dimension!" );
    } 
    if (histogram->GetNbinsX() != refhist->GetNbinsX() ) {
      throw dqm_core::BadRefHist( ERS_HERE, name, "Reference VS histo: Different number of bins!" );
    }
  }
    
 //Check of statistics
  if (histogram->GetEntries() < minstat ) {
    ERS_INFO("Histogram does not satisfy MinStat requirement " <<histogram->GetName());
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  ERS_DEBUG(1,"Statistics: "<< histogram->GetEntries()<< " entries ");

  int count = 0;
  std::vector<int> range;
  if (histogram->GetDimension() == 1){
    range = dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  } else {
    ERS_INFO("Histogram has not dimension 1 : " <<histogram->GetName());
    throw dqm_core::Exception( ERS_HERE, histogram->GetName() );
  }
  if (range[1] == 0) {
    ERS_INFO("Empty histogram: " <<histogram->GetName());
    dqm_core::Result* result = new dqm_core::Result(); 
    result->status_ = dqm_core::Result::Undefined; 
    ERS_DEBUG(1,"Undefined");
    return result;
  }
  
  std::vector<int> Tubes; 
  int EmptyTubes =0;
  int LowStatTubes =0;
  for ( int i = range[0]; i <= range[1]; ++i ) {
    double Content = histogram->GetBinContent(i);
    double ErrCont = histogram->GetBinError(i);
    if (ErrCont > LowStatErr) LowStatTubes++;
    if (Content+fabs(ErrCont) != 0.) {
      if ((Content + nErr*ErrCont) < bin_threshold ) {
	if (refcheck>0) {
	  double RefCont = refhist->GetBinContent(i);
	  double RefErrCont = refhist->GetBinError(i);
	  double Diff = fabs(Content - RefCont);
	  double ErrDiff = sqrt(ErrCont*ErrCont + RefErrCont*RefErrCont);
	  if (Diff > nErr*ErrDiff) {
	    ++count;
	    Tubes.push_back(i);
	  }
	} else {
	  ++count;
	  Tubes.push_back(i);
	}
      }
    } else { ++EmptyTubes; }
  }
  ERS_DEBUG(1,"Number of bins " << name << " different from a treshold of " << bin_threshold << " is " << count );

  dqm_core::Result* result = new dqm_core::Result(); 
  result->tags_["NChangedStatusTubes"] = count;
  if (Tubes.size()>0) {
    for (int k=0; k<(int)Tubes.size(); k++) {
      std::string ToDB="ChangedStatusTube_";
      char* nn;
      asprintf(&nn, "%i", k+1);
      ToDB += nn;
      result->tags_[ToDB] = Tubes[k];
      ERS_DEBUG(1,"MDT Tube which changed status: "<<ToDB<<" = "<<Tubes[k] );
    }
  }

  if ((EmptyTubes == range[1]) || ((LowStatTubes/range[1]) > LowStatThre)) {
    result->status_ = dqm_core::Result::Undefined; 
    ERS_DEBUG(1,"Undefined");
  } else if (count >= redTh) {
    result->status_ = dqm_core::Result::Red; 
    ERS_DEBUG(1,"Red");
  } else if ( count > greenTh) {
    result->status_ = dqm_core::Result::Yellow;
    ERS_DEBUG(1,"Yellow");
  } else {
    result->status_ = dqm_core::Result::Green;
    ERS_DEBUG(1,"Green");
  }
  return result;
}


void 
MDTTubeCheckError::printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Check if the number of entries in bins is less than BinThreshold and compare with Reference Histo \n";
  message += "Mandatory Parameter: BinThreshold: Look for bins less than BinTreshold; Count number of bins satifying requirement \n";
  message += "Mandatory Parameter: nErrBin: n-sigma of acceptance for check with the BinTreshold \n";
  message += "Mandatory Parameter: ReferenceCheck: 0 if no check on reference is requested \n";
  message += "Mandatory Green/Red Threshold: NBins: number of bins satifying requirement\n";
  message += "Optional Parameters: MinStat = Minimum histogram statistics needed to perform Algorithm\n";
  message += "                     LowStatErr = error threshold for bin with too low statistics \n";
  message += "                     LowStatThre = threshold for fraction of bins with too low statistics \n";
  message += "                     xmin: minimum x range\n";
  message += "                     xmax: maximum x range\n";
  message += "\n";
  
  out << message;
}

} // namespace dqm_algorithms
