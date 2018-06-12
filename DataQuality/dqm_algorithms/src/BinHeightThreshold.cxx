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


//static dqm_algorithms::BinHeightThreshold myInstance;
namespace
{
  dqm_algorithms::BinHeightThreshold HeightGreaterThan( "GreaterThan" );
  dqm_algorithms::BinHeightThreshold HeightGreaterThanEqual( "GreaterThanEqual" );
  dqm_algorithms::BinHeightThreshold HeightLessThan( "LessThan" );
  dqm_algorithms::BinHeightThreshold HeightLessThanEqual( "LessThanEqual" );
  dqm_algorithms::BinHeightThreshold HeightRedEqYellowGT( "redEqual_yellowGreaterThan" );
  dqm_algorithms::BinHeightThreshold HeightRedEqYellowLT( "redEqual_yellowLessThan" );
  dqm_algorithms::BinHeightThreshold HeightEqual( "Equal" );
}

dqm_algorithms::BinHeightThreshold::BinHeightThreshold( const std::string & name )
  : name_( name )
{
  precision_=1e-4;
  dqm_core::AlgorithmManager::instance().registerAlgorithm("BinHeight_"+name+"_Threshold", this);
}

dqm_algorithms::BinHeightThreshold * 
dqm_algorithms::BinHeightThreshold::clone()
{  
  return new BinHeightThreshold(name_);
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
  std::pair<bool,double> grayValue;
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
  grayValue.first=true;
  try {
    grayValue.second = dqm_algorithms::tools::GetFirstFromMap( "UndefinedStatus", config.getParameters() );
  }
  catch ( dqm_core::Exception & ex ) {
    grayValue.first=false;
    grayValue.second=-1;
  }
  try {
    precision_ = dqm_algorithms::tools::GetFirstFromMap( "EqualityPrecision", config.getParameters(),1e-4);
  }
  catch ( dqm_core::Exception & ex ) {
    precision_=1e-4;
  }


  //check if the provided parameter values make sense
  if(precision_<0) {
    ERS_INFO("'EqualityPrecision cannot be negative: it will be re-set to its absolute value.");
    precision_=fabs(precision_);
  }
  if(window_size<=0) {
      ERS_INFO("You set search window size (WindowSize) <= 0: I will search the whole histogram.");
    }
  else {
    if(n_bins>window_size) {
      ERS_INFO("You set the minimum number of bins for throwing error/warning (NBins) larger than the window size (WindowSize): in this way the algorithm can never return error/warning. Setting NBins=WindowSize.");
      n_bins=window_size;
    }
  }
  if(n_bins<=0)  {
    ERS_INFO("You set the minimum number of bins for throwing error/warning (NBins) <= 0: in this way the algorithm would always return error. Setting NBins=1 (default value).");
    n_bins=1;
  }
  CheckThresholds(name_,gthreshold,rthreshold);
  grayValue.first=checkUndefinedStatusValue(name_,gthreshold,rthreshold,grayValue);

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
  //if the histogram is just empty, or still has too few bins, do nothing 
   if(i_currentLB<=0 || i_currentLB<n_bins)
    {
     ERS_DEBUG(1,"start_from_last parameter >= total number of bins, I just cannot do the check. Do nothing.");
     return result;
    }

   //now loop over an x window of size 'window_size'
    int iLB=i_currentLB;
    int countYellow=0;
    int countRed=0;
    int countGray=0;
    while(iLB>=1 && (window_size<0 || (i_currentLB-iLB)<window_size))
       {
	double content=histogram->GetBinContent(iLB);
	dqm_algorithms::BinHeightThreshold::binStatus LBstatus=CompareBinHeightThreshold(name_,content, gthreshold , rthreshold,grayValue);
	if(LBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin) 
	{
	  countYellow++;
   	}
	if(LBstatus==dqm_algorithms::BinHeightThreshold::binStatus::aRedBin) 
	{
	  countRed++;
   	}
	if(LBstatus==dqm_algorithms::BinHeightThreshold::binStatus::anUndefBin) 
	{
	  countGray++;
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
  else if(countGray>=n_bins)
    {
      result->status_ = dqm_core::Result::Undefined;
    }
  else
    {
      result->status_ = dqm_core::Result::Green;
    }
     
  return result;
  
}


void dqm_algorithms::BinHeightThreshold::CheckThresholds(const std::string & type, double thresholdGr, double thresholdRed) {
  if(type=="GreaterThan" || type=="GreaterThanEqual")
    {
      if(thresholdGr>=thresholdRed)
	ERS_INFO("'BinHeight_" << type << "_Threshold' algorithm expects red > yellow > green. You set the warning threshold (" << thresholdGr << ") >= error threshold (" << thresholdRed << "): it will never return 'yellow/warning'.");
    }
  if(type=="LessThan" || type=="LessThanEqual")
    {
      if(thresholdGr<=thresholdRed)
	ERS_INFO("'BinHeight_" << type << "_Threshold' algorithm expects red < yellow < green. You set the warning threshold (" << thresholdGr << ") <= error threshold (" << thresholdRed << "): it will never return 'yellow/warning'. Are you sure this is what you want?");
    }
  if(type=="redEqual_yellowGreaterThan")
    {
      if(thresholdRed>0 && thresholdRed<thresholdGr)
	ERS_INFO("You set the error threshold (" << thresholdRed << ") between zero and the warning threshold (" << thresholdGr << ") in 'BinHeight_redEqual_yellowGreaterThan_Threshgold' algorithm. Are you sure this is what you want?");
    }
  if(type=="redEqual_yellowLessThan")
    {
      if(thresholdRed>thresholdGr)
	ERS_INFO("You set the error threshold (" << thresholdRed << ") larger than the warning threshold (" << thresholdGr << ") in 'BinHeight_redEqual_yellowLessThan_Threshold' algorithm. Are you sure this is what you want?");
    }
}

bool 
dqm_algorithms::BinHeightThreshold::checkUndefinedStatusValue(const std::string & type,double thresholdGr, double thresholdRed,std::pair<bool,double> valueGray) {
  if(!valueGray.first)
    return valueGray.first;
    
  if(type=="LessThan" || type=="GreaterThan")
    return valueGray.first;

  if(equalWithinPrecision(valueGray.second,thresholdRed))
    {
      ERS_INFO("You have set 'UndefinedStatus' equal to the error threshold in 'BinHeight_" << type << "_Threshold' algorithm. Error has the precedence here: the bin content WILL NOT be checked against 'UndefinedStatus'");
      return false;
    }
    
  if(type!="redEqual_yellowLessThan" && type!="redEqual_yellowGreaterThan" && equalWithinPrecision(valueGray.second,thresholdGr))
    {
      ERS_INFO("You have set 'UndefinedStatus' equal to the warning threshold in 'BinHeight_" << type << "_Threshold' algorithm. Warning has the precedence here: the bin content WILL NOT be checked against 'UndefinedStatus'");
      return false;
    }

  return valueGray.first;
}



dqm_algorithms::BinHeightThreshold::binStatus
dqm_algorithms::BinHeightThreshold::CompareBinHeightThreshold(const std::string & type,double bincontent, double thresholdGr, double thresholdRed,std::pair<bool,double> valueGray) {
  if(valueGray.first)
    {
      if(equalWithinPrecision(bincontent,valueGray.second))
	{
	  return dqm_algorithms::BinHeightThreshold::binStatus::anUndefBin;
	}
    }

  if(type=="GreaterThan")
    {
      if(bincontent>thresholdRed)
	return dqm_algorithms::BinHeightThreshold::binStatus::aRedBin;
      if(bincontent>thresholdGr)
	return dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin;  
      return dqm_algorithms::BinHeightThreshold::binStatus::aGreenBin;
    }
  if(type=="LessThan")
    {
      if(bincontent<thresholdRed)
	return dqm_algorithms::BinHeightThreshold::binStatus::aRedBin;
      if(bincontent<thresholdGr)
	return dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin;  
      return dqm_algorithms::BinHeightThreshold::binStatus::aGreenBin;
    }
  if(type=="GreaterThanEqual")
    {
      if(bincontent>thresholdRed || equalWithinPrecision(bincontent,thresholdRed))
	return dqm_algorithms::BinHeightThreshold::binStatus::aRedBin;
      if(bincontent>thresholdGr || equalWithinPrecision(bincontent,thresholdGr))
	return dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin;  
      return dqm_algorithms::BinHeightThreshold::binStatus::aGreenBin;
    }
  if(type=="LessThanEqual")
    {
      if(bincontent<thresholdRed || equalWithinPrecision(bincontent,thresholdRed))
	return dqm_algorithms::BinHeightThreshold::binStatus::aRedBin;
      if(bincontent<thresholdGr || equalWithinPrecision(bincontent,thresholdGr))
	return dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin;  
      return dqm_algorithms::BinHeightThreshold::binStatus::aGreenBin;
    }
  if(type=="redEqual_yellowGreaterThan")
    {
      if(equalWithinPrecision(bincontent,thresholdRed))
	return dqm_algorithms::BinHeightThreshold::binStatus::aRedBin;
      if(bincontent>thresholdGr) 
	return dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin;
      return dqm_algorithms::BinHeightThreshold::binStatus::aGreenBin;
    }
  if(type=="redEqual_yellowLessThan")
    {
      if(equalWithinPrecision(bincontent,thresholdRed))
	return dqm_algorithms::BinHeightThreshold::binStatus::aRedBin;
      if(bincontent<thresholdGr) 
	return dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin;
      return dqm_algorithms::BinHeightThreshold::binStatus::aGreenBin;
    }
  if(type=="Equal")
    {
      if(equalWithinPrecision(bincontent,thresholdRed))
	return dqm_algorithms::BinHeightThreshold::binStatus::aRedBin;
      if(equalWithinPrecision(bincontent,thresholdGr)) 
	return dqm_algorithms::BinHeightThreshold::binStatus::aYellowBin;
      return dqm_algorithms::BinHeightThreshold::binStatus::aGreenBin;
    }
  return dqm_algorithms::BinHeightThreshold::binStatus::anUndefBin;
}


bool 
dqm_algorithms::BinHeightThreshold::equalWithinPrecision(double a,double b)
{
  //relative difference method (following what suggested in (non-ATLAS) web page http://floating-point-gui.de/errors/comparison/)
  double absA = fabs(a);
  double absB = fabs(b);
  double diff = fabs(a - b);

  if (a == b) { // shortcut, handles infinities
    return true;
  } 
  else if (a == 0 || b == 0 || diff < DBL_MIN) {
    // a or b is zero or both are extremely close to it
    // relative error is less meaningful here
    return diff < (precision_ * DBL_MIN);
  } 
  else { // use relative error
    return (diff / std::min((absA + absB), DBL_MAX)) < precision_;
  }
}

void
dqm_algorithms::BinHeightThreshold::printDescription(std::ostream& out)
{
  TString redCond,yellowCond;
  if(name_=="redEqual_yellowGreaterThan" || name_=="redEqual_yellowLessThan" || name_=="Equal")
    {
      redCond="bin_content==redThreshold";
      if(name_=="Equal")
	yellowCond="bin_content==yellowThreshold && bin_content!=redThreshold";
      else if(name_=="redEqual_yellowGreaterThan")
	yellowCond="bin_content>yellowThreshold && bin_content!=redThreshold";
      else
	yellowCond="bin_content<yellowThreshold && bin_content!=redThreshold";
    }
  else
    {
      if(name_=="GreaterThan")
	{
	  redCond="bin_content>redThreshold";
	  yellowCond="redThreshold>=bin_content>yellowThreshold";
	}
      else if(name_=="LessThan")
	{
	  redCond="bin_content<redThreshold";
	  yellowCond="redThreshold<=bin_content<yellowThreshold";
	}
      else if(name_=="GreaterThanEqual")
	{
	  redCond="bin_content>=redThreshold";
	  yellowCond="redThreshold>bin_content>=yellowThreshold";
	}
      else //LessThenEqual is the only remaining
	{
	  redCond="bin_content<=redThreshold";
	  yellowCond="redThreshold<bin_content<=yellowThreshold";
	}
    }
  out << "BinHeight_" << name_ << "_Threshold checks the bin height of a TH1. Ideally, a quantity as a function of LB. LB is expected to be on x axis, the quantity of interest is the bin content." << std::endl;
  out << "BinHeight_" << name_ << "_Threshold defines 'red' and 'yellow' bins depending on the value of the bin content:\n \t-if " << redCond << ": the bin is 'red'.\n \t-if " << yellowCond << ": the bin is 'yellow'.\n \t-if (OPTIONAL) an 'UndefinedStatus' value is set and bin_content==UndefinedStatus, the bin is 'gray'.";
  if(name_!="GreaterThan" && name_!="LessThan")
    out << " Note that if 'UndefinedStatus' is equal to 'redThreshold' or 'yellowThreshold', the bin will be 'red'/'yellow' rather than 'gray'.";
  out << "\n \t-otherwise the bin is 'green'" << std::endl;
  out << "The algorithm checks all the bins in a window of size 'WindowSize', starting from:\n \t a) the last but X bin.\n \t b) the last non-zero bin.\n Oprion a) or b) is chosen by the parameter 'StartFromLast': if('StartFromLast'>=0), (a) holds and X is equal to 'StartFromLast', while (b) holds if 'StartFromLast'<0." << std::endl;
  out << "In the window of interest, the number of red/yellow/gray bins is counted, respectively Nred/Nyellow/Ngray. The output is then defined comparing these numbers against the parameter 'NBins':\n \t- if Nred>=NBins: returns 'RED'.\n \t- else, if (Nred+Nyellow)>=NBins: returns 'YELLOW'.\n \t- else, if Ngray>=NBins: returns 'GREY'.\n \t- else returns 'GREEN'." << std::endl;
  out << "NOTE: to avoid issues due to rounding in double precision, the equality between the bin content and any parameter is defined asking for the relative difference between the two to be smaller than a given parameter. I.e. content==Y is implemented as (abs(content-Y)/(content+Y))<epsilon. epsilon is equal to the parameter 'EqualityPrecision' which is tunable and set to 10^-4 by default\n"<<std::endl;

  out<<"Mandatory Parameter: HeightThreshold: sets the warning (yellowThreshold) and error (redThreshold) thresholds."<<std::endl; 
  out<<"Optional Parameter: NBins: minimum number of red/(yellow+red)/gray bins in the window of interest for the algorithm to return Red/Yellow/Grey. Default is 1."<<std::endl;
  out<<"Optional Parameter: WindowSize: size of the x-axis range (in number of bins) in which red/yellow/gray bins are searched for. If WindowSize<=0, the whole istogram is searched for. Default is 1."<<std::endl;
  out<<"Optional Parameter: StartFromLast: if StartFromLast=X with X>=0, the algorithm will check the bins starting from the last but X bin. If StartFromLast<0, it will start from the first bin of nonzero content. Default is -1."<<std::endl;
  out<<"Optional Parameter: UndefinedStatus: a bin is defined to be 'gray' if its content is equal to UndefinedStatus. If not set, bins are not check against it."<<std::endl;
  out<<"Optional Parameter: EqualityPrecision: sets the precision with which the bin content is defined to be 'equal' to a parameter, as described above. Default is 10^-4."<<std::endl;
}

