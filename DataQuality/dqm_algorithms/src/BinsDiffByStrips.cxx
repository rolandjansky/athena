/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsDiffByStrips.cxx calculates average bin value for a strip of bins and finds bins that are outliers from that strip. DQ decision based on worst bin only, not number of bad bins.
 * \author Evan Wulf
 */



#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BinsDiffByStrips.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/AlgorithmManager.h>

#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TClass.h>
#include <TObjArray.h>
#include <TMath.h>

#include <math.h>
#include <string>

using namespace std;

static dqm_algorithms::BinsDiffByStrips myInstance;

dqm_algorithms::BinsDiffByStrips::BinsDiffByStrips( )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("BinsDiffByStrips", this);
  TH1::AddDirectory(false);
}

dqm_algorithms::BinsDiffByStrips::~BinsDiffByStrips()
{
}

dqm_algorithms::BinsDiffByStrips * 
dqm_algorithms::BinsDiffByStrips::clone()
{
  
  return new BinsDiffByStrips();
}


dqm_core::Result *
dqm_algorithms::BinsDiffByStrips::execute(const std::string &  name, 
					  const TObject& object, 
					  const dqm_core::AlgorithmConfig& config ){ 
  
  dqm_core::Result* result = new dqm_core::Result();

  //==========================================================================
  //-- Input Histogram Retrieval --
  //==========================================================================
  // Retrieve input object, test if it is a valid 2D histogram:
  
  TH1* histogram;
  
  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)&object;
    if (histogram->GetDimension() != 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension != 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }

  //==========================================================================
  //-- Algorithm Configuration --
  //==========================================================================
  // Loading of parameters, thresholds from config, 
  
  // Thresholds:
  //-------------------
  double gthreshold;
  double rthreshold;
  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "MaxDeviation", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "MaxDeviation", config.getGreenThresholds() );
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  // Configuration Parameters:
  //-------------------------------

 
  // Parameters giving the minimum number(fraction) of bins with a certain status to change the histogram level result
  // (If the histogram status is not determined by a Chi-Squared test):
  const int nRedBinsToRedStatus = (int) dqm_algorithms::tools::GetFirstFromMap("NBinsRedToRedStatus", config.getParameters(), 1); 
  const double redFracToRedStatus = dqm_algorithms::tools::GetFirstFromMap("RedFractionToRedStatus", config.getParameters(), 0.01);
  const int nYellowBinsToYellowStatus = (int) dqm_algorithms::tools::GetFirstFromMap("NBinsYellowToYellowStatus", config.getParameters(), 1); 
  const double yellowFracToYellowStatus = dqm_algorithms::tools::GetFirstFromMap("YellowFractionToYellowStatus", config.getParameters(), 0.05); 
  const double greenFracToGreenStatus = dqm_algorithms::tools::GetFirstFromMap("GreenFractionToGreenStatus", config.getParameters(), 0.5); 

  // Minimum statistics for test:
  const double minStat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  if ( histogram->GetEntries() < minStat ) {
    result->status_ = dqm_core::Result::Undefined;
    return result;
  }

  // Parameter for exclusion of bins from consideration:
  const double ignoreValue = dqm_algorithms::tools::GetFirstFromMap( "ignoreval", config.getParameters(), -99999);
  const double minError = dqm_algorithms::tools::GetFirstFromMap( "minError", config.getParameters(), /*0.00001*/ -1); 

  // Parameter giving the range of the histogram to be tested:  
  vector<int> range;
  try{
  range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  }
  catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  const bool yStrips = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap( "useStripsOfConstantY", config.getParameters(), 0));
	
  // Parameter giving minimal statistical significance for bin test results:
  const double sigmaThresh = dqm_algorithms::tools::GetFirstFromMap("SigmaThresh",  config.getParameters(), 5); 
  // Tag publishing options:
  const bool publish    = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap( "PublishBins",    config.getParameters(), 0));
  const bool publishRed = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap( "PublishRedBins", config.getParameters(), 0));
  int maxPublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 50); 
  if (maxPublish > 999) maxPublish=999;

  // Clustering options:
  const bool clusterResults = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap( "ClusterResults", config.getParameters(), 1));
  const double seedThreshold = dqm_algorithms::tools::GetFirstFromMap( "SeedThreshold", config.getParameters(), rthreshold);
  const double growthThreshold = dqm_algorithms::tools::GetFirstFromMap( "GrowthThreshold", config.getParameters(), gthreshold);
  const int topology = static_cast<int>(dqm_algorithms::tools::GetFirstFromMap( "Topology", config.getParameters(), tools::CylinderX));

  // Test Directionality: exclude bins over(under) average by setting to a value other than 1:
  const bool greaterThan = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap( "GreaterThan", config.getParameters(), 0));
  const bool lessThan = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap( "LessThan", config.getParameters(), 0));
  bool findBinsOverAvg = true;
  bool findBinsUnderAvg = true;

  if (!lessThan && greaterThan) {
    findBinsUnderAvg = false;
  }
  else if (!greaterThan && lessThan) {
    findBinsOverAvg = false;
  }



  // Master switch: trust errors and make this algorithm a test of whether that trust is warranted: (changes three defaults and final test)
  const bool testConsistencyWithErrors = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("TestConsistencyWithErrors", config.getParameters(), 0));

  // Switch to use the mean error instead of the variance as the yard stick by which deviations are measured:
  const bool useMeanErrorForScale = (testConsistencyWithErrors || static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("UseMeanErrorForScale", config.getParameters(), 0)));

  // Switch to decide whether to do a Chi-Squared test:
  const bool doChiSquaredTest = (testConsistencyWithErrors || static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("DoChiSquaredTest", config.getParameters(), 0)));

  // Minimum bins required before(after) removal of outliers for testing of a strip to proceed:

  //minimum number of bins left after skimming for bins in this strip to be tested:
  int  minBinsBeforeSkimming = (int) dqm_algorithms::tools::GetFirstFromMap("MinBinsBeforeSkimming",  config.getParameters(), -1); 
  if (minBinsBeforeSkimming < 3) {
    if ( !testConsistencyWithErrors ) minBinsBeforeSkimming = 50; 
    else minBinsBeforeSkimming = 8;
  }
  //minimum number of bins left after skimming for bins in this strip to be tested:
  int  minBinsAfterSkimming = (int) dqm_algorithms::tools::GetFirstFromMap("MinBinsAfterSkimming",  config.getParameters(), -1); 
  if (minBinsAfterSkimming < 2) {
    if ( !testConsistencyWithErrors ) minBinsAfterSkimming = 8; 
    else minBinsAfterSkimming = 3;
  }

  // What to do if minimum bin requirement is not met: mark all bins as "Undefined" or add them all to the next strip:
  const bool combineStripsBeforeSkimming = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("CombineStripsBeforeSkimming",  config.getParameters(), 1));

  // Parameter to declare a bin as green regardless of its relative deviation: (Also will exclude this bin from any clustering of bad bins)
  double absDiffGreenThresh = dqm_algorithms::tools::GetFirstFromMap("AbsDiffGreenThresh", config.getParameters(), 0); 

  //  double maxdiffabs = dqm_algorithms::tools::GetFirstFromMap( "MaxDiffAbs", config.getParameters(), -1); 
  //  double maxdiffrel = dqm_algorithms::tools::GetFirstFromMap( "MaxDiffRel", config.getParameters(), -1);


  
  // Parameters for configuring the iterative oulier identification process:

  // Switch to use error based outlier identification:
  const bool findOutliersUsingErrors = (testConsistencyWithErrors || static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("FindOutliersUsingErrors", config.getParameters(), 0)));

  // Switch to configure non error based outlier identification: (id based on shape of distribution)
  int nIter = (int) dqm_algorithms::tools::GetFirstFromMap( "nIterations", config.getParameters(), 100); //Defines a maximum: iteration will terminate on stabilization.
  double iterThresh = dqm_algorithms::tools::GetFirstFromMap("IterDeviationThresh",  config.getParameters(), 2.075); 
  double iVarExp = dqm_algorithms::tools::GetFirstFromMap("IterVariationExponent",  config.getParameters(), 0.275); 
  double isbf = dqm_algorithms::tools::GetFirstFromMap("IterScaleBiasCorrectionFactor",  config.getParameters(), 0.4); 
  double ibc = dqm_algorithms::tools::GetFirstFromMap("IterBreakConstant",  config.getParameters(), 25); 

// empty ratio
  double emptyRatio = dqm_algorithms::tools::GetFirstFromMap("EmptyRatio",
config.getParameters(), 0.3);
  double outstandingRatio = dqm_algorithms::tools::GetFirstFromMap("OutstandingRatio",config.getParameters(), 50);
//test_begin
/*
out<<"histogram Name: "<<histogram->GetName()<<std::endl;
out<<"nIter : "<<nIter<<std::endl;
out<<"iterThresh: "<<iterThresh<<std::endl;
out<<"iVarExp : "<<iVarExp<<std::endl;
out<<"isbf: "<<isbf<<std::endl;
out<<"ibc: "<<ibc<<std::endl;
*/
//test_end
  //=======================================================
  //-- Setup --
  //=======================================================
  //Prepare for testing

  int ixmax = range[1] - range[0] + 1;
  int iymax = range[3] - range[2] + 1;

  int ismax = ixmax;
  int inmax = iymax;
  if( yStrips) {
    ismax = iymax;
    inmax = ixmax;
  }

  //Build vectors to store the axis information more efficiently, keeping to Root's use of overflow bins to keep things
  //intuitive, aswell as temporary arrays to facilitate booking of results histograms:
  vector<double> xBinCenters;
  vector<double> yBinCenters;
  double * xBinEdges = new double[ixmax+1];
  double * yBinEdges = new double[iymax+1];

  //X-Axis:
  xBinCenters.push_back( histogram->GetXaxis()->GetBinLowEdge(range[0]) ); //Underflow bin
  for ( int i = range[0]; i <= range[1]; i++ ) {
    xBinCenters.push_back( histogram->GetXaxis()->GetBinCenter(i) );
    xBinEdges[i-range[0]] =  histogram->GetXaxis()->GetBinLowEdge(i);
  }
  xBinEdges[ixmax] =  histogram->GetXaxis()->GetBinUpEdge(range[1]);
  xBinCenters.push_back( xBinEdges[ixmax] ); //Overflow bin

  //Y-Axis:
  yBinCenters.push_back( histogram->GetYaxis()->GetBinLowEdge(range[2]) ); //Underflow bin
		
  for ( int j = range[2]; j <= range[3]; j++ ) {
    yBinCenters.push_back( histogram->GetYaxis()->GetBinCenter(j) );
    yBinEdges[j-range[2]] =  histogram->GetYaxis()->GetBinLowEdge(j);
  }
  yBinEdges[iymax] =  histogram->GetYaxis()->GetBinUpEdge(range[3]);
  yBinCenters.push_back( yBinEdges[iymax] ); //Overflow bin

  double * sBinEdges = xBinEdges;
  if( yStrips ) {
    sBinEdges = yBinEdges;
  }

  //Book result histograms:
  TH2F* inputBins = new TH2F("inputBins", histogram->GetTitle(), ixmax, xBinEdges, iymax, yBinEdges);
  TH2F* binwiseStatus = new TH2F("binewiseStatus", "BinsDiffByStrips Status Results per Bin", ixmax, xBinEdges, 
				 iymax, yBinEdges);
  TH2F* binDeviations = new TH2F("binDeviations","Input bin content normalized by skimmed average and variance per strip", 
				 ixmax, xBinEdges, iymax, yBinEdges);
  TH1F* stripAverages = new TH1F("stripAverages", "Average Value from Cells in Strip After Removal of Outliers", 
				 ismax, sBinEdges);
  TH1F* stripVariances = new TH1F("stripVariances","Standard Variance of Cells in Strip After Removal of Outliers", 
				  ismax, sBinEdges);
  
  delete[] xBinEdges;
  delete[] yBinEdges;
  //Intialize result variables:
  double maxDeviation = 0;
  std::vector <tools::binContainer> yellowBins;
  std::vector <tools::binContainer> redBins;
  int nBinsRed = 0;
  int nBinsYellow = 0;
  int nBinsGreen = 0;
  int nBinsUndefined = 0;
  int nBinsDisabled = 0;

 
  //===============================================================================================
  //-- Test --
  //===============================================================================================
  // Examination of bin values, filling of result histograms, and determination of dq result:

  vector<vector<tools::binContainer> > strips;
//test_change begin
vector<tools::binContainer> AllBinInOneStrip;
double emptyBinCounter=0;
int name_flag = 0;
//test_change end
  //Loop to process histogram and pack it into a vector of strip vectors:
  for ( int is = 1; is <= ismax; is++ ) {
    vector<tools::binContainer> inputs;
 
    int ix = is;
    int iy = 1;
    if ( yStrips ) {
      iy = is;
      ix = 1;
    }
    for ( int in = 1; in <= inmax; in++ ) {
      
      if ( yStrips ) {
	ix = in;
      }
      else {
	iy = in;
      }
      
      int bin = histogram->GetBin(ix + range[0]-1,iy + range[2]-1);
      double value = histogram->GetBinContent(bin);
      double error = histogram->GetBinError(bin);
      inputBins->SetBinContent(ix,iy,value);
      inputBins->SetBinError(ix,iy,error);
//test_change begin
      if(value==0) emptyBinCounter++;
//test_change end
tools::binContainer binContent_tmp = {value,error,1,ix,iy,xBinCenters[ix],yBinCenters[iy] };
AllBinInOneStrip.push_back(binContent_tmp);
      if( (value != ignoreValue) && (error > minError) ){
      tools::binContainer binContent = {value,error,1,ix,iy,xBinCenters[ix],yBinCenters[iy] };

	 inputs.push_back(binContent);
//        AllBinInOneStrip.push_back(binContent);
      }
      else {
	nBinsDisabled++;
	binwiseStatus->SetBinContent(ix,iy,dqm_core::Result::Disabled);
      }
    }

    // If there are not enough bins in this strip and we are not combining strips, then 
    // all the active bins in this strip should be considered undefined and ignored.
    if( (!combineStripsBeforeSkimming) && ((int)inputs.size() < minBinsBeforeSkimming) ) {
      nBinsUndefined += inputs.size();
    }
    else if(inputs.size() != 0) {
      strips.push_back(inputs);
    }
  }

  //If combine strips before skimming is on, combine the strips with the fewest active bins with their neighbors.
  if( combineStripsBeforeSkimming ) {
    tools::MergePastMinStat(strips,minBinsBeforeSkimming);
  }


  //Vectors to store values for chi-squared test, if one is performed: (could one be performed on deviations alone?)
  vector<double> inputValues;
  vector<double> inputErrors;
  vector<double> stripAveragesVector;
  vector<double> stripErrors;

  vector<tools::binContainer> deviations;
  //Loop over strips:
  for( vector<vector<tools::binContainer> >::iterator stripItr = strips.begin(); stripItr != strips.end(); ++stripItr ) {
//test_change begin
//out<<"************ im here 1************"<<std::endl;
//test_change end
    if ( stripItr->empty() ) {
//out<<" this strip is empty"<<std::endl;
      continue;
    }

    //Find outliers and average when outliers are exluded:
    double stripAvg = 0;
    double iScale = 0;
    int nIn = 0;
    int nTot = stripItr->size();
    if( findOutliersUsingErrors ) {
      tools::findOutliersUsingErrors(*stripItr, stripAvg, iScale, nIn, absDiffGreenThresh);
    }
    else {
      tools::findOutliers(*stripItr, stripAvg, iScale, nIn, nIter, iVarExp , iterThresh, isbf, ibc);
    }

    // Check if there are enough bins after outliers skimming for any bins from this strip to be flagged, 
    // if not, either label them all as undefined (error = -1) or combine with the next strip:
    // (Note: in the second case we do not reset binContent.test: 
    //        outliers found here will be excluded in the first iteration when they are processed with the next strip)
//test_change begin
//out<<"iScal: "<<iScale<<std::endl;
//out<<"stripAvg: "<<stripAvg<<std::endl;
//out<<"nIn: "<<nIn<<std::endl;
//out<<"minBinsAfterSkimming: "<<minBinsAfterSkimming<<std::endl;
//test_change end
    if ( (nIn < minBinsAfterSkimming) ) {
      for (vector<tools::binContainer>::const_iterator it = stripItr->begin(); it != stripItr->end(); ++it) {
	tools::binContainer deviationContainer = { 0, -3, -13, it->ix, it->iy, it->x, it->y };
	deviations.push_back(deviationContainer);
      }
      continue; //Next strip;
    }

  
    double stripVariance = 0;
    double stripVarianceError = 0;
    double stripAvgError = 0;
    double err2Sum = 0;
//  out<<"useMeanErrorForScale : "<<useMeanErrorForScale<<std::endl; 

    if( !useMeanErrorForScale ) {
//  out<<"********** im here 2 *************"<<std::endl;
      // We will now calculate the variance with two methods, one using the the number of bins in (nIn), the number out, 
      // and range from above, and one looking at the standard variance of the nIn bins in inputs marked as good by 
      // findOutliers. We will then take a weighted average of the two measures as our final estimate of the variance of
      // the underlying, outlier free population.

      // Get range and measures of distribution:
      double S2 = 0;
      double sumSquaredDiffFromAvg = 0;
      double sumCompensator = 0; 
      double err2Diff2Sum = 0;
      double min = 0;
      double max = 0;
      nIn = 0;
  
      
      for (vector<tools::binContainer>::const_iterator it = stripItr->begin(); it != stripItr->end(); ++it) {
	//it->test will evaluate as true iff tools::findOutliers determined it was sufficiently close to the mean 
	// to not be a variance spoiling outlier:
         
	if(it->test){
	  if( nIn == 0 ) {
	    min = it->value;
	    max = it->value;
	  }
	  if( it->value > max ) max = it->value;
	  if( it->value < min ) min = it->value;
	  nIn++;
	  
	  double diffFromAvg = it->value - stripAvg;
	  sumSquaredDiffFromAvg += pow( diffFromAvg, 2);
	  sumCompensator += ( diffFromAvg ); //Use to compensate for floating point issues if they should crop up: 
	  // (would be zero if precision was perfect)
	  
	  double inputErr2 = pow(it->error,2.);
	  err2Sum += inputErr2;
	  err2Diff2Sum += inputErr2 * pow( diffFromAvg, 2);
	}
      }  

      // Record the range 
      double range = max - min;
       
        
      //Method 1: ErfInverse:
      double countingVariance = 0;
      double countingWeight = 0;
//      out<<"nTot: "<<nTot<<std::endl;
//      out<<"range: "<< range<<"  max: "<<max<<"  min: "<<min<<std::endl;
      if( (nTot != 0) && (range != 0) && ((nTot - nIn) != 0) ) {
	double erfin = TMath::ErfInverse( (1.0 * nIn) / nTot );
	double erfin2 = pow(erfin,2);
//        out<<"erfin: "<<erfin<<std::endl;
//        out<<"erfin2: "<<erfin2<<std::endl;
	if( erfin2 != 0 ) {
	  countingVariance = range / (erfin * 2 * sqrt(2));
	  countingWeight = 8 * pow(erfin2 / (range * exp(erfin2)),2) * pow(1.0 * nTot,3.) / (TMath::Pi() * nIn * (nTot - nIn));
	}
      }

      //Method 2: Compensated Bounded Variance:
      double boundedVariance = 0;
      double boundedWeight = 0;
      double sumWeights = 0;
      if(nIn > 2) {
	S2 = (sumSquaredDiffFromAvg - (pow(sumCompensator,2)/nIn) )/(nIn - 1); 
	boundedVariance = sqrt(S2);
	double boundEffect = 1;
	double U = 0;
	if( countingVariance != 0 ) {
	  U = range / (2 * countingVariance);
	}
	else if(  boundedVariance != 0 ) {
	  U = range / (2 * boundedVariance);
	}
	if( U != 0 ) {
	  boundEffect = TMath::Erf( U / 2 ) ;
	}
	if(boundEffect != 0) {
	  boundedVariance = boundedVariance / boundEffect;
	}
	double boundedErr2 = 0;
	if (S2 != 0) {
	  boundedErr2 = ( pow( err2Diff2Sum / (pow(nIn-1,2.) * S2), 2) + pow( boundedVariance*(1 - boundEffect)/2,2) );
	}
	if( boundedErr2 != 0 ) {
	  boundedWeight = 1/boundedErr2;
	}
	sumWeights = ( countingWeight + boundedWeight );
	stripVariance = 0;
	if(sumWeights != 0) {
	  //Recalculate boundEffect and rederive boundedVariance using preliminary combined strip variance:
	  stripVariance = (countingVariance * countingWeight + boundedVariance * boundedWeight ) / sumWeights;
	  if(stripVariance != 0 ) {
	    boundEffect = TMath::Erf( range / (4 * stripVariance) );
	    if( boundEffect != 0 ) {
	      boundedVariance = sqrt(S2) / boundEffect;
	      boundedErr2 = ( pow( err2Diff2Sum / (pow(nIn-1,2.) * S2), 2) + pow( boundedVariance*(1 - boundEffect)/2,2) );
	    }
	  }
	}
//test_change begin
//out<<"iScale: "<<iScale<<std::endl;
//test_change end

      }
      else if ( iScale != 0 ){
	//Use the iScale as a last resort, as method 1 is not very good with nIn <= 2 either, but give it a big error
	boundedVariance = iScale;
	boundedWeight = pow(iScale, -2);
      }
    
      // Make final Combination of the two variance estimations, weighted with their errors:
      sumWeights = ( countingWeight + boundedWeight );
      stripVariance = 0;
//test_change begin
//out<<"countingWeight: "<<countingWeight<<std::endl;
//out<<"boundedWeight: "<<boundedWeight<<std::endl;
//out<<"sumWeights: "<<sumWeights<<std::endl;
//test_change end
      if(sumWeights != 0) {
	stripVariance = (countingVariance * countingWeight + boundedVariance * boundedWeight ) / sumWeights;
	stripVarianceError = 1/sqrt(sumWeights);
//test_change begin
//   out<<"stripVariance: "<<stripVariance<<std::endl;
//   out<<"stripVariance Error: "<<stripVarianceError<<std::endl;
 
//test_change end
      }
    }
    else if (nIn > 2) {
      // Just calculate error based quantities and use these:
      for (vector<tools::binContainer>::const_iterator it = stripItr->begin(); it != stripItr->end(); ++it) {
	//it->test will evaluate as true iff tools::findOutliers determined it was sufficiently close to the mean 
	// to not be a variance spoiling outlier:
	if(it->test){
	  err2Sum += pow(it->error,2.);
	}
      }
      stripVariance = sqrt( err2Sum / nIn ); //<- what we would expect the variance to be if the errors are correct
      stripVarianceError = stripVariance / sqrt( nIn );
    }
 
     
    // Calculate error on the mean:
    stripAvgError = sqrt(err2Sum)/nIn;

    int is = 0;
    if (yStrips) {
      is = stripItr->front().iy;
    }
    else {
      is = stripItr->front().ix;
    }

    // Store final skimmed Average and Variance found for this strip:
    stripAverages->SetBinContent(is,stripAvg);
    stripAverages->SetBinError(is,stripAvgError);
    stripVariances->SetBinContent(is,stripVariance);
    stripVariances->SetBinError(is,stripVarianceError);


    if(doChiSquaredTest) {
      for (vector<tools::binContainer>::const_iterator it = stripItr->begin(); it != stripItr->end(); ++it) {
	inputValues.push_back( it->value );
	inputErrors.push_back( 0. );
	stripAveragesVector.push_back( stripAvg );
	stripErrors.push_back( stripVariance );
      }
    }
 
    //Now find the deviation from the strip average, in multiples of the strip variance, for each bin in this strip:
    for (vector<tools::binContainer>::const_iterator it = stripItr->begin(); it != stripItr->end(); ++it) {
//out<<"********** im here 5 *************"<<std::endl;
      double deviation = 0;
      double deviationError = -1;
      double diffFromAvg = it->value - stripAvg;
/*      if(it->iy==26 && it->ix==71) {
          out<<"bin value: "<<it->value<<std::endl;
          out<<"bin error: "<<it->error<<std::endl;
          out<<"stripAvg: "<<stripAvg<<std::endl;
          out<<"stripAvgError: "<<stripAvgError<<std::endl;
          out<<"diffFromAvg: "<<diffFromAvg<<std::endl;
          out<<"stripVariance: "<<stripVariance<<std::endl;
          out<<"stripVarianceError: "<<stripVarianceError<<std::endl;           
     }
*/
// test_change begin
        if(stripVariance > 0.00001){
	deviation = diffFromAvg / stripVariance;
//        if(it->iy==26 && it->ix==71) out<<"deviation: "<<deviation<<std::endl;
	if ( nIn > 1 ) {
	  deviationError = sqrt( (( pow(it->error,2) + pow(stripAvgError,2) ) / pow(stripVariance,2))
				 + ( pow(diffFromAvg,2) * pow(stripVarianceError,2) / pow(stripVariance,4)) );
/*        if(it->iy==26 && it->ix==71) {
            out<<"deviationError: "<<deviationError<<std::endl;
            out<<" eta & phi: "<<it->x << "  "<< it->y<<std::endl;
        }
*/
	}
      }
      else {deviation=0;
            deviationError=0;
            if( (it->test==0) && (fabs(it->value)/fabs(stripAvg)) > outstandingRatio ) {
              name_flag = 1; 
       //       out<<"outstandingRatio "<<outstandingRatio<<std::endl;
            }
      }  //test_change
      int binStatus = 0;
      if ( fabs(diffFromAvg) < (absDiffGreenThresh + sqrt(pow(it->error,2) + (err2Sum/nIn))) ) {
	binStatus = 3;
      }
      //Save bin for future processing and tests:
      tools::binContainer deviationContainer = { deviation , deviationError, binStatus, it->ix, it->iy, it->x, it->y };
      deviations.push_back(deviationContainer);
      //Write bin to result histogram:
      int bin = binDeviations->GetBin(it->ix,it->iy);
      binDeviations->SetBinContent(bin,deviation);
//    binDeviations->SetBinError(bin,deviation);
      binDeviations->SetBinError(bin,deviationError); // i change this 
    }
  } 


  //If clustering of results is turned on, cluster the bins according to their deviation:
  vector<tools::binCluster> clusters;
  vector<tools::binCluster> redClusters;
  vector<tools::binCluster> yellowClusters;
  if(clusterResults) {
        
    //First sort the bins:
    std::sort( deviations.begin(), deviations.end(), dqm_algorithms::tools::binContainer::comp );

    //Now map the deviations according to ix, iy, 
    vector<vector<tools::binContainer*> > binMap = makeBinMap(deviations, ixmax, iymax, topology);
    //Now do the clustering:
    for (vector<tools::binContainer>::iterator it = deviations.begin(); it != deviations.end(); ++it) {
      if( (it->value > seedThreshold + it->error) && !it->test ) {
	tools::binCluster cluster = tools::buildCluster(*it,binMap,xBinCenters,yBinCenters,growthThreshold,topology);
	if(cluster.n > 1) {
	  // Only keep clusters with more than 1 bin
	  clusters.push_back(cluster);
	}
	else {
	  // Unmark this bin: it is not in a cluster
	  it->test = 0;
	}
      }
    }
 

    //Score each cluster based on the significance of its total deviation: (Using the same criteria to be used for bins)
    for( vector<tools::binCluster>::const_iterator it = clusters.begin(); it != clusters.end(); ++it) {
      double fabsDeviation = fabs(it->value);
      double significanceBound = sigmaThresh * it->error;
      bool overAvg = ( it->value >= 0 );
      if ( (findBinsOverAvg && overAvg) || (findBinsUnderAvg && !overAvg) ) {
	if ( fabsDeviation > (gthreshold + significanceBound) ) { 
	  if ( fabsDeviation > (rthreshold + significanceBound) ) {
	    nBinsRed += it->n;
	    if  (publish || publishRed) {
	      redClusters.push_back(*it);
	    }
	    continue;
	  }
	  nBinsYellow += it->n;
	  if (publish) {
	    yellowClusters.push_back(*it);
	  }
	}
      }
    }
  }
 
  tools::binContainer maxDevBin = { 0, -999.9, 0, 0, 0, 0, 0 };

  // Now score each bin based on the significance of its deviation:
  for (vector<tools::binContainer>::const_iterator it = deviations.begin(); it != deviations.end(); ++it) {
    int bin = binwiseStatus->GetBin(it->ix,it->iy);
    //Check for bins with no defined deviation / deviation error
    if ( it->error < 0 ) {
      binwiseStatus->SetBinContent(bin,dqm_core::Result::Undefined);
      nBinsUndefined++;
      continue;
    }
    //Check if this is the maximum deviation so far:
    if (fabs(it->value) > fabs(maxDeviation) ) {   // i change the maxDeviation to 
                                                   // fabs(maxDeviation)
      maxDeviation = it->value;
      maxDevBin = *it;
    }
    //Check if this bin was marked as green by a prior test:
    if ( it->test == 3 ) {
      binwiseStatus->SetBinContent(bin,dqm_core::Result::Green);
      nBinsGreen++;
      continue;
    }
 
     
    double fabsDeviation = fabs(it->value);
    double significanceBound = sigmaThresh * it->error;

    //Flag Green bins:
    if ( fabsDeviation < (gthreshold - significanceBound ) ) {
      binwiseStatus->SetBinContent(bin,dqm_core::Result::Green);
      nBinsGreen++;
      continue;
    }
    bool overAvg = ( it->value >= 0 );
    if ( (findBinsOverAvg && overAvg) || (findBinsUnderAvg && !overAvg) ) {
      if ( fabsDeviation > (gthreshold + significanceBound) ) { 
	if ( fabsDeviation > (rthreshold + significanceBound) ) {
	  
	  binwiseStatus->SetBinContent(bin,dqm_core::Result::Red);

	  //Only publish and count red bins here if they were not grouped in any cluster of bad bins:
	  if( it->test != 10 ) {
	    // Using map machinery to ensure proper ordering: worst bins should have preference in publishing.
	    if  (publish || publishRed) {
	      redBins.push_back( *it );
	    }
	    nBinsRed++;
	  }
	  continue;
	}
	
	binwiseStatus->SetBinContent(bin,dqm_core::Result::Yellow);
	//Similarly exclude clustered yellow bins from publishing and counting:
	if( it->test != 10 ) {
	  if (publish) {
	    yellowBins.push_back( *it );
	  }
	  nBinsYellow++;
	}
	continue;
      }
    }
    binwiseStatus->SetBinContent(bin,dqm_core::Result::Undefined);
    nBinsUndefined++;
  }
//test_change begin
tools::binContainer onebin_my={0,0,0,0,0,0,0};
tools::binContainer onebin_my_pre={0,0,0,0,0,0,0};
double maxvalue_pre=-1;
vector<tools::binContainer> topBinEntries;
vector<tools::binContainer> topDeviations;
//int name_flag = 0;
//find_n(histogram->GetName(),name_flag);
//if(histogram->GetEntries() < 20000 ) name_flag = 1; //try to deal with low stat case
double emptyRatio_this = emptyBinCounter/(histogram->GetNbinsX()*histogram->GetNbinsY());
//std::string tmpname ;
//tmpname = histogram->GetName();
//if( tmpname.compare("etaphi_ncellinclus")==0 ) 
//out<<"*********** emptyRatio_this is "<< emptyRatio_this << std::endl;
if (emptyRatio_this > emptyRatio) name_flag=1;
if(name_flag==1){
  int NTopEntries=0;
  if(AllBinInOneStrip.size()>10 ) NTopEntries=10;
  else NTopEntries = AllBinInOneStrip.size();
  int AllBinInOneStrip_size = AllBinInOneStrip.size();
  bool *bin_entries_status = new bool [AllBinInOneStrip_size];
  for(int i=0; i< AllBinInOneStrip_size; i++) bin_entries_status[i] = true;
  for(int i=0;i<NTopEntries;i++){
    double maxvalue=0;
    int counter=0;
    int counter2=0;
    for(vector<tools::binContainer>::const_iterator it =AllBinInOneStrip.begin();it!=AllBinInOneStrip.end();it++){
      int flag_my = i==0 || ( bin_entries_status[counter] && it->value <= maxvalue_pre);
      if(fabs(it->value) >= fabs(maxvalue) && flag_my) {
         maxvalue = it->value; 
         onebin_my = *it; 
         counter2 = counter;
      }
     counter++;
    } 
    bin_entries_status[counter2]=0;
if (onebin_my.x!= onebin_my_pre.x||onebin_my.y!=onebin_my_pre.y) topBinEntries.push_back(onebin_my);
    maxvalue_pre = maxvalue;
    onebin_my_pre = onebin_my;
//    topBinEntries.push_back(onebin_my);
    }
  delete[] bin_entries_status;
}
else {
  int NTopdeviation=0;
  if(deviations.size()>5) NTopdeviation = 5;
  else NTopdeviation = deviations.size();
  int deviations_size = deviations.size();
  bool *bin_dev_status = new bool [ deviations_size];
  for(int i=0; i< deviations_size ; i++) bin_dev_status[i] = true;
  for(int i=0;i<NTopdeviation;i++){
    double maxvalue=0;
    int counter=0;   
    int counter2=0;
    if(deviations.size()!=0){
    for (vector<tools::binContainer>::const_iterator it = deviations.begin(); it != deviations.end(); ++it){
    int flag_my = i==0 || ( bin_dev_status[counter]  && it->value <= maxvalue_pre);
    if(fabs(it->value) >= fabs(maxvalue) && flag_my) {
      maxvalue = it->value; 
      onebin_my = *it; 
      counter2 = counter;
    }
   counter++;
  }
  bin_dev_status[counter2]=0;
 if (onebin_my.x!= onebin_my_pre.x||onebin_my.y!=onebin_my_pre.y) {
  topDeviations.push_back(onebin_my);
  maxvalue_pre = maxvalue;
  onebin_my_pre = onebin_my;
 }
}
//  topDeviations.push_back(onebin_my);

  }
  delete[] bin_dev_status;
 }
  if ( publish || publishRed) {

    int objectsPublished = 0;
    int clustersPublished = 0;
    // Publish red clusters first:
    std::sort( redClusters.begin(), redClusters.end(), dqm_algorithms::tools::binCluster::comp );
    vector<tools::binCluster>::const_reverse_iterator rbcrbegin = redClusters.rbegin();
    vector<tools::binCluster>::const_reverse_iterator rbcrend = redClusters.rend();
    for (vector<tools::binCluster>::const_reverse_iterator it = rbcrbegin; 
	 it != rbcrend; ++it) {
            
      if (objectsPublished < maxPublish) {
      	char ctag[256];
	sprintf(ctag,"C%.3u-R-%.3uBins@ Eta=(%+.3f_to_%+.3f) Phi=(%+.3f_to_%+.3f) Center=(%+.3f,%+.3f) Radius=%+.3f",clustersPublished,it->n,
		xBinCenters[it->ixmin],xBinCenters[it->ixmax],yBinCenters[it->iymin],yBinCenters[it->iymax],it->x,it->y,it->radius);

	string tag = ctag;	
	int sizeDiff = 30 - tag.size();
	if( sizeDiff > 0 ) {
	  tag.append(sizeDiff, '_');
	}
	
	result->tags_[tag] = it->value;
	clustersPublished++;
	objectsPublished++;
      }
    }
 
 
    // Publish red bins next:
    int binsPublished = 0;
    std::sort( redBins.begin(), redBins.end(), dqm_algorithms::tools::binContainer::comp );
 
    for ( std::vector<tools::binContainer>::const_reverse_iterator rIter = redBins.rbegin();
	  rIter != redBins.rend();
	  ++rIter) {
      if (objectsPublished < maxPublish) {
	char ctag[16];
	sprintf(ctag,"%.3u-R-",binsPublished);
	std::string tag = ctag;
	tools::MakeBinTag(*rIter,tag);
	result->tags_[tag] = rIter->value;
	binsPublished++;
	objectsPublished++;
      }
      else {
	break;
      }
    }
  
    // Now publish yellow bins:
    if ( publish ) {
      std::sort( yellowBins.begin(), yellowBins.end(), dqm_algorithms::tools::binContainer::comp );
      for ( std::vector<tools::binContainer>::const_reverse_iterator rIter = yellowBins.rbegin();
	    rIter != yellowBins.rend();
	    ++rIter) {
	if (objectsPublished < maxPublish) {
	  char ctag[16];
	  sprintf(ctag,"%.3u-Y-",binsPublished);
	  std::string tag = ctag;
	  tools::MakeBinTag(*rIter,tag);
	  result->tags_[tag] = rIter->value;
	  binsPublished++;
	  objectsPublished++;

	}
	else {
	  break;
	}
      }
    }
    //Lastly publish yellow clusters: (What are these, anyway?)
    if ( publish ) {
      std::sort( yellowClusters.begin(), yellowClusters.end(), dqm_algorithms::tools::binCluster::comp );
      for (vector<tools::binCluster>::const_reverse_iterator it = yellowClusters.rbegin(); 
	   it != yellowClusters.rend(); ++it) {
	if (objectsPublished < maxPublish) {
	  char ctag[256];
	  sprintf(ctag,"C%.3u-Y-%.3uBins@ Eta=(%+.3f_to_%+.3f) Phi=(%+.3f_to_%+.3f) Center=(%+.3f,%+.3f) Radius=%+.3f",clustersPublished,it->n,
		  xBinCenters[it->ixmin],xBinCenters[it->ixmax],yBinCenters[it->iymin],yBinCenters[it->iymax],it->x,it->y,it->radius);
	  string tag = ctag;
	  int sizeDiff = 30 - tag.size();
	  if( sizeDiff > 0 ) {
	    tag.append(sizeDiff, '_');
	  }
	  result->tags_[tag] = it->value;
	  clustersPublished++;
	  objectsPublished++;
	}
      }
    }

  }
  result->tags_["NBins_RED"] = nBinsRed;
  result->tags_["NBins_YELLOW"] = nBinsYellow;
/*  
  if( maxDevBin.error != -999.9 ) {
    std::string devString = "MaxDeviation-";
    tools::MakeBinTag(maxDevBin,devString);
    result->tags_[devString] = maxDeviation;
  }
*/
//test_change begin
if(name_flag!=1){
  for(unsigned int i=0;i<topDeviations.size();i++){
  if(topDeviations[i].error !=-999.9 ) {
  char tmp[100];
  sprintf(tmp,"MaxDeviation%i-",i);
  std::string myString = tmp;
  tools::MakeBinTag(topDeviations[i], myString);
  result->tags_[myString] = topDeviations[i].value;
  }
 }
}
if(name_flag==1) {
  if( maxDevBin.error != -999.9 ) {
    std::string devString = "MaxDeviation-";
    tools::MakeBinTag(maxDevBin,devString);    
    result->tags_[devString] = maxDeviation;
  } 
  for(unsigned int i=0;i<topBinEntries.size();i++){
  char tmp[100];
  sprintf(tmp,"LeadingBinContents%i-",i);
  std::string myString = tmp;
  tools::MakeBinTag(topBinEntries[i], myString);
  result->tags_[myString] = topBinEntries[i].value;
  }
}
//test_change end

  result->tags_["Algorithm--BinsDiffByStrips"] = 5; //Provide algorithm name and number of results histograms in TObjArray, for convenience of summary maker:

  //Determine if the configuration wants this to be part of an AndGroup, used by BinwiseSummary to require reds from the
  // same bin in multiple plots for red to be propogated.
  const double andGroup = dqm_algorithms::tools::GetFirstFromMap("AndGroup",  config.getParameters(), -99999); 
  if( andGroup != -99999 ) {
    result->tags_["AndGroup"] = andGroup;
  }


  TObjArray * resultList = new TObjArray(5,0);
 
  //This order is important! Add new histograms at the end of the list only to avoid breaking summarymakers and be sure to increase
  // the alocated capacity of the TObjArray set in the constructor above.
  resultList->Add(binwiseStatus);
  resultList->Add(inputBins);
  resultList->Add(binDeviations);
  resultList->Add(stripAverages);
  resultList->Add(stripVariances);

  resultList->SetOwner(true);
 
  result->object_ =  (boost::shared_ptr<TObject>)(TObject*)(resultList);
  

  if( doChiSquaredTest ) {
    
    std::pair<double,double> chiSquareResult = dqm_algorithms::tools::CalcBinsProbChisq(inputValues,inputErrors,stripAveragesVector,stripErrors);
    result->tags_["SigmaChiSq"] = chiSquareResult.second;
 
    if ( testConsistencyWithErrors ) {
      //Determine status based on quality of chi squared fit:
      if (  chiSquareResult.second <= gthreshold ) {
	result->status_ = dqm_core::Result::Green;
      } else if (  chiSquareResult.second < rthreshold ) {
	result->status_ = dqm_core::Result::Yellow;
      } else {
	result->status_ = dqm_core::Result::Red;
      }
      
      return result;
    }
  }

  // Determine status based on the number of red, yellow, green, and other bins:
  int nActiveBins = nBinsGreen + nBinsRed + nBinsYellow + nBinsUndefined;
  if( nActiveBins != 0 ) {
    if ( (nBinsRed >= nRedBinsToRedStatus) || ((nBinsRed * 1.0 / nActiveBins) > redFracToRedStatus) ) {
      result->status_ = dqm_core::Result::Red;
    }
    else if ( ((nBinsRed + nBinsYellow) >= nYellowBinsToYellowStatus) 
	      || (((nBinsRed + nBinsYellow) / nActiveBins) > yellowFracToYellowStatus) ) {
      result->status_ = dqm_core::Result::Yellow;
    }
    else if ( (nBinsGreen * 1.0 / nActiveBins ) >= greenFracToGreenStatus ) {
      result->status_ = dqm_core::Result::Green;
    }
    else {
      result->status_ = dqm_core::Result::Undefined;
    }
  }
  else {
    result->status_ = dqm_core::Result::Disabled;
  }
  return result;
}


void
dqm_algorithms::BinsDiffByStrips::printDescription(std::ostream& out)
{
  
  out<<"BinsDiffByStrips: Calculates Average bin value and variance for each strip of constant x and finds bins in that strip that our outliers from the mean\n"<<std::endl;
  out<<"Mandatory Green/Red Threshold: MaxDeviation: size of deviation from mean, in multiples of variance, for bin to be considered Green/Red. Overall result is result for worst bin"<<std::endl;
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to return defined result"<<std::endl;
  out<<"Optional Parameter: ignoreval: value to be ignored for calculating average, variance, and identifying bad bins. Ignored bins are flagged as Disabled"<<std::endl;
  out<<"Optional Parameter: useStripsOfConstantY: compare bins in strips of constant Y rather than constant X, as is the defualt (set to 1 for true). Default = false"<<std::endl;

  out<<"Optional Parameter: xmin: minimum x range"<<std::endl;
  out<<"Optional Parameter: xmax: maximum x range"<<std::endl;
  out<<"Optional Parameter: ymin: minimum y range"<<std::endl;
  out<<"Optional Parameter: ymax: maximum y range"<<std::endl;
  out<<"Optional Parameter: GreaterThan: check for bins which are GreaterThan average (set to 1 for true). Default = true"<<std::endl; 
  out<<"Optional Parameter: LessThan: check for bins which are LessThan average (set to 1 for true). Default = true"<<std::endl; 
  out<<"Optional Parameter: PublishBins: Save all bin quality decisions in output histogram and save Red and Yellow bins in tag output (set to 1 for true). Default = false"<<std::endl;
  out<<"Optional Parameter: PublishRedBins: Save bins identified as Red in tag output(set to 1 for true). Default = false"<<std::endl;
  out<<"Optional Parameter: MaxPublish: Max number of bins to save in tag output. Default = 20"<<std::endl; 
  out<<"Optional Parameter: MinBinsAfterSkimming : Minimum number of bins remaining after skimming for any bins to be flagged as Red/Yellow/Green. If threshold is not met, another attempt will be made at the iteration process with these few bin excluded in the first pass. Failing success, the entire strip will be flagged as Undefined. Default = 5"<<std::endl;
  out<<"Optional Parameter: MinAbsDiffFromAvg : Bins whose absolute difference from the mean is less than this amount, in a statistically significant fashion, will always be flagged as green."
	   <<"Bin will be flagged as either Green or Undefined, depending on significance by which it passes this cut."<<std::endl;
  out<<"Optional Parameter: nIterations : Number of iterations to be used in calculating mean and variance while skimming outliers. Default = 10" <<std::endl;
  out<<"Optional Parameter: MaxNRetryIteration : Maxiumum number of times the iteration should be restarted should it fail (by leaving fewer than MinBinsAfterSkimming), reseeding with those cells that pass the iteration removed in the first pass. If MaxNRetryIteration = 0, will never retry. Default = 5. " <<std::endl;
  out<<"Optional Parameter: IterVariationExponent: exponent k used in calculating the generalized variance, iVar, where iVar = (sum[ (abs[ x - xbar ])^k ])^(1/k). If k = 2, for instance, iVar is equivalent to the standard variance. (If nIterations = 1, this parameter has no effect.) Default = -1.4235"<<std::endl;
  out<<"Optional Parameter: IterDeviationThresh : size of deviation from mean, in multiples of the generalized variance, for bin to be excluded during iterative calculation of mean and variance. (If nIterations = 1, this parameter has no effect.) Default = 1.3"<<std::endl;
  
  out<<"Optional Parameter: SigmaThresh : Minimum significance (distance from threshold in multiples of the error) for quality statement to be made about a bin). Default = 5"<<std::endl;
  out<<"Optional Parameter: FindOutliersUsingErrors : Switch to use outlier finding based on the pull of the mean of each bin given its error as compared to the error on the mean, rather than the default error independent aproach."<<std::endl;
  out<<"Optional Parameter: UseMeanErrorForScale : Use the average error of the non-outliers, rather than their estimated variance, as the scale against which deviations are measured."<<std::endl;
  out<<"Optional Parameter: DoChiSquaredTest : Calculate the Chi_Squared value for the distribution of all the bin values given the scale (estimated variance or, if UseMeanErrorForScale is used, mean error."<<std::endl;
  out<<"Optional Parameter: TestConsistencyWithErrors : Switch on FindOutliersUsingErrors, UseMeanErrorForScale, and DoChiSquaredTest and base the DQ decision on the Chi-Squared result"<<std::endl;
}
void
dqm_algorithms::BinsDiffByStrips::find_n(std::string name_tmp,int& name_flag){
  int where = name_tmp.find_last_of("_");
  std::string name;
  name = name_tmp.substr(where+1);
  if(name.compare("5Sigma")==0 || name_tmp.compare("m_EtavsPhi3")==0 || name_tmp.compare("m_EtavsPhi4")==0 ) name_flag=1;
  else name_flag = 0;
}


