/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "dqm_algorithms/MDTOverview.h"

#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <string>

#include "TH2.h"
#include "TAxis.h"
#include "TClass.h"


#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"
#include "boost/format.hpp"

static dqm_algorithms::MDTOverview MDTOverview_Global( "MDTOverview_Global" );
static dqm_algorithms::MDTOverview MDTOverview_Station( "MDTOverview_Station" );


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

MDTOverview::MDTOverview(const std::string & name)
  : m_name ( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm(  name, this );
}


MDTOverview::~MDTOverview()
{
}


dqm_core::Algorithm*
MDTOverview::clone()
{
  return new MDTOverview(m_name);
}


dqm_core::Result*
MDTOverview::execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
{
  TH2 * hist;
  TH2 * ref;
  
  if( object.IsA()->InheritsFrom( "TH2" ) ) {
    hist = (TH2*)&object;
    if (hist->GetDimension() != 2 ){
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension != 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH2" );
  }   
  
  //Get Parameters and Thresholds

  double greenTh;
  double redTh;
  double minstat; 
  double thresh;
  double using_ref_for_flag;
  std::string thresholdname;
  if (m_name ==  "MDTOverview_Global") {
    thresholdname="Percentage";
  } else if (m_name ==  "MDTOverview_Station") {
    thresholdname="Limits";
  } else {
    throw dqm_core::BadConfig( ERS_HERE, "None", m_name );
  }

  try {
    minstat            = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), 5000);
    thresh             = dqm_algorithms::tools::GetFirstFromMap("thresh", config.getParameters(), 10);
    using_ref_for_flag = dqm_algorithms::tools::GetFirstFromMap("using_ref_for_flag", config.getParameters(), 0);
    redTh       = dqm_algorithms::tools::GetFromMap( thresholdname, config.getRedThresholds());
    greenTh     = dqm_algorithms::tools::GetFromMap( thresholdname, config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  //Get Reference Histo
  try {
    ref = static_cast<TH2 *>( config.getReference() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
  }
  if (hist->GetDimension() != ref->GetDimension() ) {
    throw dqm_core::BadRefHist( ERS_HERE, name, "Reference VS histo: Different dimension!" );
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
  int binX = hist->GetNbinsX();
  int binY = hist->GetNbinsY();
  double nML=0;

  std::list<int> hist_buffer;
  std::list<int> ref_buffer;

  for(int x_index=1;x_index<=binX;x_index++){
    for(int y_index=1;y_index<=binY;y_index++){
      if( hist->GetBinContent(x_index,y_index) != 0 ) hist_buffer.push_back((int)hist->GetBinContent(x_index,y_index));
      if( ref->GetBinContent(x_index,y_index) != 0 ) ref_buffer.push_back((int)ref->GetBinContent(x_index,y_index));
    };
  };

  const char *htitle = hist->GetTitle();
  ERS_DEBUG(1,"Histogram title: "<< htitle);

  if (m_name ==  "MDTOverview_Global") {
    //GLOBAL   htitle="Global HitsPerML Endcap"
    //if (strstr(htitle, "Global")) {
    if (strstr(htitle, "BA")) {
      nML = 594; //4 BOG0 included
    } else if (strstr(htitle, "BC")) {
      nML = 590;
    } else if (strstr(htitle, "EA")) {
      nML = 502; // 16 BIS7 + 8 BIS8 +16 BEE + 10 EEL included
    } else if (strstr(htitle, "EC")) {
      nML = 502; // BIS7/8 + BEE + EEL included
    }
    
  } else  if (m_name ==  "MDTOverview_Station") {
    // Barrel  htitle="HitsPerML_BX_Station"
    if (strstr(htitle, "B")) {
      if (strstr(htitle,"Inner")) {
	ERS_DEBUG(1,"Station Inner ");
	nML = 212;
      } else if (strstr(htitle,"Middle")) {
	ERS_DEBUG(1,"Station Middle ");
	nML = 178;
      } else if (strstr(htitle,"Outer")) {
	ERS_DEBUG(1,"Station Outer ");
	if (strstr(htitle, "BA")) {
	  nML = 204; // BOG0 included
	} else if (strstr(htitle, "BC")) {
	  nML = 200;
	}
      }
    // Endcap htitle="HitsPerML_EX_Station"
    } else if (strstr(htitle, "E")) {
      
      if (strstr(htitle,"Inner")) {
	ERS_DEBUG(1,"Station Inner ");
	nML = 124;// 16 BIS7 + 8 BIS8 included
      } else if (strstr(htitle,"Middle")) {
	ERS_DEBUG(1,"Station Middle ");
	nML = 160;
      } else if (strstr(htitle,"Outer")) {
	ERS_DEBUG(1,"Station Outer ");
	nML = 192;
      } else if (strstr(htitle,"extra")) {
	ERS_DEBUG(1,"Extra chambers (BEE, EEL, BIS7, BIS8)");
	nML = 26;// 16 BEE + 10 EEL included
      }
    }
  }

  hist_buffer.sort();
  ref_buffer.sort();

  std::list<int>::iterator it_hist;
  std::list<int>::iterator it_ref;

  int size_hist = hist_buffer.size();
  int size_ref = ref_buffer.size();

  it_hist=hist_buffer.begin();
  it_ref=ref_buffer.begin();

  for(int i=1;i<(size_hist/2);i++) it_hist++;
  for(int i=1;i<(size_ref/2);i++) it_ref++;

  int mediana_hist = *it_hist;
  int mediana_ref = *it_ref;

  int count=0;
  int count_non_0=0;
  int sec_count=0;
  int ML_count=0;
  int sector=1;
  std::pair<int,int> empty_bin;
  std::vector< std::pair<int,int> > new_empty_bins;

  for(int y_index=1;y_index<=binY;y_index++){
    if(ML_count==2){
      ML_count=0;
      sec_count=0;
      sector++;
    };
    ML_count++;

    for(int x_index=1;x_index<=binX;x_index++){
      if(using_ref_for_flag) {
	if( hist->GetBinContent(x_index,y_index)< mediana_hist*thresh/100 && ref->GetBinContent(x_index,y_index) >= mediana_ref*thresh/100){ 
	  sec_count++;
	  count++;
	  if( hist->GetBinContent(x_index,y_index)>0) count_non_0++;
	}
      } else {
	if( hist->GetBinContent(x_index,y_index)< mediana_hist*thresh/100){ 
	  sec_count++;
	  count++;
	};
      }
    };

    if(ML_count==2 && sec_count>0){
       empty_bin.first=sector;
       empty_bin.second=sec_count;
       new_empty_bins.push_back(empty_bin);
    }
  };
  
  if(nML<=0){ 
    dqm_core::Result* result = new dqm_core::Result(); 
    result->tags_["Configuration_ERROR_Setted_N_tot_ML"] = nML;
    result->status_ = dqm_core::Result::Undefined;
    return result;
  };

  //Result
  dqm_core::Result* result = new dqm_core::Result(); 
  result->tags_["00-%_ML_ON"] = 100*((double)(size_hist-count_non_0)/nML);
  
  if(thresholdname == "Limits" ) {

    if(count>0) {
      std::string tag;
      std::string tag1="Sector";
      std::string tag2="_number_of_ML_off";
      result->tags_["01-NEW_ML_OFF"] = count;
      for(int j=0; j<(int)new_empty_bins.size(); j++){
        boost::format fmt("%02d");
        fmt % new_empty_bins[j].first;
	tag="01-"+tag1+fmt.str()+tag2;
	result->tags_[tag] = new_empty_bins[j].second;
      };
    } 
    if (nML-count-size_hist > 0) result->tags_["02-KNOWN_ML_OFF"] = nML-count-size_hist;
    
    // Flag assignment
    if (count > redTh) {
      result->status_ = dqm_core::Result::Red; 
      ERS_DEBUG(1,"Red");
    } else if ( count <= redTh && count > greenTh ) {
      result->status_ = dqm_core::Result::Yellow;
      ERS_DEBUG(1,"Yellow");
    } else if (count <= greenTh) {
      result->status_ = dqm_core::Result::Green;
      ERS_DEBUG(1,"Green");
    } else {
      result->status_ = dqm_core::Result::Undefined;
      ERS_DEBUG(1,"Undefined");
    }

  } else if (thresholdname == "Percentage") {
    // Flag assignment
    if (100*((double)size_hist/nML)<redTh) {
      result->status_ = dqm_core::Result::Red; 
      ERS_DEBUG(1,"Red");
    } else if ( 100*((double)size_hist/nML)>=redTh && 100*((double)size_hist/nML)<greenTh ) {
      result->status_ = dqm_core::Result::Yellow;
      ERS_DEBUG(1,"Yellow");
    } else if (100*((double)size_hist/nML)>=greenTh) {
      result->status_ = dqm_core::Result::Green;
      ERS_DEBUG(1,"Green");
    } else {
      result->status_ = dqm_core::Result::Undefined;
      ERS_DEBUG(1,"Undefined");
    }
  }
  return result;
}


void 
MDTOverview::printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: check if there are any bins with less then a thresh% of the entries of the median bin\n";
  message += "             and perform a comparison with reference.\n";
  message += "MDTOverview_Global: Mandatory Green/Red Threshold: Percentage: of ML OFF\n";
  message += "MDTOverview_Station: Mandatory Green/Red Threshold: Limits: number of ML OFF\n";
  message += "Optional Parameters: thresh  = min % of hits from the median chamber accepted\n";
  message += "			   MinStat = Minimum histogram statistics needed to perform Algorithm\n";
  message += "			   using_ref_for_flag = if enabled (1) it compares the OFF MLs with reference, if not enabled (0)\n";
  message += "						the flag is setted checking the absolute % of ML on in the detector\n";
  message += "\n";
  
  out << message;
}

} // namespace dqm_algorithms
