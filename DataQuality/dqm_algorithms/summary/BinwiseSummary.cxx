/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file DivideByHist.cxx file takes a parameter map containing two histograms and divides one by the other.
 * \author Evan Wulf
 */

#ifndef DQM_ALGORITHMS_SUMMARY_DIVIDEBYHIST_CXX
#define DQM_ALGORITHMS_SUMMARY_DIVIDEBYHIST_CXX

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/AlgorithmManager.h>

#include <dqm_core/Result.h> 	 
#include <dqm_core/Parameter.h>

#include <TH1.h>
#include <TClass.h>
#include <TObjArray.h>
#include <dqm_algorithms/summary/BinwiseSummary.h>

#include <dqm_algorithms/tools/SimpleAlgorithmConfig.h>
#include <set>
#include <vector>
#include <map>


#ifndef DQM_ALGORITHMS_MULTIALGORITHMTEST
static dqm_algorithms::summary::BinwiseSummary myInstance;
#endif //#ifndef DQM_ALGORITHMS_MULTIALGORITHMTEST


dqm_algorithms::summary::BinwiseSummary::BinwiseSummary()
{
  dqm_core::AlgorithmManager::instance().registerSummaryMaker("BinwiseSummary",this);
}

dqm_algorithms::summary::BinwiseSummary::~BinwiseSummary()
{
}

dqm_algorithms::summary::BinwiseSummary*
dqm_algorithms::summary::BinwiseSummary::clone()
{
  return new BinwiseSummary();
}

dqm_core::Result*
dqm_algorithms::summary::BinwiseSummary::execute( const std::string &, 
						  const dqm_core::Result &, 
						  const dqm_core::ParametersMap & map)
{

  //General summary maker infrastructure: (This can be resused)
  dqm_core::Result *newresult = new dqm_core::Result();

  //Retrieve axis information for the first histogram:
  int nBinsX = 0;
  int nBinsY = 0;
  TH1* firstBinwiseHist = 0;

  //Loop over elements of parameter map to find an example binwise status histogram, if any, 
  //Extract nbins x and y now so can check against these:
  for (dqm_core::ParametersMap::const_iterator paramIter=map.begin();paramIter!=map.end();++paramIter){
    if ( paramIter->second->getWeight() == 0 ) {
      continue;
    }
    dqm_core::Result * inputResult = paramIter->second->getResult().get();
    TObject* inputobject = inputResult->getObject();
    if (inputobject == 0) {
      continue;
    }
    for (std::map<std::string,double>::const_iterator tagIter=inputResult->tags_.begin();  
	 tagIter != inputResult->tags_.end(); ++tagIter ) {
      if( tagIter->first.find("Algorithm--BinsDiffByStrips") != std::string::npos) {
	if( inputobject->IsA()->InheritsFrom("TObjArray") ) {
	  firstBinwiseHist = (TH1*) ((TObjArray*)inputobject)->First();
	  nBinsX = firstBinwiseHist->GetNbinsX();
	  nBinsY = firstBinwiseHist->GetNbinsY();
	  break;
	}
      }
    }
    if( (nBinsX != 0) || (nBinsY != 0) ) {
      break;
    }
  }
  TH1 * mask = 0;
  std::vector<std::pair<TH1*,float> > binwiseStatHists;
  dqm_core::Result::Status status = dqm_core::Result::Undefined;

  std::map<double,std::vector<std::pair<int,float> > > standardAndGroup;
  std::map<double,std::vector<std::pair<TH1*,float> > > binwiseAndGroup;
  std::set<double> setOfAndGroups;

      
  //Loop over elements of the parameter map to extract histograms and there roles, if any:
  for (dqm_core::ParametersMap::const_iterator paramIter=map.begin();paramIter!=map.end();++paramIter){
    //If weight is 0 skip this result. Allow to "turn off" 
    // results in summary makers
    float weight =  paramIter->second->getWeight();
    if ( weight == 0 ) {
      ERS_DEBUG(2,"Skip result (weight 0): "<<paramIter->first);
      continue;
    }
    dqm_core::Result * inputResult = paramIter->second->getResult().get();
    TObject* inputobject = inputResult->getObject();
    if (inputobject == 0) {
      ERS_DEBUG(2,"Empty input object, "<<paramIter->first);
      status = tools::WorstCaseAddStatus(status,inputResult->status_,weight);
      continue;
    }
        
    //This seems to be a good object, use it according to it's tags:
    bool isSpecialParameter = false;
    double andGroup = -99999.;
    TH1* binwiseStatHist = 0;
    for (std::map<std::string,double>::const_iterator tagIter=inputResult->tags_.begin();  
	 tagIter != inputResult->tags_.end(); ++tagIter ) {
    
      std::string tag = tagIter->first;
      size_t stringPos;
      std::string tagType = "ConfParameter--Role--Mask";
      if ( (stringPos = tag.find(tagType)) != std::string::npos) {
	if ( inputobject->IsA()->InheritsFrom("TH1") ) {
	  if( (((TH1*)inputobject)->GetNbinsX() == nBinsX) && (((TH1*)inputobject)->GetNbinsY() == nBinsY)) {
	    mask = (TH1*)inputobject;
	  }
	}
	isSpecialParameter = true;
	break;
      }
    	
      tagType = "Algorithm--BinsDiffByStrips"; //<-The only algorithm so far which produces a binwise status.
      if ( (stringPos = tag.find(tagType)) != std::string::npos) {
	if( inputobject->IsA()->InheritsFrom("TObjArray") ) {
	  binwiseStatHist = (TH1*) ((TObjArray*) inputobject)->First();
	  if( (binwiseStatHist->GetNbinsX() != nBinsX) || (binwiseStatHist->GetNbinsY() != nBinsY) ) {
	    binwiseStatHist = 0;
	  }
	  else {
	    isSpecialParameter = true;
	  }
	}
      }
      tagType = "AndGroup";
      if ( (stringPos = tag.find(tagType)) != std::string::npos) {
	andGroup = tagIter->second;
      }
    }
    if( !isSpecialParameter ){
      status = tools::WorstCaseAddStatus(status,inputResult->status_,weight);
    }
    else {
      if( andGroup == -99999. ) {
	if( binwiseStatHist != 0 ) {
	  std::pair<TH1*,float> binStatsPair ( binwiseStatHist, weight );
	  binwiseStatHists.push_back(binStatsPair);
	}
      }
      else {
	if( binwiseStatHist != 0 ) {
	  std::map<double,std::vector<std::pair<TH1*,float> > >::iterator fitr = binwiseAndGroup.find(andGroup);
	  if( fitr == binwiseAndGroup.end() ) {
	    //New and group, make it:
	    std::vector<std::pair<TH1*,float> > newAndGroup;
	    newAndGroup.push_back( std::make_pair( binwiseStatHist, weight ) );
	    binwiseAndGroup.insert(std::make_pair(andGroup,newAndGroup));
	    setOfAndGroups.insert(andGroup);
	  }
	  else {
	    fitr->second.push_back( std::make_pair( binwiseStatHist, weight ) );
	  }
	}
	else {
	  std::map<double,std::vector<std::pair<int,float> > >::iterator fitr = standardAndGroup.find(andGroup);
	  if( fitr == standardAndGroup.end() ) {
	    //New and group, make it:
	    std::vector<std::pair<int,float> > newAndGroup;
	    newAndGroup.push_back( std::make_pair( inputResult->status_, weight ) );
	    standardAndGroup.insert( std::make_pair(andGroup, newAndGroup) );
	    setOfAndGroups.insert(andGroup);
	  }
	  else {
	    fitr->second.push_back( std::make_pair( inputResult->status_, weight ) );
	  }
	}
      }
    }
  }

  //Loop over set of and groups, for those found with no binwise component do status calculation
  // here and remove from set.
  std::set<double>::iterator setItr = setOfAndGroups.begin();
  while (setItr != setOfAndGroups.end()) { // we cannot use a normal for-loop because we may erase the current element
    std::map<double, std::vector<std::pair<TH1*,float> > >::iterator findIter = binwiseAndGroup.find(*setItr);
    if( findIter == binwiseAndGroup.end() ) {
      std::map<double, std::vector<std::pair<int,float> > >::iterator fsIter = standardAndGroup.find(*setItr);
      if(  fsIter  != standardAndGroup.end() ) {
	if( !fsIter->second.empty() ) {
	  dqm_core::Result::Status groupStatus = dqm_core::Result::Red;
	  float groupWeight = 0;
	  for( std::vector<std::pair<int,float> >::const_iterator sandItr = fsIter->second.begin();
	       sandItr != fsIter->second.end(); ++sandItr ) {
	    groupStatus = tools::BestCaseAddStatus( groupStatus, (dqm_core::Result::Status) sandItr->first, sandItr->second );
	    groupWeight += sandItr->second;
	  }
	  status = tools::WorstCaseAddStatus(status,groupStatus,groupWeight);
	}
	standardAndGroup.erase(fsIter);
      }
      setOfAndGroups.erase(setItr++); // the erased iterator will become invalid, but post-incrementing is safe
    } else {
      ++setItr;
    }
  }
  if( (binwiseStatHists.size() == 0) && (setOfAndGroups.size() == 0) ) {
    newresult->status_ = status;
    return newresult;
  }

  std::vector<std::pair<TH1*,float> >::const_iterator itr;
  std::vector<std::pair<TH1*,float> >::const_iterator iBegin = binwiseStatHists.begin();
  std::vector<std::pair<TH1*,float> >::const_iterator iEnd   = binwiseStatHists.end();

  if( mask != 0 ){
    for(int ix = 1; ix <= nBinsX; ix++) {
      for(int iy = 1; iy <= nBinsY; iy++) {
	int bin = firstBinwiseHist->GetBin(ix,iy);
	//If there is an entry in the mask histogram, we will downweight this bin:
	double downWeight = 0;	
	if(mask->GetBinContent(bin) != 0) {
	  downWeight = 1;
	}
	for( itr = iBegin; itr != iEnd; ++itr ) {
	  status = tools::WorstCaseAddStatus( status,(dqm_core::Result::Status) floor(itr->first->GetBinContent(bin)), 
				     (itr->second - downWeight) );
	}
	//Iterate over addGroups:
	for( std::map<double, std::vector<std::pair<TH1*,float> > >::iterator bItr = binwiseAndGroup.begin();
	     bItr != binwiseAndGroup.end(); ++ bItr ) {
	  dqm_core::Result::Status groupStatus = dqm_core::Result::Red;
	  float groupWeight = 0;
	  for( std::vector<std::pair<TH1*,float> >::const_iterator bandItr = bItr->second.begin();
	       bandItr != bItr->second.end(); ++bandItr ) {
	    groupStatus = tools::BestCaseAddStatus( groupStatus,(dqm_core::Result::Status) floor(bandItr->first->GetBinContent(bin)), 
						    (bandItr->second - downWeight) );
	    groupWeight += bandItr->second - downWeight;
	  }
	  std::map<double, std::vector<std::pair<int,float> > >::iterator fsIter = standardAndGroup.find(bItr->first);
	  if(  fsIter  != standardAndGroup.end() ) {
	    for( std::vector<std::pair<int,float> >::const_iterator sandItr = fsIter->second.begin();
		 sandItr != fsIter->second.end(); ++sandItr ) {
	      groupStatus = tools::BestCaseAddStatus( groupStatus, (dqm_core::Result::Status) sandItr->first, sandItr->second );
	      groupWeight += sandItr->second;
	    }
	  }
	  status = tools::WorstCaseAddStatus( status, groupStatus, groupWeight );

	}
      }
    }
  }
  else {
    for(int ix = 1; ix <= nBinsX; ix++) {
      for(int iy = 1; iy <= nBinsY; iy++) {
	int bin = 0;
	if( ! binwiseStatHists.empty() ) {
	  bin = binwiseStatHists.front().first->GetBin(ix,iy);
	}
	else {
	  bin = binwiseAndGroup.begin()->second.front().first->GetBin(ix,iy); 
	}

	for( itr = iBegin; itr != iEnd; ++itr ) {
	  status = tools::WorstCaseAddStatus( status,(dqm_core::Result::Status) floor(itr->first->GetBinContent(bin)), 
					      itr->second );
	}

	//Iterate over addGroups:
	for( std::map<double, std::vector<std::pair<TH1*,float> > >::iterator bItr = binwiseAndGroup.begin();
	     bItr != binwiseAndGroup.end(); ++ bItr ) {

	  dqm_core::Result::Status groupStatus = dqm_core::Result::Red;
	  float groupWeight = 0;
	  for( std::vector<std::pair<TH1*,float> >::const_iterator bandItr = bItr->second.begin();
	       bandItr != bItr->second.end(); ++bandItr ) {
	    groupStatus = tools::BestCaseAddStatus( groupStatus,(dqm_core::Result::Status) floor(bandItr->first->GetBinContent(bin)), 
						    bandItr->second );
	    groupWeight += bandItr->second;
	  }
	  std::map<double, std::vector<std::pair<int,float> > >::iterator fsIter = standardAndGroup.find(bItr->first);
	  if(  fsIter  != standardAndGroup.end() ) {
	    for( std::vector<std::pair<int,float> >::const_iterator sandItr = fsIter->second.begin();
		 sandItr != fsIter->second.end(); ++sandItr ) {
	      groupStatus = tools::BestCaseAddStatus( groupStatus, (dqm_core::Result::Status) sandItr->first,sandItr->second );
	      groupWeight += sandItr->second;
	    }
	  }
	  status = tools::WorstCaseAddStatus( status, groupStatus, groupWeight );
	}
      }
    }
  }
  newresult->status_ = status;
  return newresult;
}

#endif // #ifndef DQM_ALGORITHMS_SUMMARY_DIVIDEBYHIST_CXX
