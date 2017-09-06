/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  Algorithm: AlgHLTMETStatus does the following
 *             Checks the [EF/L2]_MET_status histograms (produced for 10 min block / entire run)
 *             There are 32 status bits and each bin of the above histograms represent a status.
 *             If the status bits 17 -> 29 (in general binLo -> binHi) are filled, flag = YELLOW
 *             If the status bits 32 -> 32 (in general binLo -> binHi) are filled, flag = RED
 *             Expects the following from configuration [<value> corresponds to a bin number]
 *             METYellowMin = <value>  specify the bins you want to compute flags for [1-32]
 *             METYellowMax = <value>  ditto
 *             METRedMin = <value>     ditto
 *             METRedMax = <value>     ditto
 *             DoYellowFlag = <1 or 0> 0 means don't compute yellow flags
 *             DoRedFlag = <1 or 0> 0 means don't compute red flags
 *             
 *  Author   : Venkatesh Kaushik <venkat.kaushik@cern.ch>
 *  Date     : March 2010
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/HLTMETStatus.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>
#include <cmath>
#include <math.h>

#include <dqm_core/AlgorithmManager.h>

static dqm_algorithms::HLTMETStatus myInstance;

dqm_algorithms::HLTMETStatus::HLTMETStatus()

{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("AlgHLTMETStatus", this);
}

dqm_algorithms::HLTMETStatus * 
dqm_algorithms::HLTMETStatus::clone()
{
  
  return new HLTMETStatus();
}


dqm_core::Result *
dqm_algorithms::HLTMETStatus::execute(const std::string & name,
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
  ///////////////////////////////////////////////////////////////////////////////////////////
  // Get Green and Red Thresholds
  double grThr = 0.05, reThr = 0.1; // Green and Red thresholds
  try {
    grThr = dqm_algorithms::tools::GetFromMap("Threshold",config.getGreenThresholds() );
    reThr = dqm_algorithms::tools::GetFromMap("Threshold",config.getRedThresholds() );
  } catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig(ERS_HERE,name,"Paramter: 'Threshold' is mandatory, cannot continue");
  }

  //non sense case: compare fraction and threshold >100%
  if ((grThr>1.0 || reThr>1.0) ) {
    throw dqm_core::BadConfig(ERS_HERE,m_name,"Configuration Error: Threshold should be between [0.0, 1.0] 10% => 0.1");
  }


  ///////////////////////////////////////////////////////////////////////////////////////////
  // Get Parameters
  size_t ntotBins = (size_t) histogram -> GetNbinsX();

  // read thresholds
  size_t colflags[4];
  colflags[0] = static_cast<size_t>(dqm_algorithms::tools::GetFirstFromMap("METYellowMin", config.getParameters(), 1));
  colflags[1] = static_cast<size_t>(dqm_algorithms::tools::GetFirstFromMap("METYellowMax", config.getParameters(), ntotBins));
  colflags[2] = static_cast<size_t>(dqm_algorithms::tools::GetFirstFromMap("METRedMin",    config.getParameters(), 1));
  colflags[3] = static_cast<size_t>(dqm_algorithms::tools::GetFirstFromMap("METRedMax",    config.getParameters(), ntotBins));
  // read options
  bool doflags[2];
  doflags[0] = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("DoYellowFlag", config.getParameters(), 1));
  doflags[1] = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("DoRedFlag",    config.getParameters(), 1));

  dqm_core::Result* result = new dqm_core::Result();
  colflags[0] = ((colflags[0] > 0) && (colflags[0] <= ntotBins)) ? colflags[0] : 1;
  colflags[1] = ((colflags[1] >= colflags[0]) && (colflags[1] <= ntotBins)) ? colflags[1] : ntotBins;
  colflags[2] = ((colflags[2] > 0) && (colflags[2] <= ntotBins)) ? colflags[2] : 1;
  colflags[3] = ((colflags[3] >= colflags[2]) && (colflags[3] <= ntotBins)) ? colflags[3] : ntotBins;
  bool yellowLo2HiEmpty = true, redLo2HiEmpty = true;
  int ytot = 0, ycnt = 0, rtot = 0, rcnt = 0;
  
  // we fill 1(error) or 0(no-error) for each bin for every event
  // therefore we have to divide #of entries by #of bins
  // to get the #of events
  double nentries = histogram -> GetEntries(); 
  double nevtstot = 1.; // no. of events
  if(!std::isnan(nentries/ntotBins) || !std::isinf(nentries / ntotBins)) nevtstot = nentries/ntotBins;

  // yellow flag
  double theYFracMax = -9., theRFracMax = -9.;
  if(doflags[0]) {
    for( size_t j = colflags[0]; j <= colflags[1]; j++ ) {
      // if at least one of the bins > 0 : flag = YELLOW
      ytot ++;
      double thebinc = histogram -> GetBinContent(j);
      // fraction of events having this error-bit set
      double thefrac = thebinc / nevtstot; 

      // "Worst-case flag not to exceed YELLOW" flag:
      // if threshold exceeds green threshold, set yellow
      // if threshold exceeds red threshold, still set yellow
      // reThr > grThr
      if(thefrac > grThr)  { 
        if(theYFracMax < thefrac) theYFracMax = thefrac;
        yellowLo2HiEmpty = false;
        ycnt ++; 
      }
    }
  }

  // red flag
  if(doflags[1]) {
    for( size_t j = colflags[2]; j <= colflags[3]; j++ ) {
      // if at least one of the bins > 0 : flag = RED
      rtot ++; 
      double thebinc = histogram -> GetBinContent(j);
      // fraction of events having this error-bit set
      double thefrac = thebinc / nevtstot; 
      if(thefrac > reThr)  { 
        if(theRFracMax < thefrac) theRFracMax = thefrac;
        redLo2HiEmpty = false;
        rcnt++;
      } 
    }
  }

  // set results
  result->status_ = dqm_core::Result::Green;

  if(doflags[0] && !yellowLo2HiEmpty) {
    result->status_ = dqm_core::Result::Yellow;
  }

  if(doflags[1] && !redLo2HiEmpty) {
    result->status_ = dqm_core::Result::Red;
  }

  result->tags_["NumOfStatusBitsYellow"] = ycnt; 
  result->tags_["EventFractionYellow"] = theYFracMax; 
  result->tags_["NumOfStatusBitsRed"] = rcnt;
  result->tags_["EventFractionRed"] = theRFracMax;
 
///////////////////////////////////////////////////////////////////////////////////////////
  
  return result;
}

void
dqm_algorithms::HLTMETStatus::printDescription(std::ostream& out)
{
  
  out<<"HLT MET Status: Of the 32 status bits, check if any subset of bits are set. If so, flag YELLOW" << std::endl;
  out<<"Bit #32 is global error bit, if set flag RED." << std::endl;
  
  out<<"Mandatory parameter: XBin: The label of the X bin that you would like to check\n"<<std::endl;
  
}

