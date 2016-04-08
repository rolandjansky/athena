/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkTop/BoostedHadTopAndTopPairFilterAugmentation.h"

#include "xAODEventInfo/EventInfo.h"
#include "DerivationFrameworkTop/BoostedHadTopAndTopPairFilterTool.h"

namespace DerivationFramework {


BoostedHadTopAndTopPairFilterAugmentation::BoostedHadTopAndTopPairFilterAugmentation(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p),
  m_filterTool("")
{

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("EventInfoName",m_eventInfoName="EventInfo");
    declareProperty("FilterTool",m_filterTool);


}



BoostedHadTopAndTopPairFilterAugmentation::~BoostedHadTopAndTopPairFilterAugmentation(){}



StatusCode BoostedHadTopAndTopPairFilterAugmentation::initialize(){

  ATH_MSG_INFO("Initialize " );


  if(m_filterTool.retrieve().isFailure()){
    ATH_MSG_ERROR("unable to retrieve filter tool " <<m_filterTool);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}



StatusCode BoostedHadTopAndTopPairFilterAugmentation::finalize(){

  return StatusCode::SUCCESS;

}



StatusCode BoostedHadTopAndTopPairFilterAugmentation::addBranches() const{

  const xAOD::EventInfo* eventInfo;

  if (evtStore()->retrieve(eventInfo,m_eventInfoName).isFailure()) {
    ATH_MSG_ERROR("could not retrieve event info " <<m_eventInfoName);
    return StatusCode::FAILURE;
  }

  static SG::AuxElement::Decorator<int> decorationttbarSysPt("TTbar350");
  static SG::AuxElement::Decorator<int> decorationHadTopPt("HadTop200");
  
  int filterCode = m_filterTool->filterFlag();
  
  if (filterCode == 0 ){
    decorationHadTopPt(*eventInfo) = 0;
    decorationttbarSysPt(*eventInfo) = 0;
  }
  else if ( filterCode == 1){
    decorationttbarSysPt(*eventInfo) = 1;
    decorationHadTopPt(*eventInfo) = 0;
  }
  else if ( filterCode == 2){
    decorationttbarSysPt(*eventInfo) = 0;
    decorationHadTopPt(*eventInfo) = 1;
    
  }
  else if ( filterCode == 3){
    decorationttbarSysPt(*eventInfo) = 1;
    decorationHadTopPt(*eventInfo) = 1;  
  }
  

  //ATH_MSG_INFO("filterCode "<<filterCode );


 return StatusCode::SUCCESS;

}



} /// namespace
