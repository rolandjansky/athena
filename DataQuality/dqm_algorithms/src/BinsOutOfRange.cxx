/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  BinsOutOfRange.cxx is to pick out the problematic bins beyond [RANGE_D,RANGE_U]. 
    Author: Feng TIAN (columbia university)
    Email:  Feng.Tian@cern.ch 
*/

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BinsOutOfRange.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/AlgorithmManager.h>

#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <math.h>

#include <iostream>
#include <string>

using namespace std;
bool mySortfunc(bin3 i,bin3 j){return ((i.m_value) > (j.m_value));}
static dqm_algorithms::BinsOutOfRange myInstance;

dqm_algorithms::BinsOutOfRange::BinsOutOfRange( )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("BinsOutOfRange", this);
}

dqm_algorithms::BinsOutOfRange::~BinsOutOfRange()
{
}

dqm_algorithms::BinsOutOfRange * 
dqm_algorithms::BinsOutOfRange::clone()
{
  
  return new BinsOutOfRange();
}


dqm_core::Result *
dqm_algorithms::BinsOutOfRange::execute(const std::string &  name, 
					   const TObject& object, 
					   const dqm_core::AlgorithmConfig& config )
{ 
  TH1* histogram;

  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)&object;
    if (histogram->GetDimension() > 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
  
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  const double ignoreval = dqm_algorithms::tools::GetFirstFromMap( "ignoreval", config.getParameters(), -99999);
  const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 1); 
  const int Nmaxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20); 
  const double RANGE_D = dqm_algorithms::tools::GetFirstFromMap( "RANGE_D", config.getParameters(), -50);
  const double RANGE_U = dqm_algorithms::tools::GetFirstFromMap( "RANGE_U", config.getParameters(), 50);
  if ( histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  double gthreshold;
  double rthreshold;
  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "NbadBins", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "NbadBins", config.getGreenThresholds() );
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  std::vector<int> range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  vector<bin3> badbinstrip;
  for ( int k = range[0]; k <= range[1]; ++k ) {
    for ( int l = range[2]; l <= range[3]; ++l ) {
      double binvalue = histogram->GetBinContent(k,l);
      if (binvalue== ignoreval) continue;
      double eta = histogram->GetXaxis()->GetBinCenter(k);
      double phi = histogram->GetYaxis()->GetBinCenter(l); 
      if(binvalue > RANGE_D && binvalue < RANGE_U ) continue;
      bin3 onebin = {eta,phi,k,l,binvalue};
      badbinstrip.push_back(onebin);
    }    
  }

 std::sort(badbinstrip.begin(),badbinstrip.end(),mySortfunc); 
 dqm_core::Result* result = new dqm_core::Result();
// publish problematic bins
  int count=0;
  for(unsigned int i=0;i<badbinstrip.size();i++){
    if(publish){
      char tmp[500];
      sprintf(tmp,"%i-(eta,phi)=(%0.3f,%0.3f)",count,badbinstrip[i].m_eta,badbinstrip[i].m_phi);
      std::string tag = tmp;
      result->tags_[tag] = badbinstrip[i].m_value;
    }  
    count++;
    if(count>Nmaxpublish) break;
  }

  result->tags_["NBadBins"] = count;

  if(count>rthreshold ) result->status_ = dqm_core::Result::Red;
  else if (count>gthreshold) result->status_ = dqm_core::Result::Yellow;
       else result->status_ = dqm_core::Result::Green;

  return result;
  
}


void
dqm_algorithms::BinsOutOfRange::printDescription(std::ostream& out)
{
  
  out<<"BinsOutOfRange: Print out the badbins which are out of range [range_d,range_u] "<<std::endl;
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: ignoreval: valued to be ignored for being processed"<<std::endl; 
  out<<"Optional Parameter: MaxPublish: Max number of bins to save (default 20)"<<std::endl; 
  
}

