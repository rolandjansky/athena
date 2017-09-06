/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkTop/BoostedHadTopAndTopPairFilterAugmentation.h"

#include "xAODEventInfo/EventInfo.h"
#include "DerivationFrameworkTop/BoostedHadTopAndTopPairFilterTool.h"

namespace DerivationFramework {


BoostedHadTopAndTopPairFilterAugmentation::BoostedHadTopAndTopPairFilterAugmentation(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p),
  m_filterTool_High(""),
  m_filterTool_Low("")
{

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("EventInfoName",m_eventInfoName="EventInfo");
    declareProperty("FilterTool_High", m_filterTool_High);
    declareProperty("FilterTool_Low",  m_filterTool_Low);


}



BoostedHadTopAndTopPairFilterAugmentation::~BoostedHadTopAndTopPairFilterAugmentation(){}



StatusCode BoostedHadTopAndTopPairFilterAugmentation::initialize(){

  ATH_MSG_INFO("Initialize " );


  if(m_filterTool_High.retrieve().isFailure()){
    ATH_MSG_ERROR("unable to retrieve filter tool " << m_filterTool_High);
    return StatusCode::FAILURE;
  }

  if(m_filterTool_Low.retrieve().isFailure()){
    ATH_MSG_ERROR("unable to retrieve filter tool " << m_filterTool_Low);
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

  // call first the tool for high pT values
  static SG::AuxElement::Decorator<int> decorationttbarSysPt_High("TTbar350");
  static SG::AuxElement::Decorator<int> decorationHadTopPt_High("HadTop500");
  
  int filterCode_High = m_filterTool_High->filterFlag(500000.0, 350000.0);
  
  if (filterCode_High == 0 ){
    decorationHadTopPt_High(*eventInfo) = 0;
    decorationttbarSysPt_High(*eventInfo) = 0;
  }
  else if ( filterCode_High == 1){
    decorationttbarSysPt_High(*eventInfo) = 1;
    decorationHadTopPt_High(*eventInfo) = 0;
  }
  else if ( filterCode_High == 2){
    decorationttbarSysPt_High(*eventInfo) = 0;
    decorationHadTopPt_High(*eventInfo) = 1;
    
  }
  else if ( filterCode_High == 3){
    decorationttbarSysPt_High(*eventInfo) = 1;
    decorationHadTopPt_High(*eventInfo) = 1;  
  }
  

  // now call tool for low pT values                                                                                                                                                                                             
  static SG::AuxElement::Decorator<int> decorationttbarSysPt_Low("TTbar150");
  static SG::AuxElement::Decorator<int> decorationHadTopPt_Low("HadTop200");

  int filterCode_Low = m_filterTool_Low->filterFlag(200000.0, 150000.0);

  if (filterCode_Low == 0 ){
    decorationHadTopPt_Low(*eventInfo) = 0;
    decorationttbarSysPt_Low(*eventInfo) = 0;
  }
  else if ( filterCode_Low == 1){
    decorationttbarSysPt_Low(*eventInfo) = 1;
    decorationHadTopPt_Low(*eventInfo) = 0;
  }
  else if ( filterCode_Low == 2){
    decorationttbarSysPt_Low(*eventInfo) = 0;
    decorationHadTopPt_Low(*eventInfo) = 1;

  }
  else if ( filterCode_Low == 3){
    decorationttbarSysPt_Low(*eventInfo) = 1;
    decorationHadTopPt_Low(*eventInfo) = 1;
  }



  //ATH_MSG_INFO("filterCode "<<filterCode );


 return StatusCode::SUCCESS;

}



} /// namespace
