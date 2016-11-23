/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ConditionsParameterTestAlg.cxx  Implementation file for SCT_ConditionsParameterTestAlg class.
 *  @author Shaun Roe, based on Jorgen Dalmau example
 **/

// Include SCT_ConditionsParameterTestAlg and Svc
#include "SCT_ConditionsParameterTestAlg.h"
#include "SCT_ConditionsServices/ISCT_ConditionsParameterSvc.h"
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"
#include "SCT_SimpleHisto.h"
#include "SCT_ConditionsUtilities.h"

// Include Event Info 
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Include Athena stuff
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

// Include Gaudi stuff
#include "GaudiKernel/StatusCode.h"

// Include STL stuff
#include <string>
//for o/p to file
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;
using namespace SCT_ConditionsServices;

SCT_ConditionsParameterTestAlg::SCT_ConditionsParameterTestAlg(const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),m_currentEvent(0),
    m_conditionsParameterSvc("SCT_ConditionsParameterSvc",name) //use SCT_ConditionsParameterSvc if you are not running with InDetRecExample
{ //nop
}
//
SCT_ConditionsParameterTestAlg::~SCT_ConditionsParameterTestAlg(){ }

//----------------------------------------------------------------------
StatusCode SCT_ConditionsParameterTestAlg::initialize(){  
  // Get the messaging service, print where you are
  msg(MSG::INFO) << "in initialize()" << endmsg;
  //
  StatusCode sc(StatusCode::SUCCESS);
  sc = m_conditionsParameterSvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) return (msg(MSG::ERROR) << "Unable to get the parameter conditions service" << endmsg), sc;
  
  return sc;
} // SCT_ConditionsParameterTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_ConditionsParameterTestAlg::execute(){
  //This method is only used to test the service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in execute()" << endmsg;
  //
  StatusCode sc(StatusCode::SUCCESS);
  
  // Get the current event
  sc = evtStore()->retrieve(m_currentEvent);
  if ( sc.isFailure() ) return (msg(MSG::ERROR) << "Could not get event info" << endmsg), sc;
  //
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Current Run.Event,Time: "
  << "[" << m_currentEvent->event_ID()->run_number()
  << "." << m_currentEvent->event_ID()->event_number()
  << "," << m_currentEvent->event_ID()->time_stamp()
  << "]" << endmsg;
  
  bool paramFilled(false);
  msg(MSG::INFO) << m_conditionsParameterSvc << endmsg;
  
  try{
      paramFilled =(m_conditionsParameterSvc->filled());
  }catch(...){
    msg(MSG::FATAL) << "Exception caught while trying to determine whether the data object was filled" << endmsg;
    return StatusCode::FAILURE;
  }
  
  try{
     float maxval =m_conditionsParameterSvc->max(AVG_THRESHOLD);
     float minval=m_conditionsParameterSvc->min(AVG_THRESHOLD);
     float avg=m_conditionsParameterSvc->avg(AVG_THRESHOLD);
     float sd=m_conditionsParameterSvc->sd(AVG_THRESHOLD);
     unsigned int n=m_conditionsParameterSvc->n(AVG_THRESHOLD);
     float thresh=m_conditionsParameterSvc->value(IdentifierHash(1760), AVG_THRESHOLD);
     msg(MSG::INFO) << "   value element 1760: "<<thresh<<endmsg;
     msg(MSG::INFO) << "        max threshold: "<<maxval<<endmsg;
     msg(MSG::INFO) << "        min threshold: "<<minval<<endmsg;
     msg(MSG::INFO) << "        avg threshold: "<<avg<<endmsg;
     msg(MSG::INFO) << "   standard deviation: "<<sd<<endmsg;
     msg(MSG::INFO) << "          no measured: "<<n<<endmsg;
  }catch(...){
    msg(MSG::FATAL) << "Exception caught while trying to access the thresholds" << endmsg;
    return StatusCode::FAILURE;
  }
  //simple histogram
  SCT_ConditionsServices::S_t histo;
  init(histo,0.0,8.0,100);
  std::vector<float> values;
  m_conditionsParameterSvc->getValues(values,AVG_THRESHOLD);
  for (std::vector<float>::const_iterator i=values.begin();i!=values.end();++i){
    fill(histo,*i); 
  }
  m_histoString=asXmlString(histo);
  cout<<m_histoString<<endl;
  if (not paramFilled) sc = StatusCode::FAILURE;
  return sc;
} // SCT_ConditionsParameterTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_ConditionsParameterTestAlg::finalize(){
  // Get the messaging service, print where you are
  msg(MSG::INFO) << "in finalize()" << endmsg;
  string testAreaPath=CoveritySafe::getenv("TestArea");
  string stylePath="./histo.xsl";
  string filename=testAreaPath+"/InnerDetector/InDetConditions/SCT_ConditionsServices/share/thresholds.xml";
  msg(MSG::INFO)<<"Filename: "<<filename<<endmsg;
  cout<<m_histoString<<endl;
  cout<<"------------"<<endl;
  ofstream opFile(filename.c_str(),ios::out);
  opFile<<xmlHeader();
  opFile<<stylesheet(stylePath);
  opFile<<m_histoString<<endl;
  return StatusCode::SUCCESS;
} // SCT_ConditionsParameterTestAlg::finalize()


