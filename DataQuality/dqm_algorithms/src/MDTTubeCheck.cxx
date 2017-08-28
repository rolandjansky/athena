/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MDTTubeCheck.cxx,v 1.0 2008/31/07 Elena Solfaroli Camillocci
// **********************************************************************

#include "dqm_algorithms/MDTTubeCheck.h"

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


static dqm_algorithms::MDTTubeCheck staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

MDTTubeCheck::MDTTubeCheck()
  : m_name("MDTTubeCheck")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


MDTTubeCheck::~MDTTubeCheck()
{
}


dqm_core::Algorithm*
MDTTubeCheck::clone()
{
  return new MDTTubeCheck(*this);
}


dqm_core::Result*
MDTTubeCheck::execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
{
  TH1 * histogram(0);
  TH1 * refhist(0);
  
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
  double refcheck;
  double greenTh;
  double redTh;
  double LowStatLevel;
  try {
    bin_threshold = dqm_algorithms::tools::GetFirstFromMap( "BinThreshold", config.getParameters());
    refcheck      = dqm_algorithms::tools::GetFirstFromMap( "ReferenceCheck", config.getParameters());
    LowStatLevel  = dqm_algorithms::tools::GetFirstFromMap( "LowStatLevel", config.getParameters(), 10);
    minstat       = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
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

  int NewDead_count = 0;
  int StillDead_count = 0;
  int Revived_count = 0;
  int LowStat_count = 0;
  std::vector<int> range;
  if (histogram->GetDimension() == 1){
    range = dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  } else {
    ERS_INFO("Histogram has not dimension 1 : " <<histogram->GetName());
    throw dqm_core::Exception( ERS_HERE, histogram->GetName() );
  }

  std::vector<int> Tubes; 
  for ( int i = range[0]; i <= range[1]; ++i ) {
    double content = histogram->GetBinContent(i);
    if (content == LowStatLevel) {
      ++LowStat_count;
    } else if (content != 0.) {
      if (refcheck>0) {
	double RefCont = refhist->GetBinContent(i);
	if ((content < bin_threshold) && ((RefCont >= bin_threshold) || (RefCont == LowStatLevel))) {
	  ++NewDead_count;
	  Tubes.push_back(i);
	} else if ((content < bin_threshold) && ((RefCont < bin_threshold) || (RefCont == LowStatLevel))) {
	  ++StillDead_count;
	} else if ((content >= bin_threshold) && (RefCont < bin_threshold)) {
	  ++Revived_count;
	}
      } else {
	if (((content < bin_threshold) ) ) {
	  ++NewDead_count;
	  Tubes.push_back(i);
	}
      }
    }
  }
  ERS_DEBUG(1,"Number of bins " << name << " different from a treshold of " << bin_threshold << " is " << NewDead_count );

  dqm_core::Result* result = new dqm_core::Result(); 
  if (LowStat_count > 0) result->tags_["NTubes_withLowStat"] = LowStat_count;
  if (Revived_count > 0) result->tags_["NTubes_Revived"] = Revived_count;
  result->tags_["NewDead_Tubes"] = NewDead_count;
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
  if (NewDead_count >= redTh) {
    result->status_ = dqm_core::Result::Red; 
    ERS_DEBUG(1,"Red");
  } else if ( (NewDead_count > greenTh) || (Revived_count > 0)) {
    result->status_ = dqm_core::Result::Yellow;
    ERS_DEBUG(1,"Yellow");
  } else if (LowStat_count > 0) {
    result->status_ = dqm_core::Result::Undefined;
    ERS_DEBUG(1,"Undefined");
  } else if (StillDead_count == range[1]) {
    //All tubes are still dead
    result->status_ = dqm_core::Result::Undefined;
  } else {
    result->status_ = dqm_core::Result::Green;
    ERS_DEBUG(1,"Green");
  }
  return result;
}


void 
MDTTubeCheck::printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Check if the number of entries in bins is less than BinThreshold and compare with Reference Histo \n";
  message += "Mandatory Parameter: BinThreshold = Look for bins less than BinTreshold; Count number of bins satifying requirement \n";
  message += "Mandatory Parameter: ReferenceCheck = 0 if no check on reference is requested \n";
  message += "Mandatory Green/Red Threshold: NBins = number of bins satifying requirement\n";
  message += "Optional Parameters: LowStatLevel = Level (in y) that means too few statistics for analysis\n";
  message += "Optional Parameters: MinStat = Minimum histogram statistics needed to perform Algorithm\n";
  message += "                     xmin: minimum x range\n";
  message += "                     xmax: maximum x range\n";
  message += "\n";
  
  out << message;
}

} // namespace dqm_algorithms
