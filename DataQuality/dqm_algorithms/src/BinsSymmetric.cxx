/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsSymmetric.cxx check if +ve and -ve bins around the central value are consistent
 * \author M Petteni
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BinsSymmetric.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <cstdio>
#include <TClass.h>
#include <ers/ers.h>
#include <iostream>
#include <string>

#include <dqm_core/AlgorithmManager.h>

static dqm_algorithms::BinsSymmetric myInstance;

dqm_algorithms::BinsSymmetric::BinsSymmetric ()
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("BinsSymmetric",this);
}

dqm_algorithms::BinsSymmetric::~BinsSymmetric ()
{
}
dqm_algorithms::BinsSymmetric *
dqm_algorithms::BinsSymmetric::clone()
{
  return new BinsSymmetric();
}

dqm_core::Result *
dqm_algorithms::BinsSymmetric::execute( 	const std::string & name , 
						const TObject & object, 
                                                const dqm_core::AlgorithmConfig & config)
{  
  
  TH1 * histogram;
  
  if (object.IsA()->InheritsFrom("TH1")){
  
    histogram = (TH1*)&object;

    if (histogram->GetDimension() > 3 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 3 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }    
  
  if (histogram->GetEntries() == 0) {
    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" is Empty");
    return new dqm_core::Result(dqm_core::Result::Red);
  }

  const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 0); 
  const int maxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20);
  const double mindiffabs = (double) dqm_algorithms::tools::GetFirstFromMap( "MaxDiffAbs", config.getParameters(), 0);
  const double bin_threshold = (double) dqm_algorithms::tools::GetFirstFromMap( "NSigmaBin", config.getParameters(), 3);
  const bool ignorezero = (bool) dqm_algorithms::tools::GetFirstFromMap("IgnoreZero",config.getParameters(), 0);
  
  double gthreshold;
  double rthreshold;
  
  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "NSigma", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "NSigma", config.getGreenThresholds() );
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
 
  int count = 0; 
  
  std::vector<int> range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  
  dqm_core::Result* result = new dqm_core::Result();

  // vectors for global chisq calculation
  std::vector<double> binvals,binerrs;
  std::vector<double> refvals,referrs;
  double small_num = 1.e-05;

  // logic to see if there is a bin at the central value. If so compare central to average of two bins on either side 
  bool even_nbins = ((range[1] - range[0] + 1) % 2 == 0) ? true : false;
  int range_comp = (range[1] - range[0] + 1) / 2;

  int start_bin_low = range[1] - range_comp;
  int start_bin_high = range[0] + range_comp;
  
  // check central bin if there is one 
  if (!even_nbins){

    double xbin0 = histogram->GetBinCenter(start_bin_low);

    double bin0 = histogram->GetBinContent(start_bin_low);
    double bin1 = histogram->GetBinContent(start_bin_low + 1);
    double bin2 = histogram->GetBinContent(start_bin_low - 1);
    
    double binerr0 = histogram->GetBinError(start_bin_low);
    double binerr1 = histogram->GetBinError(start_bin_low + 1);
    double binerr2 = histogram->GetBinError(start_bin_low - 1);
    
    double mean_bins = (bin1+bin2)/2.;
    double errmean_bins = sqrt(pow(binerr1,2.)+pow(binerr2,2.)/2.);
  
    double diff = fabs(bin0 - mean_bins);
    double differr = sqrt(pow(binerr0,2.)+pow(errmean_bins,2));
    double sigma = 1.;

    if ((!ignorezero) || (bin0 != 0 && mean_bins != 0)){

	    binvals.push_back(bin0);
	    binerrs.push_back(binerr0);
	    // use reference bin for chisq calculation as the combined bin from neighbours
	    refvals.push_back(mean_bins);
	    referrs.push_back(errmean_bins);

	    if (differr/diff > small_num) sigma = diff/differr;

	    // sigma threshold used to highlight potentially problematic bins
	    if (sigma > bin_threshold  && (fabs(diff) > mindiffabs)) {
      
		    ++count;

		    if (publish && count <= maxpublish) {

			    std::ostringstream os;
			    os << "Sigma(" << xbin0 << ")(" << bin0 << "," << mean_bins << ")";
			    std::string badbins = os.str();
			    result->tags_[badbins.c_str()] = sigma;
			    ERS_DEBUG(1,"x bin" << start_bin_low << " value " << bin0 << " sigma difference " << sigma);

		    }
	    }
    }
  }
  
  // now loop over all the other bins
  for (int i = 0; i < range_comp; ++i){

    double binhigh = histogram->GetBinContent(start_bin_high+i);
    double binlow = histogram->GetBinContent(start_bin_low-i);   

    double xbinhigh = histogram->GetBinCenter(start_bin_high+i);

    double binerrhigh = histogram->GetBinError(start_bin_high+i);
    double binerrlow = histogram->GetBinError(start_bin_low-i);   

    double diff = fabs(binlow - binhigh);
    double differr = sqrt(pow(binerrlow,2.)+pow(binerrhigh,2));

    if ((!ignorezero) || (binlow != 0 && binhigh != 0)){

    binvals.push_back(binhigh);
    binerrs.push_back(binerrlow);
    refvals.push_back(binlow);
    referrs.push_back(binerrlow);

    double sigma = 1.;
    if (differr > small_num) sigma = diff/differr;

    if (sigma > bin_threshold  && (fabs(diff) > mindiffabs)) {

      ++count;
      if (publish && count <= maxpublish){

	std::ostringstream os;
	os << "Sigma(" << xbinhigh << ")" << "(" << binlow << "," << binhigh << ")";
	std::string badbins = os.str();
	result->tags_[badbins.c_str()] = sigma;
	ERS_DEBUG(1,"x bin " << start_bin_high+i << " value " << binhigh);
	ERS_DEBUG(1,"x bin " << start_bin_low-i << " value " << binlow << " sigma difference " << sigma);
      }
      }
    }
  }
  
  std::pair<double,double> chisq_prob = dqm_algorithms::tools::CalcBinsProbChisq(binvals,binerrs,refvals,referrs);

  ERS_DEBUG(1, "Number of bins " << bin_threshold << " Sigma away from reference bin is " << count);
  ERS_DEBUG(1,  "Green threshold: "<< gthreshold << " bin(s);   Red threshold : " << rthreshold << " bin(s) ");
  result->tags_["NBins"] = count;
  result->tags_["SigmaChisq"] = chisq_prob.second;

  // cut on sigma, this can be eventually modified to cut on prob as well

  double sigma_check = fabs(chisq_prob.second);
  if ( sigma_check <= gthreshold ) {
    result->status_ = dqm_core::Result::Green;
  } else if ( sigma_check < rthreshold ) {
    result->status_ = dqm_core::Result::Yellow;
  } else {
    result->status_ = dqm_core::Result::Red;
  }
  return result;

}

void dqm_algorithms::BinsSymmetric::printDescription(std::ostream& out){
  
  out<<"BinsSymmetric: Checks if histogram is symmetric around mid-point of given range. works only for 1-D histograms \n"<<std::endl;
out<<"Mandatory Green/Red Threshold: NSigma: N sigma for chisq global fit probability to be away from 0 from unit normal distribution. Thresholds are for Green/Red results\n"<<std::endl;
  out<<"Optional Parameter: NSigmaBin: Number of sigma bins must be over opposite bin to be flagged and reported (default 3) \n"<<std::endl;
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: xmin: minimum x range"<<std::endl;
  out<<"Optional Parameter: xmax: maximum x range"<<std::endl;
  out<<"Optional Parameter: PublishBins: Save bins which are different from average in Result (set to 1)"<<std::endl; 
  out<<"Optional Parameter: MaxDiffAbs: test fails if NBins more than NSigma away and NBins more than MaxDiffAbs (absolut difference) away"<<std::endl; 
  out<<"Optional Parameter: MaxDiffRel: test fails if NBins more than NSigma away and NBins more than MaxDiffRel (relative difference) away\n"<<std::endl; 

}
