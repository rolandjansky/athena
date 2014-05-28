/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Bins_Diff_FromAvg.cxx calculates average bin value; finds bins more than N Sigma away from average and returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/Bins_Diff_FromAvg.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/AlgorithmManager.h>

#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <math.h>

#include <iostream>
#include <string>

using namespace std;

static dqm_algorithms::Bins_Diff_FromAvg myInstance;

dqm_algorithms::Bins_Diff_FromAvg::Bins_Diff_FromAvg( )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("Bins_Diff_FromAvg", this);
}

dqm_algorithms::Bins_Diff_FromAvg::~Bins_Diff_FromAvg()
{
}

dqm_algorithms::Bins_Diff_FromAvg * 
dqm_algorithms::Bins_Diff_FromAvg::clone()
{
  
  return new Bins_Diff_FromAvg();
}


dqm_core::Result *
dqm_algorithms::Bins_Diff_FromAvg::execute(const std::string &  name, 
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
  bool greaterthan = (bool) dqm_algorithms::tools::GetFirstFromMap( "GreaterThan", config.getParameters(), 0);
  bool lessthan = (bool) dqm_algorithms::tools::GetFirstFromMap( "LessThan", config.getParameters(), 0);
  const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 0); 
  const int maxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20); 
  const double maxdiffabs = dqm_algorithms::tools::GetFirstFromMap( "MaxDiffAbs", config.getParameters(), -1); 
  const double maxdiffrel = dqm_algorithms::tools::GetFirstFromMap( "MaxDiffRel", config.getParameters(), -1);
  
  if (greaterthan && lessthan) {
    ERS_INFO("Both GreaterThan and LessThan parameters set: Will check for for both");
    greaterthan = false;
    lessthan = false;
  }
  
  if ( histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  double bin_threshold;
  double gthreshold;
  double rthreshold;
  try {
    bin_threshold = dqm_algorithms::tools::GetFirstFromMap( "NSigma", config.getParameters() );
    rthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getGreenThresholds() );
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
    
  double sumwe=0;
  double sume=0;
  TH1* resulthisto;
  if (histogram->InheritsFrom("TH2")) {
	resulthisto=(TH1*)(histogram->Clone());
  } else if (histogram->InheritsFrom("TH1")) {
        resulthisto=(TH1*)(histogram->Clone());
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }

  resulthisto->Reset();

  int count = 0; 
  std::vector<int> range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  
  for ( int i = range[0]; i <= range[1]; ++i ) {
    for ( int j = range[2]; j <= range[3]; ++j ) {
      if (histogram->GetBinContent(i,j) == ignoreval) continue;
      if (histogram->GetBinError(i,j) == 0 ) continue;
      sumwe += histogram->GetBinContent(i,j)*(1/pow(histogram->GetBinError(i,j),2));
      sume += 1./pow(histogram->GetBinError(i,j),2);
    }
  }
  double avg;

  if (sume !=0 ) {
    avg=sumwe/sume;
  } else {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["SumErrors"] = sume;
    delete resulthisto;
    return result;
  }
 
  dqm_core::Result* result = new dqm_core::Result();
  result->tags_["Average"] = avg;

  for ( int k = range[0]; k <= range[1]; ++k ) {
    for ( int l = range[2]; l <= range[3]; ++l ) {
      double inputcont = histogram->GetBinContent(k,l);
      double inputerr = histogram->GetBinError(k,l);
      double diff=inputcont - avg;
      double reldiff=1;
      if(avg!=0) reldiff=diff/avg;
      else if(diff==0) reldiff=0;
      if (inputcont == ignoreval) continue;
      if (inputerr != 0){
	double sigma=diff/inputerr;	
	if (greaterthan && diff < 0. ) continue;  
	if (lessthan && diff > 0. ) continue;  

	if ( (fabs(sigma) > bin_threshold) && (fabs(diff) > maxdiffabs) && (fabs(reldiff) > maxdiffrel) ) {
          resulthisto->SetBinContent(k,l,inputcont);
	  count++;
	  if (publish && count < maxpublish){
            dqm_algorithms::tools::PublishBin(histogram,k,l,inputcont,result);
	  }
	}
      }
      
    }    
  }
  
  result->tags_["NBins"] = count;
  result->object_ =  (std::auto_ptr<TObject>)(TObject*)(resulthisto);

  ERS_DEBUG(1,"Number of bins " << bin_threshold << " Sigma away from average of "<< avg << " is " << count);
  ERS_DEBUG(1,"Green threshold: "<< gthreshold << " bin(s);   Red threshold : " << rthreshold << " bin(s) ");
  
  
  
  if ( count <= gthreshold ) {
    result->status_ = dqm_core::Result::Green;
  } else if ( count < rthreshold ) {
    result->status_ = dqm_core::Result::Yellow;
  } else {
    result->status_ = dqm_core::Result::Red;
  }
  return result;
  
}
void
dqm_algorithms::Bins_Diff_FromAvg::printDescription(std::ostream& out)
{
  
  out<<"Bins_Diff_FromAvg: Calculates average bin value and checks number of bins N sigma away from calculated average\n"<<std::endl;
  
  out<<"Mandatory Parameter: NSigma: Number of sigma each bin must be within average bin value\n"<<std::endl;
  
  
  out<<"Mandatory Green/Red Threshold: NBins: number of bins N sigma away from average to give Green/Red result\n"<<std::endl;
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: xmin: minimum x range"<<std::endl;
  out<<"Optional Parameter: xmax: maximum x range"<<std::endl;
  out<<"Optional Parameter: ymin: minimum y range"<<std::endl;
  out<<"Optional Parameter: ymax: maximum y range"<<std::endl;
  out<<"Optional Parameter: ignoreval: valued to be ignored for calculating average"<<std::endl; 
  out<<"Optional Parameter: GreaterThan: check only for bins which are GreaterThan average (set to 1)"<<std::endl; 
  out<<"Optional Parameter: LessThan: check only for bins which are LessThan average (set to 1)"<<std::endl; 
  out<<"Optional Parameter: PublishBins: Save bins which are different from average in Result (set to 1)"<<std::endl; 
  out<<"Optional Parameter: MaxPublish: Max number of bins to save (default 20)"<<std::endl; 
  out<<"Optional Parameter: MaxDiffAbs: test fails if NBins more than NSigma away and NBins more than MaxDiffAbs (absolut difference) away from average"<<std::endl; 
  out<<"Optional Parameter: MaxDiffRel: test fails if NBins more than NSigma away and NBins more than MaxDiffRel (relative difference) away from average\n"<<std::endl; 
  
}

