/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "ScoutingStreamWriter.h"
#include "TrigSteeringEvent/ScoutingInfo.h"


ScoutingStreamWriter::ScoutingStreamWriter(const std::string& name, ISvcLocator* pSvcLocator):
  //HLT::AllTEAlgo(name, pSvcLocator),
  HLT::HypoAlgo(name, pSvcLocator),
  m_clidSvc("ClassIDSvc", name){
  declareProperty("CollectionTypeName",  m_collectionTypeName);
}

ScoutingStreamWriter::~ScoutingStreamWriter(){
}

HLT::ErrorCode ScoutingStreamWriter::hltInitialize(){

  if (!m_clidSvc.retrieve().isSuccess()) {
    msg() << MSG::FATAL << "Cannot get ClassIDSvc " << m_clidSvc << endmsg;
    return HLT::FATAL;    
  }
  
  std::vector< std::string> coll = m_collectionTypeName.value();

  std::vector< std::string> types;
  std::vector<std::string> names;

  types.clear();
  names.clear();

  for (std::vector<std::string>::const_iterator it = coll.begin(), it_e = coll.end(); it != it_e; it++) {
    std::size_t found = (*it).find_first_of('#');
    
    if (found != std::string::npos)
      {
	types.push_back((*it).substr(0,found));
	names.push_back((*it).substr(found+1,(*it).size()-found+1));    
      }else{
      
      msg() << MSG::FATAL << "Collection type and collection name not configured correctly. " << endmsg;
      return HLT::FATAL; 
    }
  }
  
  if (types.size() != names.size()){
    msg() << MSG::FATAL << "Collection type and collection name vectors have different size. " << endmsg;
    return HLT::FATAL;    
  }   

  return HLT::OK;
  
}

HLT::ErrorCode ScoutingStreamWriter::hltExecute(const HLT::TriggerElement*, bool& pass){
						//std::vector<std::vector<HLT::TriggerElement*> >& input, 
						//unsigned int output){

      
  std::vector< std::string> coll = m_collectionTypeName.value();
  
  std::vector< std::string> types;
  std::vector<std::string> names;

  types.clear();
  names.clear();
  
  for (std::vector<std::string>::const_iterator it = coll.begin(), it_e = coll.end(); it != it_e; it++) {
    std::size_t found = (*it).find_first_of('#');
    if (found != std::string::npos)
      {
	types.push_back((*it).substr(0,found));
	names.push_back((*it).substr(found+1,(*it).size()-found+1));    
      }else{
      
      msg() << MSG::FATAL << "Collection type and collection name not configured correctly. " << endmsg;
      return HLT::FATAL; 
    }
  }
  
  if (types.size() != names.size()){
    msg() << MSG::FATAL << "Collection type and collection name vectors have different size. " << endmsg;
    return HLT::FATAL;    
  }   
  
  std::vector<CLID> clid;
  clid.clear();
  
  for (std::vector<std::string>::const_iterator it = types.begin(), it_e = types.end(); it != it_e; it++) {
    CLID classid;
    if (!m_clidSvc->getIDOfTypeName(*it, classid).isSuccess()){
      msg() << MSG::WARNING << "Cannot convert type. No CLID for " << (*it) << endmsg;
    } else {
      clid.push_back(classid);
    }
  }
  
  ScoutingInfo* ScoutInfo = config()->getScoutingInfo();
  if(!ScoutInfo){
    msg() << MSG::DEBUG << "*** Not Executing this ScoutingStreamWriter " << name() << endmsg;
    return HLT::OK;
  } else {
    for(unsigned int i = 0; i < clid.size(); ++i)
      ScoutInfo->add(clid.at(i), names.at(i));
  }
  
  pass = true;
  
  return HLT::OK;
  
}

HLT::ErrorCode ScoutingStreamWriter::hltFinalize(){
  
  return HLT::OK;

}
