/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file ChiComp.cxx does a simple Chi Squared like comparison of 2 histograms, returns Chi2/DOF ; returns dqm_core::Result
 * \author : Pier-Olivier DeViveiros, inspired by all the other dqm algorithms!
 * Call the algorithm using library libdqm_algorithms.so and name AlgChiComp_Basic
 */

#include "dqm_algorithms/ChiComp.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"

#include "TH1.h"
#include "TF1.h"
#include "TProfile.h"
#include "TClass.h"

using namespace std;

// Only 1 variation at the moment - might do more later
static dqm_algorithms::ChiComp Basic( "Basic" );

dqm_algorithms::ChiComp::ChiComp( const std::string & name )
  : m_name ( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( "AlgChiComp_"+ name, this );
}

dqm_algorithms::ChiComp * 
dqm_algorithms::ChiComp::clone()
{
  return new ChiComp( m_name );
}


dqm_core::Result *
dqm_algorithms::ChiComp::execute(	const std::string & name, 
						const TObject & object, 
						const dqm_core::AlgorithmConfig & config )
{
  // Histograms read from file
  const TH1 * histogram;
  TH1 * refhist;

  // Thresholds
  double gthresho;
  double rthresho;
  
  // Checks whether this is a Profile plot
  bool profile=0;
  if(object.IsA()->InheritsFrom( "TProfile" ))
    profile=1;
  
  // Checks which type of histogram this is...
  if(object.IsA()->InheritsFrom( "TH1" )) {
    histogram = static_cast<const TH1*>( &object );
    if (histogram->GetDimension() > 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }

  try {
    refhist = static_cast<TH1 *>( config.getReference() );
  }
   catch ( dqm_core::Exception & ex ) {
   throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
  }
  
  if (histogram->GetDimension() != refhist->GetDimension() ) {
    throw dqm_core::BadRefHist( ERS_HERE, name, "Dimension" );
  }
  
  if ((histogram->GetNbinsX() != refhist->GetNbinsX()) || (histogram->GetNbinsY() != refhist->GetNbinsY())) {
    throw dqm_core::BadRefHist( ERS_HERE, name, "number of bins" );
  }

  // Get thresholds

  std::string thresh="ChiSqPerNdof";
  try {
    gthresho = dqm_algorithms::tools::GetFromMap( thresh, config.getGreenThresholds() );
    rthresho = dqm_algorithms::tools::GetFromMap( thresh, config.getRedThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  // Here we clone the Reference so that we can scale it...
  TH1 * reference = (TH1*)refhist->Clone();

  // Cast the histograms into TProfiles id needed
  TProfile * prof_hist(0);
  TProfile * prof_ref(0);
  if(profile){
    prof_hist = (TProfile*)histogram->Clone();
    prof_ref = (TProfile*)refhist->Clone();
  }
  
  // Force ROOT to use Sum of Squares to handle uncertainties correctly
  // Scale Reference - No need to do this for Profile plots...
  if(!profile)
    {
      reference->Sumw2();
      if(histogram->Integral() != reference->Integral())
	reference->Scale(histogram->Integral()/reference->Integral());
    }
  
  
  // Constant to check Validity of Test - Number of points contributing to Chi^2
  int numchi = 0;
  
  // Chi Square Like Significance
  double significance = 0;
      
  // Calculate Chi Square
  for(int i=1; i<histogram->GetNbinsX()+1; i++){
  
    // Calculate the contribution
    double diff = fabs(histogram->GetBinContent(i) - reference->GetBinContent(i));

    double uncert1 = histogram->GetBinError(i);
    double uncert2 = reference->GetBinError(i);
    double uncert = sqrt(uncert1*uncert1 + uncert2*uncert2);

    bool badbin = 0;
    
    // Check whether this is a 0 uncertainty bin in a profile plot 
    // If it is the case, we have to ignore it...
    if(profile)
      badbin = prof_hist->GetBinEntries(i)<2 || prof_ref->GetBinEntries(i)<2;
    
    // Prevent Negative weights from MCatNLO from messing things up
    // Also, zero bins become very problematic when comparng histogrames
    // with vaslty different statistics - Remove them...

    if(!profile)
      badbin = histogram->GetBinContent(i)<=0 || reference->GetBinContent(i)<=0;
    
    if(uncert !=0 && badbin ==0){
      numchi++;
      significance += (diff/uncert)*(diff/uncert);
    }
  }
  
  dqm_core::Result* result = new dqm_core::Result();
  
  std::string out ="ChiSqPerBin";
  std::string out1="ChiSqTotal";
  std::string out2="ChiSqUsedBins";

  if(numchi>0)
    {
      double value = significance/numchi;
      result->tags_[out] = value;
      result->tags_[out1] = significance;
      result->tags_[out2] = numchi;
      if ( value <= gthresho ) {
	result->status_ = dqm_core::Result::Green;
      } else if (  value < rthresho ) {
	result->status_ = dqm_core::Result::Yellow;
      } else {
	result->status_ = dqm_core::Result::Red;
      }
    } else {
      result->tags_[out] = 0;
      result->tags_[out1]= 0;
      result->tags_[out2]= 0;
      result->status_ = dqm_core::Result::Undefined;
    }

  
  // Print all the useful information for comparing plots
  result->tags_["Entries"] = histogram->GetEntries();
  result->tags_["Mean"] = histogram->GetMean();
  result->tags_["RMS"] = histogram->GetRMS();
  result->tags_["EntriesReference"] = refhist->GetEntries();
  result->tags_["MeanReference"] = refhist->GetMean();
  result->tags_["RMSReference"] = refhist->GetRMS();
  
  return result;
}
void
dqm_algorithms::ChiComp::printDescription(std::ostream& out)
{
  out<<"AlgChiComp_" << m_name << ": Gives back a Chi Squared like quantity per histogram bin in comparison to a reference histogram. Note that this assumes Gaussian statistics, and handles different numbers of events per histogram correctly.  It is also fully compatible with TProfile graphs"<<std::endl;
  out<<"Mandatory Green/Red Threshold ChiSqPerNdof: Chi Squared per bin for Green/Red results\n"<<std::endl;
}
