/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinThreshold.cxx checks bins wrt to a threshold value and returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BinThreshold.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>

#include <iostream>

#include <dqm_core/AlgorithmManager.h>

namespace
{
  dqm_algorithms::BinThreshold GreaterThan( "GreaterThan" );
  dqm_algorithms::BinThreshold GreaterThanAbs( "GreaterThanAbs" );
  dqm_algorithms::BinThreshold GreaterThanNonZeroMedian( "GreaterThanNonZeroMedian" );
  dqm_algorithms::BinThreshold GreaterThanEqual( "GreaterThanEqual" );
  dqm_algorithms::BinThreshold LessThan( "LessThan" );
  dqm_algorithms::BinThreshold LessThanAbs( "LessThanAbs" );
  dqm_algorithms::BinThreshold LessThanNonZeroMedian( "LessThanNonZeroMedian" );
  dqm_algorithms::BinThreshold LessThanEqual( "LessThanEqual" );
  dqm_algorithms::BinThreshold Equal( "Equal" );
  dqm_algorithms::BinThreshold NotEqual( "NotEqual" );
}


dqm_algorithms::BinThreshold::BinThreshold( const std::string & name )
  : m_name( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("Bins_"+name+"_Threshold", this);
}

dqm_algorithms::BinThreshold * 
dqm_algorithms::BinThreshold::clone()
{
  
  return new BinThreshold( m_name );
}


dqm_core::Result *
dqm_algorithms::BinThreshold::execute(	const std::string &  name, 
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
  
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 0); 
  const int maxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20); 
  
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  double bin_threshold;
  double gthreshold;
  double rthreshold;
  try {
    bin_threshold = dqm_algorithms::tools::GetFirstFromMap( "BinThreshold", config.getParameters() );
    rthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  
  int count = 0; 
  std::vector<int> range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  dqm_core::Result* result = new dqm_core::Result();
  TH1* resulthisto;
  if (histogram->InheritsFrom("TH2")) {
	resulthisto=(TH1*)(histogram->Clone());
  } else if (histogram->InheritsFrom("TH1")) {
        resulthisto=(TH1*)(histogram->Clone());
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
 
  resulthisto->Reset();
  if(m_name.find("Median")!=std::string::npos){
    std::vector<double> bin_vals;
    for ( int i = range[0]; i <= range[1]; ++i ) {
      for ( int j = range[2]; j <= range[3]; ++j ) {
	if(histogram -> GetBinContent(i,j) > 0) bin_vals . push_back( histogram -> GetBinContent(i,j) );
      }
    }
    std::sort(bin_vals.begin(), bin_vals.end());
    unsigned bin_vals_size = bin_vals.size();
    double median = 0;
    if(bin_vals_size%2==0 && bin_vals_size>1) median = (bin_vals.at(bin_vals_size/2-1)+bin_vals.at(bin_vals_size/2))/2;
    else if(bin_vals_size%2==1 && bin_vals_size>1) median = bin_vals.at(bin_vals_size/2-1);
    else if(bin_vals_size==1) median = bin_vals.at(0);

    double min_bin_threshold = dqm_algorithms::tools::GetFirstFromMap( "MinBinThreshold", config.getParameters() , -99999);
    double max_bin_threshold = dqm_algorithms::tools::GetFirstFromMap( "MaxBinTrheshold", config.getParameters() , -99999);
    bin_threshold=median*bin_threshold;

    bin_threshold = std::max(bin_threshold, min_bin_threshold);
    if(max_bin_threshold > -1 ) bin_threshold = std::min(bin_threshold, max_bin_threshold);

    result->tags_["Effective_BinThreshold"] = bin_threshold;
  }

  for ( int i = range[0]; i <= range[1]; ++i ) {
    for ( int j = range[2]; j <= range[3]; ++j ) {
      double content= histogram -> GetBinContent(i,j);
      if ( CompareBinThreshold(m_name, content, bin_threshold )) {
	++count;
          resulthisto->SetBinContent(i,j,content);
	  if (publish && count< maxpublish){
           dqm_algorithms::tools::PublishBin(histogram,i,j,content,result);
	  }
      }
    }
  }

  ERS_DEBUG(1,"Number of bins " << m_name << " treshold of " << bin_threshold << " is " << count );
  ERS_DEBUG(1,"Green threshold: "<< gthreshold << " bin(s);   Red threshold : " << rthreshold << " bin(s) ");    
  
  int TotalBins = (int) dqm_algorithms::tools::GetFirstFromMap( "TotalBins", config.getParameters() , -99999);
  if(TotalBins > -10){
    if(TotalBins < 0) TotalBins = histogram->GetNbinsX()*histogram->GetNbinsY()*histogram->GetNbinsZ();
    int effectiveCount = count - (histogram->GetNbinsX()*histogram->GetNbinsY()*histogram->GetNbinsZ() - TotalBins);
    if(m_name.find("LessThan")!=std::string::npos) result->tags_["NBins_%"] = 100.*effectiveCount/TotalBins;
    else result->tags_["NBins_%"] = 100.*count/TotalBins;
  }

  result->tags_["NBins"] = count;
  result->object_ =  (std::auto_ptr<TObject>)(TObject*)(resulthisto);
  if (gthreshold > rthreshold) {
     if ( count >= gthreshold ) {
        result->status_ = dqm_core::Result::Green;
     } else if ( count > rthreshold ) {
        result->status_ = dqm_core::Result::Yellow;
     } else {
        result->status_ = dqm_core::Result::Red;
     }
  } else {
     if ( count <= gthreshold ) {
        result->status_ = dqm_core::Result::Green;
     } else if ( count < rthreshold ) {
        result->status_ = dqm_core::Result::Yellow;
     } else {
        result->status_ = dqm_core::Result::Red;
     }
  }
  
  return result;
  
}

bool
dqm_algorithms::BinThreshold::CompareBinThreshold(const std::string & type, double bincontent, double threshold) {
  
  if (type == "GreaterThan") return (bincontent > threshold);
  if (type == "GreaterThanAbs") return (fabs(bincontent) > threshold);
  if (type == "GreaterThanNonZeroMedian") return (bincontent > threshold);
  if (type == "LessThan") return (bincontent < threshold);
  if (type == "LessThanAbs") return (fabs(bincontent) < threshold);
  if (type == "LessThanNonZeroMedian") return (bincontent < threshold);
  if (type == "LessThanEqual") return (bincontent <= threshold);
  if (type == "GreaterThanEqual") return (bincontent >= threshold);
  if (type == "Equal") return (bincontent == threshold);
  if (type == "NotEqual") return (bincontent != threshold);

  return 0;
}


void
dqm_algorithms::BinThreshold::printDescription(std::ostream& out)
{
  
  out<<"Bins_"+m_name+"_Threshold: Checks for number of bins "+m_name+" threshold value\n"<<std::endl;
  
  out<<"Mandatory Parameter: BinThreshold: Look for bins "+m_name+" BinTreshold; Count number of bins satifying requirement \n"<<std::endl;
  
  out<<"Mandatory Green/Red Threshold: NBins: Number of bins satifying "+m_name+" BinThreshold constraint to give Green/Red result\n"<<std::endl;
  
  out<<"Optional Parameter: PublishBins: Save bins which are different from average in Result (set to 1)\n"<<std::endl; 
  out<<"Optional Parameter: MaxPublish: Max number of bins to save (default 20)"<<std::endl; 
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: xmin: minimum x range"<<std::endl;
  out<<"Optional Parameter: xmax: maximum x range"<<std::endl;
  out<<"Optional Parameter: ymin: minimum y range"<<std::endl;
  out<<"Optional Parameter: ymax: maximum y range\n"<<std::endl;
  
  
}

