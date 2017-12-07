/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "UserDataSvc.h" 
#include "AthContainers/UserDataStore.h"
#include "AthenaKernel/errorcheck.h"
#include "Navigation/AthenaBarCodeBase.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h" 

UserDataSvc::UserDataSvc(const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name,pSvcLocator),
  m_storeGateSvc("StoreGateSvc",name),
  m_userData(nullptr),
  m_eventABC(IAthenaBarCode::UNDEFINEDBARCODE){

  declareProperty("StoreGateSvc",m_storeGateSvc);
  declareProperty("Key",m_key="UserDataStore","StoreGate key of UserDataStore object");
}

UserDataSvc::~UserDataSvc() {}

StatusCode UserDataSvc::initialize() {

  if (m_storeGateSvc.retrieve().isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve StoreGateSvc" << endmsg;
    return StatusCode::FAILURE;
  }


  IIncidentSvc* incSvc = nullptr;
  if (service("IncidentSvc",incSvc).isFailure()) {
    msg(MSG::ERROR) << "Unable to get the IncidentSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  incSvc->addListener( this, IncidentType::BeginEvent  );
  ATH_MSG_DEBUG("Added incidentLister for BeginEvent" );

  return StatusCode::SUCCESS;
}


StatusCode UserDataSvc::finalize() {
  return StatusCode::SUCCESS;
}


void UserDataSvc::handle( const Incident& inc ) {
  ATH_MSG_DEBUG ("Incident handle. Incident type " << inc.type() << " from " << inc.source());
  m_userData=nullptr;
  getUserDataStore();
  return;
}


void UserDataSvc::getUserDataStore() {
  if (!m_storeGateSvc->contains<UserDataStore>(m_key)) {
    ATH_MSG_DEBUG("Create and record UserDataStore object with key " << m_key);
    m_userData=new UserDataStore;
    StatusCode sc=m_storeGateSvc->record(m_userData,m_key);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to record UserDataStore with key " << m_key << endmsg;
      delete m_userData;
      m_userData=nullptr;
      return;
    }
    ATH_MSG_DEBUG("Successfully created UserDataStore Object with key " << m_key);
    AthenaBarCodeBase newEventABC; //Generate new AthenaBarCode
    m_userData->setEventABC(newEventABC.getAthenaBarCode());
    m_eventABC.setAthenaBarCode(newEventABC.getAthenaBarCode());
    ATH_MSG_DEBUG("Event BarCode: " << newEventABC.getAthenaBarCode()); 
  }//end if UserDataStore exists
  else {
     ATH_MSG_DEBUG("Loading existing UserDataStore Object with key " << m_key);
     const UserDataStore* cUDS;
     StatusCode sc=m_storeGateSvc->retrieve(cUDS,m_key);
     if (sc.isFailure()) {
       msg(MSG::ERROR) << "Failed to retrieve UserDataStore with key " << m_key << endmsg;
       m_userData=nullptr;
       return;
     }
     //Now cast away const'ness. This is necessary b/c data objects read from an input file 
     //are always const in Athena. For user data this policy does not make sense.
     m_userData=const_cast<UserDataStore*>(cUDS);
     m_eventABC.setAthenaBarCode(m_userData->getEventABC());
  }
  return;
}


StatusCode UserDataSvc::recordAny(const IAthenaBarCode& abc,const std::string& label, const boost::any& value) {
  ATH_MSG_DEBUG("Trying to record UserData with label " << label);
  if (!m_userData) return StatusCode::FAILURE;
  return m_userData->recordAny(abc,label,value);
}


  
StatusCode UserDataSvc::retrieveAny(const IAthenaBarCode& abc,const std::string& label, const boost::any*& value)  {
  ATH_MSG_DEBUG("Trying to retrieve UserData with label " << label);
  if (!m_userData) return StatusCode::FAILURE;
  return m_userData->retrieveAny(abc,label,value);
}


bool UserDataSvc::contains(const IAthenaBarCode& abc,const std::string& label) {
  ATH_MSG_DEBUG("Check existance of UserData with label " << label);
  if (!m_userData) return false;
  return m_userData->contains(abc,label);
}
 
 

StatusCode UserDataSvc::recordAny(const std::string& label, const boost::any& value) {
  ATH_MSG_DEBUG("Trying to record Event UserData with label " << label);
  return recordAny(m_eventABC,label,value);
}

  
StatusCode UserDataSvc::retrieveAny(const std::string& label, const boost::any*& value) {
  ATH_MSG_DEBUG("Trying to retrieve Event UserData with label " << label);
  return retrieveAny(m_eventABC,label,value);
}


bool UserDataSvc::contains(const std::string& label) {
  ATH_MSG_DEBUG("Check existance of Event UserData with label " << label);
  return contains(m_eventABC,label);
}

StatusCode UserDataSvc::typeIDtoAny(const std::type_info &decoinfo, void* & deco, boost::any& out) {

  if (this->AnyFromType<uint32_t>(decoinfo,deco,out)) 
    return StatusCode::SUCCESS;
  else if (this->AnyFromType<int32_t>(decoinfo,deco,out))
    return StatusCode::SUCCESS;
  else if (this->AnyFromType<float>(decoinfo,deco,out))
    return StatusCode::SUCCESS;
  else if (this->AnyFromType<double>(decoinfo,deco,out))
    return StatusCode::SUCCESS;
  else if (this->AnyFromType<std::vector<uint32_t> >(decoinfo,deco,out)) 
    return StatusCode::SUCCESS;
  else if (this->AnyFromType<std::vector<int32_t> >(decoinfo,deco,out))
    return StatusCode::SUCCESS;
  else if (this->AnyFromType<std::vector<float> >(decoinfo,deco,out))
    return StatusCode::SUCCESS;
  else if (this->AnyFromType<std::vector<double> >(decoinfo,deco,out))
    return StatusCode::SUCCESS;
  else {
    msg(MSG::ERROR) << "Unsupported type " << decoinfo.name() << endmsg;
    return StatusCode::FAILURE;
  }
}
  

int UserDataSvc::vdecorateElement(const IAthenaBarCode &abc, const std::string& label,
					 const std::type_info &decoinfo, void* & deco) {
  boost::any value;
  CHECK(this->typeIDtoAny(decoinfo,deco,value), -1);
  if (this->recordAny(abc,label,value).isSuccess()) 
    return 0;
  else
    return -1;
}


int UserDataSvc::vgetElementDecoration(const IAthenaBarCode &abc, const std::string& label,
					      const std::type_info &decoinfo, void *&deco) {
  
  const boost::any* value=nullptr;
  if (this->retrieveAny(abc,label,value).isFailure()) {
    return -1;
  }
  if (value->type()!=decoinfo) {
    msg(MSG::ERROR) << "Typeid mismatch! " <<  value->type().name() << " vs " << decoinfo.name() << endmsg;
    return -1;
  }
 
  if (decoinfo==typeid(uint32_t)) {
    deco=(void*)boost::any_cast<uint32_t>(value);
  }
  else if (decoinfo==typeid(int32_t)) {
    deco=(void*)boost::any_cast<int32_t>(value);
  }
  else if (decoinfo==typeid(float)) {
    deco=(void*)boost::any_cast<float>(value);
  }
  else if (decoinfo==typeid(double)) {
    deco=(void*)boost::any_cast<double>(value);
  }
  else if (decoinfo==typeid(std::vector<uint32_t>)) {
    deco=(void*)boost::any_cast<std::vector<uint32_t> >(value);
  }
  else if (decoinfo==typeid(std::vector<int32_t>)) {
    deco=(void*)boost::any_cast<std::vector<int32_t> >(value);
  }
  else if (decoinfo==typeid(std::vector<float>)) {
    deco=(void*)boost::any_cast<std::vector<float> >(value);
  }
  else if (decoinfo==typeid(std::vector<double>)) {
    deco=(void*)boost::any_cast<std::vector<double> >(value);
  }
  else if (decoinfo==typeid(bool)) {
    deco=(void*)boost::any_cast<bool>(value);
  }
  else {
    msg(MSG::ERROR) << "Unsupported/unknown type " << decoinfo.name() << ", label=" << label << endmsg;
    return -1;
  }

  return 0;
}


std::vector<const std::string*> UserDataSvc::getLabels(const IAthenaBarCode& obj) {
  if (!m_userData) return std::vector<const std::string*>();
  return m_userData->getLabels(obj.getAthenaBarCode());
}


std::vector<const std::string*> UserDataSvc::getLabels() {
  if (!m_userData) return std::vector<const std::string*>();
  return m_userData->getLabels(m_eventABC.getAthenaBarCode());
}
  

StatusCode
UserDataSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IUserDataSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = (IUserDataSvc*) this;
  }
  else {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
