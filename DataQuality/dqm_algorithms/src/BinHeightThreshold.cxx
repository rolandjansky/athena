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
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }

  double bin_threshold_red;
  double bin_threshold_yellow; 
  double gthreshold;
  double rthreshold;
  try {
    bin_threshold_yellow = dqm_algorithms::tools::GetFirstFromMap( "NBins", config.getParameters() );
    bin_threshold_red = dqm_algorithms::tools::GetFirstFromMap( "NRedBins", config.getParameters() ,bin_threshold_yellow); //with this method, bin_threshold_red should be set ==bin_threshold_yellow if a 'NRedBins' option is not provided
    rthreshold = dqm_algorithms::tools::GetFromMap( "RedThreshold", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "GreenThreshold", config.getGreenThresholds() );
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

  int countConsec=0;
  //first, look for the last filled LB, i.e. the bin with nonzero content of largest x
  int i_currentLB=histogram->GetNbinsX();
  while(i_currentLB>=1)
    {
      if(histogram->GetBinContent(i_currentLB)!=0) break;
      i_currentLB--;
    }
  //if the histogram is just empty, do nothing (check already done, in principle should not happen)
   if(i_currentLB<=0)
    {
     ERS_DEBUG(1,"Histogram is empty");
     return result;
    }
   //check current LB
   double currentLBcont= histogram -> GetBinContent(i_currentLB);
   dqm_algorithms::BinHeightThreshold::binStatus currentLBstatus=CompareBinHeightThreshold(currentLBcont, gthreshold , rthreshold);
//if current LB is not green, check the number of consecutive non-green bins
   if(currentLBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin || currentLBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aRedBin) 
    {
    int iLB=i_currentLB;
    double content;
    dqm_algorithms::BinHeightThreshold::binStatus LBstatus;
    while(iLB>=1)
       {
	content=histogram->GetBinContent(iLB);
	LBstatus=CompareBinHeightThreshold(content, gthreshold , rthreshold);
	if(LBstatus==currentLBstatus) 
	{
	  countConsec++;
 	//fill result histogram
          resulthisto->SetBinContent(iLB,content);
   	}
	else break;
	iLB--;
       }
    }
       
    std::string thrName="RED/YELLOW";
    if(currentLBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin) thrName="YELLOW";
    if(currentLBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aRedBin) thrName="RED";
    ERS_DEBUG(1,"Found " << countConsec << " consecutive " << thrName << " bins");
    ERS_DEBUG(1,"Min. # of consecutive red bins for returning Red: " << bin_threshold_red);
    ERS_DEBUG(1,"Min. # of consecutive yellow bins for returning Yellow: " << bin_threshold_yellow);
    ERS_DEBUG(1,"Green treshold=" << gthreshold << " Red threshold=" << rthreshold );

  result->tags_["NBins"] = countConsec;
  result->object_ =  (std::auto_ptr<TObject>)(TObject*)(resulthisto);
  if(currentLBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aRedBin && countConsec>=bin_threshold_red) 
    {
      result->status_ = dqm_core::Result::Red;
    } 
  else if(currentLBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin && countConsec>=bin_threshold_yellow) 
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

