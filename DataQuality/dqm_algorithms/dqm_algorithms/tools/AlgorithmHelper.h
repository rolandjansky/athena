/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AlgorithmHelper.cpp does basic functions to get dqm_core::Results from algorithms 
 * \author Haleh Hadavand
 */

#ifndef DQM_ALGORITHMS_TOOLS_ALGORITHMHELPER_H
#define DQM_ALGORITHMS_TOOLS_ALGORITHMHELPER_H

#include <dqm_core/Result.h>
#include <dqm_core/AlgorithmConfig.h>
#include <dqm_core/exceptions.h>

#include <map>
#include <vector>
#include <deque>
#include <list>
#include <utility>

#include <TH1.h>
#include <TObject.h>
#include <TClass.h>
#include <boost/thread/once.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>
#include <iostream>

class TF1;
class TAxis;

namespace dqm_algorithms
{
  namespace tools
  {
    enum AxisType { XYAxes, XAxis, YAxis, XYZAxes, XZAxes, YZAxes, ZAxis };
    
    enum Topology { Rectangle, CylinderX, CylinderY, Torus };

    //Structure to hold the essence of a 2D histogram bin, and be sortable:
    struct binContainer { 
      double value; 
      double error; 
      int test; 
      int ix; 
      int iy; 
      double x;
      double y;
      //Comparison function, for sorting
      inline static bool comp(const binContainer &lhs, const binContainer &rhs) { return fabs(lhs.value) < fabs(rhs.value); }
    };

    struct binCluster {
      double value; 
      double error; 
      double x;
      double y;
      double radius;
      int n;
      int ixmin;
      int ixmax;
      int iymin;
      int iymax;
      inline static bool comp(const binCluster &lhs, const binCluster &rhs) { return fabs(lhs.value) < fabs(rhs.value); }
    };

    std::map<std::string, double > GetFitParams(const TF1 * func);

    std::map<std::string, double > GetFitParamErrors(const TF1 * func);
    
    dqm_core::Result * MakeComparisons( const std::map<std::string,double> & algparams, 
    					const std::map<std::string,double> & gthreshold,
                                        const std::map<std::string,double> & rthreshold );

    dqm_core::Result * CompareWithErrors( const std::map<std::string,double> & algparams, 
					  const std::map<std::string,double> & paramErrors,
					  const std::map<std::string,double> & gthreshold,
					  const std::map<std::string,double> & rthreshold, double minSig);
    
    dqm_core::Result * GetFitResult (const TF1 * func, const dqm_core::AlgorithmConfig & config, double minSig = 0) ;
    
    double GetFirstFromMap(const std::string &paramName, const std::map<std::string, double > &params);
    // mandatory: throws an exception if the parameter is not found

    double GetFirstFromMap(const std::string &paramName, const std::map<std::string, double > &params, double defaultValue);
    // optional: returns defaultValue if the parameter is not found
    
    std::vector<int> GetBinRange(const TH1* histogram, const std::map<std::string, double > & params);
  
    void PublishBin(const TH1 * histogram, int xbin, int ybin, double content, dqm_core::Result *result);

    TH1* DivideByHistogram(const TH1* hNumerator, const TH1* hDenominator); 

    void ModifyHistogram(TH1 * histogram, const dqm_core::AlgorithmConfig & config);

    std::string ExtractAlgorithmName(const dqm_core::AlgorithmConfig& config);

    dqm_core::Result * ExecuteNamedAlgorithm(const std::string & name, const TObject & object, 
					     const dqm_core::AlgorithmConfig & config);
    
    TH1* BookHistogramByExample(const TH1* histogram, const std::string& title, const std::string& name, AxisType axisType);
	
    template <class T>
    const T & GetFromMap( const std::string & pname, const std::map<std::string,T> & params )
    {
	typename std::map<std::string,T>::const_iterator it = params.find( pname );
	if ( it != params.end() ){
	    return it->second;
	}else {
	    throw dqm_core::BadConfig( ERS_HERE, "None", pname );
	}
    }

    /** Helper function used to handle complex reference histograms
     * This function gets as input a reference object and checks if it is a
     * collection of references, in this case extracts and returns the first element.
     \param inputReference : the original reference object
     \param firstReference : the first element of the reference object, 
                             If the input reference is a TCollection, otherwise
			     the inputReference itself
     \param secondReference : the remaining reference(s), if the input collection
                              has 2 elements the secondReference is a TObject,
			      otherwise it is of the same class as the 
			      inputReference
     \throw dqm_core::BadRefHist : in case of wrong references
     */
    void handleReference( const TObject& inputReference , TObject*& firstReference , TObject*& secondReference);
    
    // Function to find outliers in input; iterates over values nIteration times, recalculating mean each time and 
    // removing values that are beyond threshold * scale, where:
    //
    //     scale =  ( sum_in[ abs( value - mean )^ exponent ] / (Nin - 1 - SBCF * Nout) ) ^ ( 1 / exponent ).
    //
    // If all bins are in, and the exponent is two, this is just and unbiased estimator of the standard variance. 
    // SBCF, or the Scale Bias Correction Factor, is an empirical quantity intended to correct for the bias induced from the 
    // bin exclusion process (in a sense, the act of excluding bins could be thought of as decreasing the number of degrees 
    // of freedom). In practice, the SBCF serves to impose an upper bound on the fraction of bins that can be excluded. 


    void findOutliers( std::vector<binContainer>& input, double& mean, double& scale, int& nIn, int nIterations, 
		       double exponent, double threshold, double SBCF = 1., double nStop = 8. );

    void findOutliersUsingErrors( std::vector<binContainer>& input, double& mean, double& meanError, int& nIn, 
				  double mindiff = 0, int minNin = 4);

    // Method for building a cluster:
    binCluster buildCluster( binContainer& seed, const std::vector<std::vector<binContainer*> >& binMap, 
			     const std::vector<double>& xValues, const std::vector<double>& yValues, 
			     double threhold, int topology = CylinderX);

    // Method for mapping binContainer object by their relative positions:
    std::vector<std::vector<binContainer*> >
      makeBinMap(std::vector<dqm_algorithms::tools::binContainer>& bins, int ixmax, int iymax, int topology = CylinderX);

    dqm_core::Result::Status
      WorstCaseAddStatus(dqm_core::Result::Status baseStatus, dqm_core::Result::Status addedStatus, float weight = 1.0); 

    dqm_core::Result::Status
      BestCaseAddStatus(dqm_core::Result::Status baseStatus, dqm_core::Result::Status addedStatus, float weight = 1.0); 
    
    std::pair<double,double> CalcBinsProbChisq(std::vector<double> inputval,std::vector<double> inputerr,
					       double x0, double x0_err);
    std::pair<double,double> CalcBinsProbChisq(std::vector<double> inputval,std::vector<double> inputerr,
					       std::vector<double> x0,std::vector<double> x0_err);
 
    void MergePastMinStat(std::vector<std::vector<tools::binContainer> >& strips, int minStat);

    void MakeBinTag( const binContainer& bin, std::string & tag );

    void FormatToSize( double value, int size, std::string & str, bool showSign = true );

  }
}


#endif // #ifndef DQM_ALGORITHMS_TOOLS_ALGORITHMHELPER_H
