/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MDTMultiplicity.cxx,v 2.0 2008/10/08 Valerio Consorti
// **********************************************************************

#include "dqm_algorithms/MDTMultiplicity.h"

#include <cmath>
#include <iostream>
#include <map>
#include <vector>
#include <string>

#include <TClass.h>
#include <TH1.h>
#include <TF1.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"


static dqm_algorithms::MDTMultiplicity staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

MDTMultiplicity::MDTMultiplicity()
  : m_name("MDTmultiplicity")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


MDTMultiplicity::~MDTMultiplicity()
{
}


dqm_core::Algorithm*
MDTMultiplicity::clone()
{
  return new MDTMultiplicity(*this);
}


dqm_core::Result*
MDTMultiplicity::execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
{
  using namespace std;

  TH1 * hist;
  TH1 * ref;
  
  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    hist = (TH1*)&object;
    if (hist->GetDimension() >= 2 ){
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension >= 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }   

  //Get Reference Histo
 
  try {
    ref = static_cast<TH1 *>( config.getReference() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
  }
  if (hist->GetDimension() != ref->GetDimension() ) {
    throw dqm_core::BadRefHist( ERS_HERE, name, "Reference VS histo: Different dimension!" );
  } 
   
  //Get Parameters and Thresholds
  double minstat; 
  double thresh_on_difference=75;
  double thresh_on_peak_content=15;
  //double peak_number=2;
  double pos1=3;
  double pos2=6;
  double pos3=-1;
  double greenTh=1;
  double redTh=2;
  double ref_y_n=1;

  try {
    minstat      		= dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), 100);
    thresh_on_difference	= dqm_algorithms::tools::GetFirstFromMap("IsolationThresh", config.getParameters(), 75);
    thresh_on_peak_content	= dqm_algorithms::tools::GetFirstFromMap("StatisticThresh", config.getParameters(), 15);
    //peak_number      		= dqm_algorithms::tools::GetFirstFromMap("NumberOfPeaks", config.getParameters());
    pos1	      		= dqm_algorithms::tools::GetFirstFromMap("FirstPeakPosition", config.getParameters(), 4);
    pos2	      		= dqm_algorithms::tools::GetFirstFromMap("SecondPeakPosition", config.getParameters(), 8);
    pos3	      		= dqm_algorithms::tools::GetFirstFromMap("ThirdPeakPosition", config.getParameters(), -1);
    ref_y_n	      		= dqm_algorithms::tools::GetFirstFromMap("UseRef", config.getParameters(), 0);
    redTh         = dqm_algorithms::tools::GetFromMap( "PeakShift", config.getRedThresholds());
    greenTh       = dqm_algorithms::tools::GetFromMap( "PeakShift", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

 //Check of statistics
  if (hist->GetEntries() < minstat ) {
    ERS_INFO("Histogram does not satisfy MinStat requirement " <<hist->GetName());
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = hist->GetEntries();
    return result;
  }
  ERS_DEBUG(1,"Statistics: "<< hist->GetEntries()<< " entries ");

 //Algo
  unsigned int i=0;
  unsigned int k=0;
  vector<double> derivate;
  vector<double> peak_candidate;
  vector<double> peak;  
  vector<double> valley;

  vector<double> derivate_ref;
  vector<double> peak_candidate_ref;
  vector<double> peak_ref;  
  vector<double> valley_ref;

  //Filling vector of derivate
  for(i=1;i<=(unsigned int)hist->GetNbinsX();i++){
    if((i+1)<=(unsigned int)hist->GetNbinsX()) derivate.push_back(hist->GetBinContent(i+1)-hist->GetBinContent(i));
    if((i+1)<=(unsigned int)ref->GetNbinsX() && ref_y_n==1) derivate_ref.push_back(ref->GetBinContent(i+1)-ref->GetBinContent(i));
  };

  i=0;

  //Look for peaks and valley
  Double_t highest_peak=0;
  Double_t peak_content;
  Double_t first_valley_content;
  Double_t second_valley_content;
  Double_t highest_peak_ref=0;
  Double_t peak_content_ref;
  Double_t first_valley_content_ref;
  Double_t second_valley_content_ref;

  if(derivate[0]>=0) valley.push_back(0);
  if(derivate[0]<0){
    peak_candidate.push_back(0);
    highest_peak=hist->GetBinContent(1);
  }
  if(ref_y_n==1){
    if(derivate_ref[0]>=0) valley_ref.push_back(0);
    if(derivate_ref[0]<0){
      peak_candidate_ref.push_back(0);
      highest_peak_ref=ref->GetBinContent(1);
    }
  };

  for(i=0;i<(derivate.size()-1);i++){
    if(derivate[i]>0 && derivate[i+1]<0){
      peak_candidate.push_back(i+1);
      peak_content=hist->GetBinContent(i+2);
      if(peak_content>highest_peak) highest_peak=peak_content;
    }else if(derivate[i]<0 && derivate[i+1]>0){
      valley.push_back(i+1);
    };

    if(ref_y_n==1){
      if(derivate_ref[i]>0 && derivate_ref[i+1]<0){
      peak_candidate_ref.push_back(i+1);
      peak_content_ref=ref->GetBinContent(i+2);
      if(peak_content_ref>highest_peak_ref) highest_peak_ref=peak_content_ref;
      }else if(derivate_ref[i]<0 && derivate_ref[i+1]>0){
        valley_ref.push_back(i+1);
      };
    };
  };

  valley.push_back(hist->GetNbinsX());
  valley_ref.push_back(hist->GetNbinsX());

  //Peak Definition (isolation and high statistic)
  i=0;

  for(i=0;i<peak_candidate.size();i++){
    peak_content=hist->GetBinContent((Int_t)peak_candidate[i]+1);
    first_valley_content=hist->GetBinContent((Int_t)valley[i]+1);
    second_valley_content=hist->GetBinContent((Int_t)valley[i+1]+1);
    if(first_valley_content==0) first_valley_content=1;
    if(second_valley_content==0) second_valley_content=1;

    if((first_valley_content<=second_valley_content) && (peak_content>(thresh_on_peak_content*highest_peak/100)) && (first_valley_content<(thresh_on_difference*peak_content/100))){
      peak.push_back(peak_candidate[i]);
    }else if((first_valley_content>second_valley_content) && (peak_content>(thresh_on_peak_content*highest_peak/100)) && (second_valley_content<(thresh_on_difference*peak_content/100))){
      peak.push_back(peak_candidate[i]);
    };
  };

  i=0;
  if(ref_y_n==1){
    for(i=0;i<peak_candidate_ref.size();i++){
      peak_content_ref=ref->GetBinContent((Int_t)peak_candidate_ref[i]+1);
      first_valley_content_ref=ref->GetBinContent((Int_t)valley_ref[i]+1);
      second_valley_content_ref=ref->GetBinContent((Int_t)valley_ref[i+1]+1);
      if(first_valley_content_ref==0) first_valley_content_ref=1;
      if(second_valley_content_ref==0) second_valley_content_ref=1;

      if((first_valley_content_ref<=second_valley_content_ref) && (peak_content_ref>(thresh_on_peak_content*highest_peak_ref/100)) && (first_valley_content_ref<(thresh_on_difference*peak_content_ref/100))){
        peak_ref.push_back(peak_candidate_ref[i]);
      }else if((first_valley_content_ref>second_valley_content_ref) && (peak_content_ref>(thresh_on_peak_content*highest_peak_ref/100)) && (second_valley_content_ref<(thresh_on_difference*peak_content_ref/100))){
        peak_ref.push_back(peak_candidate_ref[i]);
      };
    };
  };

 //Result

  dqm_core::Result* result = new dqm_core::Result(); 

  double count=0;
  //double first_peak=-1;
  //double second_peak=-1;
  //double third_peak=-1;
  double diff1=9999;
  double diff2=9999;
  double diff3=9999;

  if(ref_y_n==0){
    i=0;
    for(i=0;i<peak.size();i++){
      if(fabs(peak[i]-pos1)<=diff1){
        //first_peak=i;
        diff1=fabs(peak[i]-pos1);
      };
      if(fabs(peak[i]-pos2)<=diff2){
        //second_peak=i;
        diff2=fabs(peak[i]-pos2);
      };
      if(fabs(peak[i]-pos3)<=diff3 && pos3>0){
        //third_peak=i;
        diff3=fabs(peak[i]-pos3);
      };
    };
    //if(peak.size() != peak_number) count = 10;
    if(diff1>greenTh) count++;
    if(diff2>greenTh) count++;
    if(diff3>greenTh && pos3>0) count++;
    if(diff1>redTh) count+=2;
    if(diff2>redTh) count+=2;
    if(diff3>redTh && pos3>0) count+=2; 

    i=0;
    string tag_n_r;
    string peak_tag = "-Peak";
    char numb_n_r[3];

    result->tags_["00-Number_of_found_peaks"] = peak.size();
    for(i=0;i<peak.size();i++){
      if(i<10) sprintf(numb_n_r,"0%d",(i+1));
      if(i>=10) sprintf(numb_n_r,"%d",(i+1));
      tag_n_r=(std::string)numb_n_r+peak_tag;
      result->tags_[tag_n_r] = peak[i];
    };
  };

  if(ref_y_n==1){
    i=0;
    k=0;
    vector<double> diff;
    vector<double> peak_corrispondence;
    for(i=0;i<peak.size();i++){
      diff1=9999;
      peak_corrispondence.push_back(99999);
      diff.push_back(99999);
      for(k=0;k<peak_ref.size();k++){
        if(fabs(peak[i]-peak_ref[k])<=diff1){
          peak_corrispondence[i]=k;
          diff[i]=fabs(peak[i]-peak_ref[k]);
          diff1=fabs(peak[i]-peak_ref[k]);
        };
      };
    };
    i=0;
    for(i=0;i<peak.size();i++){
      if(diff[i]>greenTh) count++;
      if(diff[i]>redTh) count+=2;
    };

    //if(peak.size()!=peak_ref.size()) count+=10;

    string tag;
    string run_tag ="a-";
    string ref_tag ="b-";
    string multi = "Peak";
    char numb[3];

    result->tags_["a-00-Number_of_found_peaks"] = peak.size();
    result->tags_["b-00-Number_of_found_peaks_ref"] = peak_ref.size();

    for(i=0;i<peak.size();i++){
      if(i<10) sprintf(numb,"0%d",(i+1));
      if(i>=10) sprintf(numb,"%d",(i+1));
      tag=run_tag+(std::string)numb+multi;
      result->tags_[tag] = peak[i];
    };
    i=0;
    for(i=0;i<peak_ref.size();i++){
      if(i<10) sprintf(numb,"0%d",(i+1));
      if(i>=10) sprintf(numb,"%d",(i+1));
      tag=ref_tag+(std::string)numb+multi;
      result->tags_[tag] = peak_ref[i];
    };
  };

  if(count==0){
    result->status_ = dqm_core::Result::Green;
  };
  if(count>=1 && count<3){
    result->status_ = dqm_core::Result::Yellow;
  };
  if(count>=3){
    result->status_ = dqm_core::Result::Red;
  };

  // Return the result
  return result;
}


void
MDTMultiplicity::printDescription(std::ostream& out){
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: it look for peaks in the histogram and it compare their position with user defined position or with reference histogram peak position. The algorithm define a peak first looking for the higher statistic bin then it look for other bins with at last a user defined percent of the entrance of the maximum bin  \n";
  message += "Green/Red Thresh: PeakShift: the number of bins tollerated for the shift of a peak\n";
  message += "Mandatory Params: MinStat: minimum statistical requirement\n";
  message += "                  IsolationThresh: The alghorithm need it to define a peak. It define the minumum difference,in percent, between the peak entrance and the closer 'valley' entrance(valley is defined as the bin where the derivate change sign from negative to positive).\n";
  message += "                  StatisticThresh: The minimum bin entrance required expressed in percent compared with the maximum bin. If the bin has less then this percent of entrance it can't be defined as a peak.\n";
  message += "                  FirstPeakPosition: Mandatory in no reference mode. It is the position expected for the first peak\n";
  message += "                  SecondPeakPosition: Mandatory in no reference mode. It is the position expected for the second peak\n";
  message += "                  ThirdPeakPosition: Mandatory in no reference mode. It is the position expected for the third peak\n";
  message += "                  UseRef: 1 to use a reference histogram, 0 to use user definition for the peaks position without a reference comparison\n";
  message += "Optional Params: NumberOfPeaks: In the no reference mode it define how many peaks the algorithm has to looking for. if it find more then this number of peack it set the flag red\n";
  out << message;
}

} // namespace dqm_algorithms
