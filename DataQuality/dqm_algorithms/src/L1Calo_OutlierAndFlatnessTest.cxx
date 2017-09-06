/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file L1Calo_OutlierAndFlatnessTest.cxx checks and removes outliers from TH1-Histos and tests the remaining distribution for flatness. Returns dqm_core::Result
 * \author Steffen Schaepe, simplified for L1Calo purposes by Sarah Heim
 */

#include <dqm_algorithms/L1Calo_OutlierAndFlatnessTest.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>

#include <dqm_core/AlgorithmManager.h>
#include "dqm_core/exceptions.h"
#include <dqm_core/AlgorithmConfig.h>
#include <dqm_core/Result.h>

#include <TMath.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>

namespace
{
  dqm_algorithms::L1Calo_OutlierAndFlatnessTest L1Calo_OutlierAndFlatnessTest( "L1Calo_OutlierAndFlatnessTest" );
}


dqm_algorithms::L1Calo_OutlierAndFlatnessTest::L1Calo_OutlierAndFlatnessTest( const std::string & name ) : m_name( name ) {
  dqm_core::AlgorithmManager::instance().registerAlgorithm(name, this);
  m_counterSkip = 0;
}

dqm_algorithms::L1Calo_OutlierAndFlatnessTest * dqm_algorithms::L1Calo_OutlierAndFlatnessTest::clone(){
  return new L1Calo_OutlierAndFlatnessTest( m_name );
}

dqm_core::Result *dqm_algorithms::L1Calo_OutlierAndFlatnessTest::execute( const std::string& name, const TObject& object, 
    const dqm_core::AlgorithmConfig& config) { 

//   if(m_counterSkip < counterSkipMax){
//     dqm_core::Result* result = new dqm_core::Result();
//     result->status_ = dqm_core::Result::Green;
//     m_counterSkip++;
//     return result;
//   }

//   m_counterSkip = 0;

  TH1 const * histogram;
  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = static_cast<TH1 const *>(&object);
 
    if (histogram->GetDimension() > 2 ) { 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }

  const double minstat = tools::GetFirstFromMap( "MinStat", config.getParameters(), 1);
  
  //Check for minimum statistics  
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result* result = new dqm_core::Result();
    result->status_ = dqm_core::Result::Green;
    result->tags_["Not enough Signal Statistics"] = minstat;
    //delete histogram;
    return result;
  }

  //check if histogram contains only zeros
  int checkZeroContent = (int) tools::GetFirstFromMap( "CheckZeroContent", config.getParameters(), 1 );

  if ((checkZeroContent > 0) && (histogram->Integral() == 0) && (histogram->GetEntries() > 0)) {
    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" is filled with zeroes!");
    dqm_core::Result* result = new dqm_core::Result();
    result->status_ = dqm_core::Result::Red;
    result->tags_["Integral"] = histogram->Integral();
    //delete histogram;
    return result;
  }  
  
  
  int checkSigmaDev = 1;
  int dontCountSigmaOutliers = 0;
  int ignore0 = 1;
  double sigmaDev = 0; 
  double absDev = 0;
  
  //Get configuration parameters and set defaults  
  try {
    ignore0 = (int) tools::GetFirstFromMap( "Ignore0", config.getParameters(), 1 );
    
    checkSigmaDev = (int) tools::GetFirstFromMap( "CheckSigmaDev", config.getParameters(), 1 );

    absDev = tools::GetFirstFromMap( "AbsDev", config.getParameters(), 15.0 );

    if (checkSigmaDev) {
      sigmaDev = tools::GetFirstFromMap( "SigmaDev", config.getParameters());
      dontCountSigmaOutliers = (int) tools::GetFirstFromMap( "DontCountSigmaOutliers", config.getParameters(), 0);
    }  
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
    
  const std::vector<int> range = dqm_algorithms::tools::GetBinRange(histogram, config.getParameters());
  const int xminBin = range[0];
  const int xmaxBin = range[1];

  //compute the number of empty bins (or bins=0)
  int zeroBins = 0;
  for (int i = xminBin; i <= xmaxBin; i++) {
    if (histogram->GetBinContent(i) == 0) zeroBins++;
  }

  //Compute outlier bins by using a simplified Grubb's test without cutting on a t-distribution but by giving an external threshold defining how many standard deviations a value has to differ from the mean to be classified as an outlier or by giving an absolute threshold for classifying values as outliers
  int totalBadBins = 0;
  int totalUncountedBadBins = 0;
  double mean=0;
  double stdev=0;    
  
  //compute the arithmetic mean
  double sum=0,sum2=0;
  int counter = 0;
  
  for(int i=xminBin;i <= xmaxBin;i++){
    if( histogram->GetBinContent(i) != 0 || ignore0 == false) {
      sum += histogram->GetBinContent(i);
	sum2 += histogram->GetBinContent(i)*histogram->GetBinContent(i);
	counter++;
    }
  }
  
  mean = counter > 0 ? sum/(double)counter : 0;
  stdev = counter > 0 ? sqrt( (1/(double)counter)*sum2 - mean*mean) : 0;   
    
    
    //check how many bins are out of N sigma range (SigmaDev) or out of the absolute dev (AbsDev) or over absolute limit (AbsLimit), count them as (uncounted) outliers 
    double dev;
    double absdev_test = 0;
    if (checkSigmaDev) {
      for (int i = xminBin; i <= xmaxBin; i++) {
        if ((histogram->GetBinContent(i) != 0) || (ignore0 == 0)) {
	  dev=fabs(histogram->GetBinContent(i) - mean)/stdev;
	  absdev_test=fabs(histogram->GetBinContent(i) - mean);
	  if ((dev > sigmaDev) && (absdev_test > absDev)) {
	    if (dontCountSigmaOutliers){
	      totalUncountedBadBins++;
	    } else {
	      totalBadBins++;
	    }
	  }
	}
      }
    } 

    int totalBadBins_all = 0;
    int totalUncountedBadBins_all = 0;
    double mean_all=0;
    double stdev_all=0;    
    
    //compute the arithmetic mean
    double sum_all=0,sum2_all=0;
    int counter_all = 0;
  
    for(int i=0;i <= histogram->GetNbinsX() ;i++){
    if( histogram->GetBinContent(i) != 0 || ignore0 == false) {
      sum_all += histogram->GetBinContent(i);
	sum2_all += histogram->GetBinContent(i)*histogram->GetBinContent(i);
	counter_all++;
    }
  }
  
  mean_all = counter_all > 0 ? sum_all/(double)counter_all : 0;
  stdev_all = counter_all > 0 ? sqrt( (1/(double)counter_all)*sum2_all - mean_all*mean_all) : 0;   
    
    
    //check how many bins are out of N sigma range (SigmaDev) or out of the absolute dev (AbsDev) or over absolute limit (AbsLimit), count them as (uncounted) outliers 
    double dev_all;
    double absdev_test_all = 0;
    if (checkSigmaDev) {
      for (int i=0;i <= histogram->GetNbinsX() ;i++) {
        if ((histogram->GetBinContent(i) != 0) || (ignore0 == 0)) {
	  dev_all=fabs(histogram->GetBinContent(i) - mean_all)/stdev_all;
	  absdev_test_all=fabs(histogram->GetBinContent(i) - mean_all);
	  if ((dev_all > sigmaDev) && (absdev_test_all > absDev)) {
	    if (dontCountSigmaOutliers){
	      totalUncountedBadBins_all++;
	    } else {
	      totalBadBins_all++;
	    }
	  }
	}
      }
    } 
  

  //Prepare map for results and commit outlier specific parts
  std::map<std::string,double> algparams;
  algparams["Number_of_outlier_bins"] = totalBadBins;
  algparams["Number_of_outlier_bins_all"] = totalBadBins_all;
  algparams["Mean"] = mean;
  algparams["Standard_deviation"] = stdev;
  algparams["Number_of_bins_equal_zero"] = zeroBins; 

  
  //Compare with given thresholds and compute DQ-Flag
  dqm_core::Result *result = tools::MakeComparisons(algparams, config.getGreenThresholds(), config.getRedThresholds());
  
  
  return result;
}

void dqm_algorithms::L1Calo_OutlierAndFlatnessTest::printDescription(std::ostream& out) { 
  out << m_name << ": derivation of OutlierAndFlatnessTest, slimmed and optimized for L1Calo needs"
    "Checks TH1-inherited histograms for bins which lie either Nsigma or AbsDev away from the mean (by default excludes bins with zero entries). Can also check mean itself\n"
    "Config Parameters:\n"
    "\tCheckSigmaDev:\tCheck for deviation in units of standard deviations from mean (default 1).\n"
    "\tMinStat:\tMinimum Statistics for the histogram to be checked (default 1).\n"
    "\txmin and/or xmax:\tRestrict all counting to this x-axis interval (default: full axis range).\n"
    "\txmin and/or xmax:\tThere is a second output parameter (Number_of_outlier_bins_all), which always uses the full range.\n"
    "\tIgnore0:\tBins with 0 content are ignored for outlier/mean computation (default 1).\n"
    "\tSigmaDev:\tNumber of Standard Deviations a single bin has to differ from the mean of the distribution to be classified as outlier. Has to be given if \"CheckSigmaDev\" is set to 1. (default 5)\n"
    "\tAbsDev:\tAbsolute value a single bin has to differ from the mean of the distribution to be classified as outlier.  Has to be given if \"CheckAbsDev\" is set to 1.\n"
    "\tDontCountSigmaDev:\tBins deviating by a given number of standard deviations are not counted as outliers (default 0)."
    "\tDontCountSigmaDev:\t(Not recommended to be switched on in current config)\n"
    "Threshold Parameters:\n"
    "\tStandard_deviation: \tStandard Deviation of distribution.\n"
    "\tNumber_of_outlier_bins:\tNumber of bins classified as outliers using the given thresholds (in chosen histogram range).\n"
    "\tNumber_of_outlier_bins_all:\tNumber of bins classified as outliers using the given thresholds (in full histogram range).\n"
    "\tMean:\tMean of distribution.\n"
    "\tNumber_of_bins_equal_zero:\tNumber of Bins with zero content." << std::endl;
}
