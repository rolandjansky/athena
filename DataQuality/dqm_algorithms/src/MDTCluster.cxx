/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MDTCluster.cxx,v 1.0 2008/10/08 Valerio Consorti
// **********************************************************************

#include "dqm_algorithms/MDTCluster.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TClass.h>
#include <TH1.h>
#include <TAxis.h>
#include <utility>
#include <string>
#include <vector>


#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"

static dqm_algorithms::MDTCluster staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

MDTCluster::MDTCluster()
  : m_name("MDTcluster")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


MDTCluster::~MDTCluster()
{
}


dqm_core::Algorithm*
MDTCluster::clone()
{
  return new MDTCluster(*this);
}


dqm_core::Result*
MDTCluster::execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
{
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
  
  //Get Parameters and Thresholds
  double minstat; 
  double n_sigma;
  double greenTh;
  double redTh;
  try {
    minstat      = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), 500);
    n_sigma      = dqm_algorithms::tools::GetFirstFromMap("N_sigma", config.getParameters());
    redTh        = dqm_algorithms::tools::GetFromMap( "ClusterSize", config.getRedThresholds());
    greenTh      = dqm_algorithms::tools::GetFromMap( "ClusterSize", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  double cluster_size=greenTh;

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
  if (hist->GetNbinsX() != ref->GetNbinsX() ) {
    throw dqm_core::BadRefHist( ERS_HERE, name, "Reference VS histo: Different bin numbre in X axis!" );
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
  double N;
  double N_ref;

  N= hist->GetEntries();
  N_ref= ref->GetEntries();

  if(N == 0){
    ERS_INFO("Histogram has no entries" <<hist->GetName());
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = hist->GetEntries();
    return result;
  };    

  if(N_ref == 0){
    ERS_INFO("Reference histogram has no entries" <<hist->GetName());
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientRefEntries"] = ref->GetEntries();
    return result;
  };    

  //Scaling reference to histo

  double norm= (double) N/N_ref;
  ref->Scale(norm);


  //Storing bin-bin differences between histo and reference
  unsigned int i=1;
  int j=1;
  std::vector<double> diff;

  for(j=1;j<=hist->GetNbinsX();j++){    
    diff.push_back(hist->GetBinContent(j)-ref->GetBinContent(j));
  };



  //looking for clusters:
  //the custer begins when the difference between hist and ref is larger than 
  //n_sigma*sqrt(hist_N_bin_entries+ref_N_bin_entries*norm*norm)
  //and finishes when the difference is smaller. 

  i=0;
  bool cluster_open=0;
  std::pair<int,int> bin_start_end;
  std::pair<int , std::pair<int,int> > cluster_size_binStart_binEnd;
  std::vector< std::pair< int , std::pair< int, int> > > clusters;
  int size=0;

  for(i=0;i<diff.size();i++){

    if(fabs(diff[i])>n_sigma*sqrt(hist->GetBinContent(i+1)+ref->GetBinContent(i+1)*norm)){
      if(cluster_open){
        if(i!=0){
        
          if(diff[i-1]*diff[i]>0){
            size++;
          };
        
          if(diff[i-1]*diff[i]<=0 ){
            if(size>=cluster_size){
              bin_start_end.second=i;
              cluster_size_binStart_binEnd.first=size;
              cluster_size_binStart_binEnd.second=bin_start_end;
              clusters.push_back(cluster_size_binStart_binEnd);
             };
            cluster_open=0;
          };

          if(i==0){
            ERS_INFO("Cluster searching fail");
            dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
            result->tags_["i"] = i;
            return result;
          };
        };
      };
        
      if(!cluster_open){
        cluster_open=1;
        size=0;
        bin_start_end.first=i+1;
        size++;
      };

    };

    if(fabs(diff[i])<=n_sigma*sqrt(hist->GetBinContent(i+1)+ref->GetBinContent(i+1)*norm)){
      if(cluster_open){
        if(size>=cluster_size){
          bin_start_end.second=i;
          cluster_size_binStart_binEnd.first=size;
          cluster_size_binStart_binEnd.second=bin_start_end;
          clusters.push_back(cluster_size_binStart_binEnd);
        };
        cluster_open=0;
      };
    };
    if(i==diff.size()-1){
      if(cluster_open){
        if(size>=cluster_size){
          bin_start_end.second=i+1;
          cluster_size_binStart_binEnd.first=size;
          cluster_size_binStart_binEnd.second=bin_start_end;
          clusters.push_back(cluster_size_binStart_binEnd);
        };
        cluster_open=0;
      };
    };
  };
  

//Result

  dqm_core::Result* result = new dqm_core::Result(); 
  result->tags_["00-N_clusters"] = clusters.size();

  std::string Cluster="Cluster_";
  char num[3];
  std::string Size="_info_1_size";
  std::string start_at_bin="_info_2_start_at_bin";
  std::string finish_at_bin="_info_3_finish_at_bin";
  std::string message1;
  std::string message2;
  std::string message3;

  for(i=0;i<clusters.size();i++){
    if((i+1)<10){
      sprintf(num,"0%d",(i+1));
    };  

    if((i+1)>=10){
      sprintf(num,"%d",(i+1));
    };  
 
    message1 = Cluster + (std::string) num + Size;
    message2 = Cluster + (std::string) num + start_at_bin;
    message3 = Cluster + (std::string) num + finish_at_bin;
    
    result->tags_[message1] = clusters[i].first;
    result->tags_[message2] = clusters[i].second.first;
    result->tags_[message3] = clusters[i].second.second;
  }; 


  i=0;
  int flag=0;
  for(i=0;i<clusters.size();i++){
    if(clusters[i].first<redTh) flag++;
    if(clusters[i].first>=redTh) flag+=2;
  }; 


  if (flag<1) {
    result->status_ = dqm_core::Result::Green; 
    ERS_DEBUG(1,"Green");
  } else if ( flag>=1 && flag<2 ) {
    result->status_ = dqm_core::Result::Yellow;
    ERS_DEBUG(1,"Yellow");
  } else if (flag>=2) {
    result->status_ = dqm_core::Result::Red;
    ERS_DEBUG(1,"Red");
  }

  return result;
}


void 
MDTCluster::printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: look for clusters of adiacent bins where each bin differ from reference more then n sigma\n";
  message += "Mandatory Green/Red Threshold: 	ClusterSize: the green threshold is the minimum cluster size, each cluster\n";
  message += "					smaller than this size will not recorded as cluster by this algorithm\n";
  message += "					red threshold is the limit size of a cluster, if there is just one cluster\n";
  message += "					bigger then this limit the flag will be red;\n"; 
  message += "					otherwise the flag will be red if it find more at last 2 clusters\n";
  message += "			   N_sigma :	maximum distance in standard deviation of a bin of the histo from the reference one\n";
  message += "Optional Parameters: MinStat = Minimum histogram statistics needed to perform Algorithm\n";
  message += "\n";
  
  out << message;
}

} // namespace dqm_algorithms
