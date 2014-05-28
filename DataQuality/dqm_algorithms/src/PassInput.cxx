/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file PassInput.cxx takes the input and returns it in dqm_core::Result. This is intended to act as a way of getting histograms
 * and configuration information to a summary maker, in order to perform multi-input analysis.
 * \author Evan Wulf
 */

#ifndef DQM_ALGORITHMS_PASSINPUT_CXX
#define DQM_ALGORITHMS_PASSINPUT_CXX

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/PassInput.h>

#include <TClass.h>
#include <map>

#include <dqm_core/AlgorithmManager.h>

#ifndef DQM_ALGORITHMS_MULTIALGORITHMTEST
static dqm_algorithms::PassInput myInstance;
#endif //

dqm_algorithms::PassInput::PassInput( )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("PassInput", this);
}

dqm_algorithms::PassInput::~PassInput()
{
}

dqm_algorithms::PassInput * 
dqm_algorithms::PassInput::clone()
{
  
  return new PassInput();
}


dqm_core::Result *
dqm_algorithms::PassInput::execute(const std::string &  name, 
				   const TObject& object, 
				   const dqm_core::AlgorithmConfig& config )
{ 
  dqm_core::Result* result = new dqm_core::Result();
  std::string tagName;

  if( (!object.IsA()->InheritsFrom( "TH1" )) && (!object.IsA()->InheritsFrom( "TObjArray" )) ) {
    throw dqm_core::BadConfig( ERS_HERE, name, "input object does not inherit from either TH1 or TObjArray");
  }

  //Pass this input object pointer
  TObject* resultObject = object.Clone();
  result->object_ =  (std::auto_ptr<TObject>)resultObject;

  //Pass the config as a series of tags (with labels embedded in the strings):
  //First the options:
  std::map<std::string,double> configParametersMap = config.getParameters(); //Copy needed to avoid runtime errors. AlgConfig needs to return map by reference, which it does not do. 
  if (!configParametersMap.empty()){
    for ( std::map<std::string,double>::const_iterator paramIter = configParametersMap.begin();
	  paramIter !=  configParametersMap.end(); ++paramIter ) {
      // if( paramIter->second.empty() ) {
      // 	//Could just drop this, or throw an error, but will keep and label as empty for now
      // 	tagName="EmptyParameter--" + paramIter->first;
      // 	result->tags_[tagName] = -1.;
      // 	continue;
      // }
      // if ( paramIter->second.size() == 1 ) {
	tagName="ConfParameter--" + paramIter->first;
	result->tags_[tagName] = paramIter->second;
      // }
      // else {
      // 	int i=0;
      // 	for( std::vector<double>::const_iterator valueIter = paramIter->second.begin();
      // 	     valueIter != paramIter->second.end(); valueIter++ ) {
      // 	  char numStr[32];
      // 	  sprintf(numStr,"%d",i);
      // 	  tagName="ConfParameter["+(numStr+("]--"+ paramIter->first));
      // 	  result->tags_[tagName] = *valueIter;
      // 	}
      // }
    } 
  }
  //Now the Green Thresholds:
  std::map<std::string,double> greenThreshMap = config.getGreenThresholds(); //Copy needed to avoid runtime errors. 
  if (!greenThreshMap.empty()) {
    for ( std::map<std::string,double>::const_iterator itr = greenThreshMap.begin(); 
 	  itr != greenThreshMap.end(); ++itr ){
 	tagName = "GThreshold--"+itr->first;
	result->tags_[tagName] = itr->second;
    }
  }
  //Finally, the Red Thresholds:
  std::map<std::string,double> redThreshMap = config.getRedThresholds(); //Copy needed to avoid runtime errors.
  if (!redThreshMap.empty()) {
    for ( std::map<std::string,double>::const_iterator itr = redThreshMap.begin(); 
 	  itr != redThreshMap.end(); ++itr ){
 	tagName = "RThreshold--"+itr->first;
	result->tags_[tagName] = itr->second;
    }
  }

  //No checks are done, so the result should be undefined:
  result->status_ = dqm_core::Result::Undefined;

  return result;
}

void
dqm_algorithms::PassInput::printDescription(std::ostream& out)
{
  out<<"PassInput: Minimal algorithm to pass its input histogram through to its result." << std::endl;

}

#endif  // #ifndef DQM_ALGORITHMS_PASSINPUT_CXX
