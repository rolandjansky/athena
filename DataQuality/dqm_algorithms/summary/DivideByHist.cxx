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
#include <dqm_algorithms/summary/DivideByHist.h>

#include <dqm_algorithms/tools/SimpleAlgorithmConfig.h>

#include <dqm_algorithms/Bins_Diff_FromAvg.h>

#ifndef DQM_ALGORITHMS_MULTIALGORITHMTEST
static dqm_algorithms::summary::DivideByHist myInstance;
#endif //#ifndef DQM_ALGORITHMS_MULTIALGORITHMTEST


dqm_algorithms::summary::DivideByHist::DivideByHist()
{
  dqm_core::AlgorithmManager::instance().registerSummaryMaker("DivideByHist",this);
}

dqm_algorithms::summary::DivideByHist::~DivideByHist()
{
}

dqm_algorithms::summary::DivideByHist*
dqm_algorithms::summary::DivideByHist::clone()
{
  return new DivideByHist();
}

dqm_core::Result*
dqm_algorithms::summary::DivideByHist::execute( const std::string & name, 
						const dqm_core::Result & lastResult, 
						const dqm_core::ParametersMap & map)
{
  //General summary maker infrastructure: (This can be resused)
  dqm_core::Result *newresult = new dqm_core::Result();

  std::multimap<std::string,TObject*> inputs;
  dqm_algorithms::tools::SimpleAlgorithmConfig algConfig;

  dqm_core::Result * overWriteResult = 0;

  //Loop over elements of the parameter map to extract histograms and there roles, if any:
  for (dqm_core::ParametersMap::const_iterator iter=map.begin();iter!=map.end();++iter){
    //If weight is 0 skip this result. Allow to "turn off" 
    // results in summary makers
    if ( iter->second->getWeight() == 0 ) {
      ERS_DEBUG(2,"Skip result (weight 0): "<<iter->first);
      continue;
    }
    TObject* inputobject = iter->second->getResult().get()->getObject();
    if (inputobject == 0) {
      //This is a sign that we just don't have all the parameters yet (summary makers are called every time a parameter comes in,
      // so when the first one comes, the second one isn't in yet, so we get a null pointer). 
      newresult->status_ = lastResult.status_;
      return newresult;
    }
    else {
      //This seems to be a good object, try to find it's role and store a pointer to it if we do:
      std::string role;
      for (std::map<std::string,double>::const_iterator tagIter=iter->second->getResult().get()->tags_.begin();  
	   tagIter != iter->second->getResult().get()->tags_.end(); ++tagIter ) {
	std::string tag = tagIter->first;
	size_t stringPos;
	std::string tagType = "ConfParameter--Role--";
	if ( (stringPos = tag.find(tagType)) != std::string::npos) {
	  stringPos += tagType.length();
	  role = tag.substr(stringPos);
	  break;
	}
      }
      //Only keep objects with a valid role:
      if(role.size() != 0) {
	inputs.insert(std::pair<std::string,TObject*>(role,inputobject));
      }
    }
  }

  if(inputs.size() < 2) {
    newresult->status_ = lastResult.status_;
    return newresult;
  }

  //Extract config information from result tags in the parameter map, only extract config info from the first result
  //found when iterating backwards which has nonzero tags.

  std::map<std::string,double > paramVecMap;
  for (dqm_core::ParametersMap::const_reverse_iterator rIter=map.rbegin() ; rIter!=map.rend();++rIter) {
    if( rIter->second->getResult().get()->tags_.empty() ){
      continue;
    }
    for (std::map<std::string,double>::const_iterator tagIter=rIter->second->getResult().get()->tags_.begin();  
	 tagIter != rIter->second->getResult().get()->tags_.end(); ++tagIter ) {
      std::string tag = tagIter->first;
      size_t stringPos;
      std::string tagType;

      tagType = "ConfParameter--Role--";
      if ( tag.find(tagType) != std::string::npos ){
	//Already dealt with,
	continue;
      }
      tagType = "ConfParameter--OverWrite";
      if ( tag.find(tagType) != std::string::npos ){
	//This parameter wants to be overwriten, remember this for later and continue:
	overWriteResult = rIter->second->getResult().get();
	//Disable updating of the parameter:
	// rIter->second->stateChanged(false,false); (uncomment once dqm_core-01-10-00 is available)
	continue;
      }
      tagType = "ConfParameter--";
      if ( (stringPos = tag.find(tagType)) != std::string::npos ){
	//This is a single member config parameter; add it:
	stringPos += tagType.length();
	algConfig.addParameter( tag.substr(stringPos),tagIter->second );
	continue;
      }
      tagType = "GThreshold--";
      if ( (stringPos = tag.find(tagType)) != std::string::npos ){
	//This is a green threshold; add it:
	stringPos += tagType.length();
	algConfig.addGreenThreshold( tag.substr(stringPos),tagIter->second );
	continue;
      }
      tagType = "RThreshold--";
      if ( (stringPos = tag.find(tagType)) != std::string::npos ){
	//This is a red threshold; add it:
	stringPos += tagType.length();
	algConfig.addRedThreshold( tag.substr(stringPos),tagIter->second );
	continue;
      }
      if ( tag.find("ConfParameter[") != std::string::npos ){
	std::cerr<< "MULTIOPTION PARAMETERS NO LONGER SUPPORTED; IGNORING" << std::endl;
	// //This is an element of a configuration parameter vector; we can't add it yet
	// stringPos = tag.find("]--") + 3;
	// paramVecMap[tag.substr(stringPos)].push_back(tagIter->second);
      }
    }//end loop over result tags
    break; //Assume that the first non-empty set of tags is all that is needed, so continue without looking at more parameters.
  }//end loop over parameters map

  //Now loop over the paramVecMap to finish the algConfig
  for ( std::map<std::string,double>::const_iterator pvIter=paramVecMap.begin(); pvIter!=paramVecMap.end();pvIter++){
    algConfig.addParameter( pvIter->first, pvIter->second );
  }

  delete newresult;
  newresult = execute(name,inputs,algConfig);

  // Note: this is experimental and may not work:
  if ((overWriteResult != 0) && (newresult->getObject() != 0) ) {

    //A parameter wanted to have it's result overwritten with the results of this summary maker, let us do it:
    overWriteResult->status_ = newresult->status_;
    overWriteResult->tags_ = newresult->tags_;

    TObject* overWriteObject = overWriteResult->getObject();
    if ( (!overWriteObject->IsA()->InheritsFrom("TH1")) || (!newresult->getObject()->IsA()->InheritsFrom("TH1")) ) {
      //Do nothing: overwriting a non TH1 object might be a bad idea.
    }
    else {

      TH1* hOverW = (TH1*) overWriteObject;
      TH1* hResult = (TH1*) newresult->getObject();

      if (hOverW->GetDimension() == hResult->GetDimension()) {
	hOverW->Reset();
	hOverW->Add(hResult);
      }
    }
  }

  return newresult;
}

dqm_core::Result*
dqm_algorithms::summary::DivideByHist::execute(const std::string & name,
					       const std::multimap<std::string,TObject*> & inputs, 
					       const dqm_core::AlgorithmConfig& config) 
{
  //  dqm_core::Result *newresult = new dqm_core::Result();
  dqm_core::Result *newresult(0);

  TH1 * hNumerator = 0;
  TH1 * hDenominator = 0;
  TH1 * hQuotient = 0;

  TObject * numero = 0;
  TObject * denomino = 0;

  if( inputs.size() < 2 ){
    throw dqm_core::BadConfig( ERS_HERE, name ,"incorrect number of arguements, number of inputs for division must be at least two");
  }

  //Retrieve the Histograms to be divided, using their specified roles:
  std::multimap<std::string,TObject*>::const_iterator mmItr;

  mmItr = inputs.find("Numerator");
  if ( mmItr != inputs.end() ) {
    numero = mmItr->second;
  }
  else {
    throw dqm_core::BadConfig( ERS_HERE, name, "no object with 'Role--Numerator' was specified.");
  }
  mmItr = inputs.find("Denominator");
  if ( mmItr != inputs.end() ) {
    denomino = mmItr->second;
  }
  else {
    throw dqm_core::BadConfig( ERS_HERE, name, "no object with 'Role--Denominator' was specified.");
  }

  if( numero->IsA()->InheritsFrom("TH1") ){
    hNumerator = (TH1*) numero;
  }
  else {
    char errorStr[64];
    sprintf(errorStr,"%s does not inherit from TH1",numero->GetName());
    throw dqm_core::BadConfig( ERS_HERE, name, errorStr);
  }
  if( denomino->IsA()->InheritsFrom("TH1") ){
    hDenominator = (TH1*) denomino;
  }
  else {
    char errorStr[64];
    sprintf(errorStr,"%s does not inherit from TH1",denomino->GetName());
    throw dqm_core::BadConfig( ERS_HERE, name, errorStr);
  }

  //Perform the division:
  hQuotient = dqm_algorithms::tools::DivideByHistogram(hNumerator,hDenominator);

  //Modify the resultant histogram if requested by config:
  dqm_algorithms::tools::ModifyHistogram(hQuotient,config);

  //Extract the name of the algorithm to run:
  std::string algorithmName = dqm_algorithms::tools::ExtractAlgorithmName(config);

  //Run the algorithm:
  newresult = dqm_algorithms::tools::ExecuteNamedAlgorithm(algorithmName,*hQuotient,config);
  
  //Make the quotient histogram the output result object
  newresult->object_ = (std::auto_ptr<TObject>)(TObject*)hQuotient;

  return newresult;
}

#endif // #ifndef DQM_ALGORITHMS_SUMMARY_DIVIDEBYHIST_CXX
