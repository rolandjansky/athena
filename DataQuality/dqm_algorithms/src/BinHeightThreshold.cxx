/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinHeightThreshold.cxx checks for a set of consecutive bins above a threshold value and returns dqm_core::Result
 * \author Margherita Spalla (adjustment of BinThreshold.cxx)
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BinHeightThreshold.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>

#include <iostream>

#include <dqm_core/AlgorithmManager.h>


static dqm_algorithms::BinHeightThreshold myInstance;


dqm_algorithms::BinHeightThreshold::BinHeightThreshold()
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("BinHeightThreshold", this);
}

dqm_algorithms::BinHeightThreshold * 
dqm_algorithms::BinHeightThreshold::clone()
{
  
  return new BinHeightThreshold();
}


dqm_core::Result *
dqm_algorithms::BinHeightThreshold::execute(	const std::string &  name, 
						const TObject & object, 
                                                const dqm_core::AlgorithmConfig & config )
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
  const double minstat = 1;
  if (histogram->GetEntries() < minstat ) {
    ERS_INFO( "Too few entries: " << histogram->GetEntries() );
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }

  int start_from_last;
  int n_bins;
  int window_size; 
  double gthreshold;
  double rthreshold;
  try {
    n_bins = dqm_algorithms::tools::GetFirstFromMap( "NBins", config.getParameters() ,1);
    window_size = dqm_algorithms::tools::GetFirstFromMap( "WindowSize", config.getParameters() ,1); 
    start_from_last = dqm_algorithms::tools::GetFirstFromMap( "StartFromLast", config.getParameters() ,-1); 
    gthreshold = dqm_algorithms::tools::GetFromMap( "HeightThreshold", config.getGreenThresholds() );
    rthreshold = dqm_algorithms::tools::GetFromMap( "HeightThreshold", config.getRedThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  dqm_core::Result* result = new dqm_core::Result();
  TH1* resulthisto;
  if (histogram->InheritsFrom("TH2")) {
	throw dqm_core::BadConfig( ERS_HERE, name, "TH2 received. This algorithm only works with TH1" );
  } else if (histogram->InheritsFrom("TH1")) {
        resulthisto=(TH1*)(histogram->Clone());
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
 
  resulthisto->Reset();


  //first, look for the last filled LB, i.e. the bin with nonzero content of largest x if start_from_last<0, if start_from_last>=0, then starts from last but N-th bin, where N=start_from_last
  int i_currentLB=histogram->GetNbinsX();
  if(start_from_last>=0)
    i_currentLB-=start_from_last;
  else
    {
      while(i_currentLB>=1)
	{
	  if(histogram->GetBinContent(i_currentLB)!=0) break;
	  i_currentLB--;
	}
    }
  //if the histogram is just empty, or still has too few bins, do nothing (check already done, in principle should not happen)
   if(i_currentLB<=0)
    {
     ERS_DEBUG(1,"start_from_last parameter >= total number of bins, I just cannot di the check. Do nothing.");
     return result;
    }

   //now loop over an x window of size 'window_size'
    int iLB=i_currentLB;
    int countYellow=0;
    int countRed=0;
    while(iLB>=1 && (i_currentLB-iLB)<window_size)
       {
	double content=histogram->GetBinContent(iLB);
	dqm_algorithms::BinHeightThreshold::binStatus LBstatus=CompareBinHeightThreshold(content, gthreshold , rthreshold);
	if(LBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin) 
	{
	  countYellow++;
   	}
	if(LBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aRedBin) 
	{
	  countRed++;
   	}
 	//fill result histogram
	resulthisto->SetBinContent(iLB,content);
	iLB--;
       }

       
    ERS_DEBUG(1,"Found " << countRed << " red bins and " << countYellow << " red bins. In a window of size " << window_size << " bins, starting at bin " << i_currentLB);
    ERS_DEBUG(1,"To be compared with: " << n_bins);
    ERS_DEBUG(1,"Green treshold=" << gthreshold << " Red threshold=" << rthreshold );

  result->tags_["NRedBins"] = countRed;
  result->tags_["NYellowBins"] = countYellow;
  result->object_ =  (std::auto_ptr<TObject>)(TObject*)(resulthisto);
  if(countRed>=n_bins)
    {
      result->status_ = dqm_core::Result::Red;
    } 
  else if(countRed+countYellow>=n_bins)
    {
      result->status_ = dqm_core::Result::Yellow;
    } 
  else 
    {
      result->status_ = dqm_core::Result::Green;
    }
     
  return result;
  
}

dqm_algorithms::BinHeightThreshold::binStatus
dqm_algorithms::BinHeightThreshold::CompareBinHeightThreshold(double bincontent, double thresholdGr, double thresholdRed) {
if(thresholdRed!=-1)
  {
    if(bincontent>=thresholdRed)
	return dqm_algorithms::BinHeightThreshold::binStatus::aRedBin;
    if(bincontent>=thresholdGr)
	return dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin;  
    return dqm_algorithms::BinHeightThreshold::binStatus::aGreenBin;
  }
else
  {
    if(bincontent==thresholdRed)
       return dqm_algorithms::BinHeightThreshold::binStatus::aRedBin;
    if(bincontent==thresholdGr)
        return dqm_algorithms::BinHeightThreshold::binStatus::aGreenBin;
    if(bincontent>thresholdGr) 
      return dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin;
    return dqm_algorithms::BinHeightThreshold::binStatus::anUndefBin;
  }
}


void
dqm_algorithms::BinHeightThreshold::printDescription(std::ostream& out)
{
  out<<"BinHeightThreshold is built to monitor trigger pre-scale (PS) as a function of lumi-block (LB). It only works with TH1 as input. LB is expected to be on x axis, PS is the bin content. It defines 'red' and 'yellow' bins depending on the value of the bin content. The algorithm looks for a set of consecutive bins which:\n \t-include the current LB (i.e. the nonzero bin at largest x).\n \t-are all 'red' or all 'yellow'.\n If the bin set is found and it is longer than Nbins, then the algorithm returns 'Red' for a set of 'red' bins, 'Yellow' for a set of 'yellow' bins.\n"<<std::endl;
  out<<"Mandatory Parameter: NBins: number of consecutive red(yellow) bins over which the algorithm returns Red(Yellow).\n"<<std::endl;
  out<<"Optional Parameter: NRedBins: if set, replace NBins in the case of a set of red bins. To be used if different thresholds are desired for red and yellow bin sets.\n"<<std::endl;
  out<<"Mandatory Parameter: RedThreshold: used in defining 'red'/'yellow'/'green' bins (see the following).\n"<<std::endl; 
  out<<"Mandatory Parameter: GreenThreshold: used in defining 'red'/'yellow'/'green' bins (see the following).\n"<<std::endl;
  out<<"'red'/'yellow'/'green' bins are defined depending on the RedThreshold and GreenThreshold parameters.\n If RedThreshold==-1:\n \t BinContent==RedThreshold --> red bin (i.e. trigger disabled)\n \t BinContent==GreenThreshold --> green bin\n \t BinContent>GreenThreshold --> yellow bin (i.e. trigger pre-scaled). \n If RedThreshold>0:\n \t BinContent<GreenThreshold --> green bin\n \t GreenThreshold<=BinContent<RedThreshold --> yellow bin\n \t BinContent>=RedThreshold --> red bin\n"<<std::endl; 
}

