/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsFilledOutRange.cxx compares bins of histogram wrt to reference histogram and counts number of bins N Sigma away from ref; returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BinsFilledOutRange.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <math.h>
#include <ers/ers.h>

using namespace std;


#include <dqm_core/AlgorithmManager.h>
static dqm_algorithms::BinsFilledOutRange myInstance;


dqm_algorithms::BinsFilledOutRange::BinsFilledOutRange()
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( "BinsFilledOutRange", this );
}

dqm_algorithms::BinsFilledOutRange::~BinsFilledOutRange()
{
}

dqm_algorithms::BinsFilledOutRange * 
dqm_algorithms::BinsFilledOutRange::clone()
{
  return new BinsFilledOutRange();
}


dqm_core::Result *
dqm_algorithms::BinsFilledOutRange::execute(	const std::string & name, 
							const TObject & object, 
                                                const dqm_core::AlgorithmConfig & config )
{
  TH1 * histogram;
  
  if(object.IsA()->InheritsFrom( "TH1" )) {
    histogram = (TH1*)&object;
    if (histogram->GetDimension() > 1 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 1 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1");
  }
  
  
  const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 0); 
  const int maxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20); 
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);  
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  double gthreshold;
  double rthreshold;
  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getGreenThresholds() );
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  
  int count = 0; 
  std::vector<int> range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters());
  int nbins=histogram->GetNbinsX();
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
  
  for ( int i = 1; i < range[0]; ++i ) {
    double content = histogram -> GetBinContent(i);
    if ( content !=0 ) {
        ++count;
          resulthisto->SetBinContent(i,content);
      if (publish && count<maxpublish) {
        dqm_algorithms::tools::PublishBin(histogram,i,0,content,result);
      }
    }
  }
  for ( int j = range[1]; j <= nbins; ++j ) {
      double content= histogram -> GetBinContent(j);
      if ( content != 0 ) {
        ++count;
          resulthisto->SetBinContent(j,content);
      if (publish && count<maxpublish) {
        dqm_algorithms::tools::PublishBin(histogram,j,0,content,result);
      }
      }
  }
  
  ERS_DEBUG(1,"Number of bins with content != 0  is " << count );
  ERS_DEBUG(1,"Green threshold: "<< gthreshold << " bin(s);   Red threshold : " << rthreshold << " bin(s) ");
  
  
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
void
dqm_algorithms::BinsFilledOutRange::printDescription(std::ostream& out)
{
  out<<"Bins_Filled_OutsideRange: Checks for number of non-empty bins outside bin range\n"<<std::endl;
  
  out<<"Mandatory Parameter: xmin: minimum x range"<<std::endl;
  out<<"Mandatory Parameter: xmax: maximum x range"<<std::endl;
  out<<"Mandatory Green/Red Threshold: NBins: Number of non-empty bins to give Green/Red result\n"<<std::endl;
                                                                                                                                                             
  out<<"Optional Parameter: PublishBins: Save bins which are different from average in Result (set to 1)"<<std::endl; 
  out<<"Optional Parameter: MaxPublish: Max number of bins to save (default 20)"<<std::endl; 
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
}

