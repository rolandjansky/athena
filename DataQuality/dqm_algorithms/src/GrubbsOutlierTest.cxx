/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file GrubbsOutlierTest.cxx
 *               Determine outliers at a given significance level by defining the Grubbs test
 *               statistic, and finding out the probability of each bin being an outlier. The
 *               critical value is computed for each bin and the probability is estimated 
 *               using the two tailed students T distribution. If the probability for a given bin 
 *               falls below the significance level the bin is an outlier. The flag can be computed
 *               using fraction of outlier bins.
 *               
 *  \author Venkatesh Kaushik <venkat.kaushik@cern.ch>
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_core/AlgorithmManager.h>

#include <dqm_algorithms/GrubbsOutlierTest.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <ers/ers.h>

// STL
#include <sstream>

// ROOT
#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TClass.h>

#ifndef GTESTPARS
#define GTESTPARS 5
#endif

dqm_algorithms::GrubbsOutlierTest GrubbsTest( "Grubbs" );

dqm_algorithms::GrubbsOutlierTest::GrubbsOutlierTest( const std::string & name )
  : m_name ( name ) {
  dqm_core::AlgorithmManager::instance().registerAlgorithm("OutlierTest_"+ name, this );
}

dqm_algorithms::GrubbsOutlierTest * 
dqm_algorithms::GrubbsOutlierTest::clone()
{
  return new GrubbsOutlierTest( m_name );
}


dqm_core::Result *
dqm_algorithms::GrubbsOutlierTest::execute(	const std::string & name , 
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config )
{
  const TH1 * histogram;

  // check if it inherits from 2d histogram
  if ( object.IsA()->InheritsFrom("TH2") ) {
    throw dqm_core::BadConfig( ERS_HERE , name , " dimension > 1 ");
  }

  // check if its a 1d histogram or not
  if ( object.IsA()->InheritsFrom("TH1") ) {
    histogram = (TH1*)&object;
  } else {
    throw dqm_core::BadConfig( ERS_HERE ,name , " does not inherit from TH1");
  }

   // Configure the DQ algorithm
  double grThr, reThr; // Green and Red thresholds

  // param 1: skip bin mod(N) 
  const int skipmod = (int)dqm_algorithms::tools::GetFirstFromMap("SkipBinModulo", config.getParameters(), histogram->GetNbinsX() + 10);
  if (skipmod < 1) {
    std::ostringstream msg;
    msg << " Parameter \"SkipBinModulo\" is out of bounds. Value given = " << skipmod; 
    msg << "\n Range of values acceptable: integer >= 1";
    msg << "\n Default Value: skip none. Do not set \"SkipBinModulo\" in your config to use default";
    throw dqm_core::BadConfig(ERS_HERE,name,msg.str());
  }
  
  // param 2: significance level
  const double signL = dqm_algorithms::tools::GetFirstFromMap("Significance", config.getParameters(), 0.04);
  if (signL < 0.01 || signL > 0.95) {
    std::ostringstream msg;
    msg << " Parameter \"Significance\" is out of bounds. Value given = " << signL; 
    msg << "\n Range of values acceptable: floating point number [0.01, 0.95]";
    msg << "\n Default Value: 0.04. Do not set \"Significance\" in your config to use default";
    throw dqm_core::BadConfig(ERS_HERE,name,msg.str());
  }
  
  // param 3: min stat per bin
  const double minStat = dqm_algorithms::tools::GetFirstFromMap("MinStatPerBin", config.getParameters(), 10);
  if (minStat < 0) {
    std::ostringstream msg;
    msg << " Parameter \"MinStatPerBin\" is out of bounds. Value given = " << signL; 
    msg << "\n Range of values acceptable: integer >= 0";
    msg << "\n Default Value: 10. Do not set \"MinStatPerBin\" in your config to use default";
    throw dqm_core::BadConfig(ERS_HERE,name,msg.str());
  }

  // param 4,5: min and max bins
  // the following is a duplicate of dqm_algorithms::tools::GetBinRange, but with different parameter names
  const double notFound = -99999;
  const double xmin = dqm_algorithms::tools::GetFirstFromMap("Min", config.getParameters(), notFound);
  const double xmax = dqm_algorithms::tools::GetFirstFromMap("Max", config.getParameters(), notFound);
  const int minbin = (xmin == notFound) ? 1                      : histogram->GetXaxis()->FindBin(xmin);
  const int maxbin = (xmax == notFound) ? histogram->GetNbinsX() : histogram->GetXaxis()->FindBin(xmax);
  ERS_DEBUG(1, "xmin = " << xmin << ", xmax = " << xmax << " , minbin = " << minbin << ", maxbin = " << maxbin);

  // param 6: include bins with entries < minStat as outliers?
  const bool useMinStat = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("IsMinStatOutlier", config.getParameters(), 1));
  ERS_DEBUG(1, "useMinStat = " << useMinStat);

  // Get Green and Red Thresholds
  try {

    grThr = dqm_algorithms::tools::GetFromMap("Threshold",config.getGreenThresholds() );
    reThr = dqm_algorithms::tools::GetFromMap("Threshold",config.getRedThresholds() );

  } catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig(ERS_HERE,name,"Paramter: 'Threshold' is mandatory, cannot continue");
  }

  if ( grThr > 1.0 || reThr > 1.0) {
    throw dqm_core::BadConfig(ERS_HERE,m_name,"Configuration Error: Threshold>100%");
  }
  
    //loop over bins and store values
    int binc = 0;
    std::vector<float> binCountVec;
    std::vector<size_t> binIndices;
    
    // find mean, sigma max and min binvals
    for(int k = minbin; k < maxbin; k++) {
      // skip every N'th bin
      if (k % skipmod == 0) {}
      else {
        float binval = histogram->GetBinContent(k);
        binCountVec.push_back(binval);
        if((binval <= minStat) && useMinStat) {
          binIndices.push_back(binc);
        }
        binc++;
      } // else
    } // for k

    // find number of (and fraction of) outliers    
    int minOutliers = binIndices.size();
    int maxOutliers, nOutliers = minOutliers;
    int nbinsDenominator = binCountVec.size();

    dqm_core::Result * result = new dqm_core::Result;
    
    std::ostringstream msg;
    try {
      // invoke Grubbs' test
      maxOutliers = GrubbsTest(binCountVec,binIndices,signL);

      // debug
      msg<< "maxOutliers = " << maxOutliers << "\t bincountVecsize = " << nbinsDenominator << "\t";
      if(binIndices.size() <= binCountVec.size())
        for(size_t g= 0; g < binIndices.size(); g++) 
          msg << binIndices[g] << ":" << binCountVec[binIndices[g]] <<"\n";
      ERS_DEBUG(1, msg.str());     

    } catch ( dqm_core::Exception & ex ) {
      msg << " Exception caught in GrubbsTest \n";
      throw dqm_core::BadConfig(ERS_HERE,name,msg.str());
    }
   
    // no. of outliers
    if(maxOutliers >= 0) {
      nOutliers += (useMinStat ? maxOutliers : 0);
    }
    result->tags_.insert(std::make_pair("NumOfOutliers",nOutliers));
    
    // fraction of outliers
    float totalBins = nbinsDenominator > 0 ? nbinsDenominator :  histogram->GetNbinsX();
    float outlierFraction = 0.;
#ifndef __APPLE__
    bool IsInf = isinf(float(nOutliers) / totalBins);
    bool IsNaN = isnan(float(nOutliers) / totalBins);
#else
    bool IsInf = std::isinf(float(nOutliers) / totalBins);
    bool IsNaN = std::isnan(float(nOutliers) / totalBins);
#endif
    if(!IsInf || !IsNaN) 
      outlierFraction = float(nOutliers) / totalBins;
    
    result->tags_.insert(std::make_pair("OutlierFraction",outlierFraction));
    msg.str("");
    msg << "Outlier fraction = "<< nOutliers << "/" << totalBins << " = " << outlierFraction;
    ERS_DEBUG(1, msg.str());

    
    // compare red/yellow/green thresholds with outlierfraction
    // and return results
    if (reThr> grThr) {
      if ( outlierFraction>reThr )
      {
        ERS_DEBUG(1,"[RED] Result : "<<outlierFraction);
        result->status_=dqm_core::Result::Red;
        return result;
      }
      else if ( outlierFraction > grThr )
      {
        ERS_DEBUG(1,"[YELLOW] Result : "<<outlierFraction);
        result->status_=dqm_core::Result::Yellow;
        return result;
      }
    }else {
      if ( outlierFraction < reThr )
      {
        ERS_DEBUG(1,"[RED] Result : "<<outlierFraction);
        result->status_=dqm_core::Result::Red;
        return result;
      }
      else if ( outlierFraction < grThr )
      {
        ERS_DEBUG(1,"[YELLOW] Result : "<<outlierFraction);
        result->status_=dqm_core::Result::Yellow;
        return result;
      }
    }

    // return (default) result
    ERS_DEBUG(1,"[GREEN] Result");
    result->status_=dqm_core::Result::Green;
    return result;

}

// Algorithm implementation
int dqm_algorithms::GrubbsOutlierTest::GrubbsTest(std::vector<float>& vec, std::vector<size_t>&binIndices, float signLevel) {
  // return codes: 
  // >= 0 : number of outliers found
  // -1 : input vector empty
  // -2 : less then two bins have occupancy (need >= 2 bins filled for this test)
  // -3 : significance level > 100% (nonsensical)
  // -4 : significance level < 0.1% (is never achievable in most cases)

  // negative return codes
  if(vec.empty()) return -1;

  size_t nItems = vec.size();
  if(vec.size() < 2) return -2;

  float epsilon = 1.e-5;
  if(signLevel > 1.0) return -3;

  if((signLevel-0.001) < epsilon) return -4;

  bool notEmpty = !binIndices.empty();

  // find mean
  float mean = 0.;
  for(size_t j = 0; j < nItems; j++) mean += vec[j];
  mean /= nItems;

  // find standard deviation (sd)
  float sigma = 0.;
  for(size_t j = 0; j < nItems; j++) {
    float diff = (vec[j]-mean);
    sigma += diff*diff;
  }
  sigma /= (nItems-1);
  sigma = sqrt(sigma);

  float zval = 0., tval = 0., prob = 0.;
  int retval = 0;
  // loop over bins and...
  for(size_t j = 0; j < nItems; j++) {
    
    // define test statistic z = |mean - val|/sigma
    zval = fabs(mean - vec[j])/sigma;
    
    // find out the critical value (tval) for the test statistic
    tval = sqrt(nItems*(nItems-2)*(zval*zval)/((nItems-1)*(nItems-1)-nItems*(zval*zval)));

    // determine the probability (two sided student's t pdf with N-2 d.o.f) of tval being 
    // far away (determined by significance level) from others so as to be considered an outlier
    prob = 2*(1. - TMath::StudentI(tval,nItems-2));

    // if p-value falls below significance level, this bin is an outlier
    if(prob < signLevel) {
      if(notEmpty) {
        bool storeMe = true;
        for(size_t k = 0; k < binIndices.size(); k++) {
          if(binIndices[k] == j) { storeMe = false; break; }
        } // for
        if(storeMe) {
          binIndices.push_back(j);
          retval++;
        }
      } else {
        binIndices.push_back(j);
        retval ++;
      }
    } // if prob
  } // for j

  // return #of outliers found
  return retval;

}

  void
dqm_algorithms::GrubbsOutlierTest::printDescription(std::ostream& out) {
  out << "-------------- Grubbs' Test for Outliers ----------------------------------------------------" << std::endl;  
  out << "Identify outlier bins in a 1d histogram. Given N = {n1, n2, n3...} where n1 = frequency in 1st bin etc..\n";
  out << "Define test statistic: Z_i = |n_i - mean| / sigma, where mean = sum(n_i)/sizeof(N) and sigma = standard deviation of N\n"; 
  out << "For each Z_i calculate the critical value (tval) of the test-statistic and find out its corresponding p-value. if p-value\n";
  out << "p-value is calculated using \"two-sided t-distribution\" pdf with sizeof(N)-2 degrees of freedom.\n";
  out << "If p-value <= significance, reject null hypothesis (H_0 = no outliers).\n";
  out << "----------------------------------------------------------------------------------------------" << std::endl;  
  out << "Parameters      : IsMinStatOutlier, Min, Max, MinStatPerBin, Significance, SkipBinModulo\n";
  out << "----------------------------------------------------------------------------------------------" << std::endl;  
  out << "IsMinStatOutlier: If bin contents are less than MinStatPerBin, is it an outlier? [type = bool] [values = 0, 1] [ default: 1]\n";
  out << "Min, Max        : Process bin numbers corresponding to Min, Max (on x-axis). [type = float] [default: all bins]\n";
  out << "MinStatPerBin   : Min number of entries per bin in order to be considered for testing. [type = integer] [values >= 0] [default = 9]\n";
  out << "Significance    : Null hypothesis (no outliers) is rejected if critical value > significance [type = float] [range = [0.04, 0.99], default = 0.04]\n";
  out << "SkipBinModulo   : Skip every N'th bin. [type = integer] [values = [0, #of bins in 1d hist], default: skip none]\n";
  out << "----------------------------------------------------------------------------------------------" << std::endl;  
}
