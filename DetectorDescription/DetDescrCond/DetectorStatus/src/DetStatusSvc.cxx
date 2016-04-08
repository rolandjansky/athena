/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DetStatusSvc.cxx
// Richard Hawkings, started 13/12/06

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/DetStatusMap.h"
#include "DetStatusSvc.h"

DetStatusSvc::DetStatusSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name,svc),
  par_conddbfolder("/GLOBAL/DETSTATUS/LBSUMM"),
  par_detreqs(""),
  m_sgkey("DetectorStatus")
{
  // declare properties
  declareProperty("CondDBFolder",par_conddbfolder);
  declareProperty("StatusNames",par_detname);
  declareProperty("StatusReqs",par_detreq);
  declareProperty("StringReqs",par_detreqs);
}

DetStatusSvc::~DetStatusSvc() {}

const InterfaceID& DetStatusSvc::type() const
{ 
return IDetStatusSvc::interfaceID();
}

StatusCode DetStatusSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (IDetStatusSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(IDetStatusSvc*)this;
  } else {
    return Service::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode DetStatusSvc::initialize()
{
  // service initialisation - get parameters, setup default cache
  // and register for condDB callbacks if needed

  MsgStream log(msgSvc(), name());
  if (StatusCode::SUCCESS!=Service::initialize()) log << MSG::ERROR <<
	    "Service initialisation failed" << endreq;

  log << MSG::DEBUG << "in initialize()" << endreq;

  // get event store
  if (StatusCode::SUCCESS!=service("StoreGateSvc",p_evtstore)) {
    log << MSG::FATAL << "Detector store not found" << endreq; 
    return StatusCode::FAILURE;
  }
  // get detector store
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    log << MSG::FATAL << "Detector store not found" << endreq; 
    return StatusCode::FAILURE;
  }
  // register callback if needed
  const DataHandle<CondAttrListCollection> cptr;
  if (StatusCode::SUCCESS==p_detstore->regFcn(&DetStatusSvc::update,this,
						cptr,par_conddbfolder)) {
    log << MSG::INFO << 
    "Register callback to read detector status from CondDB folder "
	    << par_conddbfolder << endreq;
  } else {
    log << MSG::FATAL << "Could not register callback to CondDB folder "
          << par_conddbfolder << endreq;
    return StatusCode::FAILURE;
  }
  // put (empty) DetStatusMap in TDS ready for callback to update
  DetStatusMap* statmap=new DetStatusMap();
  if (StatusCode::SUCCESS!=p_detstore->record(statmap,m_sgkey)) {
    log << MSG::FATAL << "Could not record DetStatusMap at " << m_sgkey
        << " in TDS" << endreq;
    return StatusCode::FAILURE;
  }

  // check status requirements - list from separate folders
  if (par_detname.size()!=par_detreq.size()) {
    log << MSG::INFO << 
      "Inconsistent setting of StatusNames and StatusReqs properities" 
	<< endreq;
    return StatusCode::FAILURE;
  }
  m_detname.clear();
  m_detreq.clear();
  for (unsigned int i=0;i<par_detname.size();++i) {
    m_detname.push_back(par_detname[i]);
    m_detreq.push_back(par_detreq[i]);
  }
  // add in list from string
  if (par_detreqs!="") {
    if (StatusCode::FAILURE==parseDetReqString()) return StatusCode::FAILURE;
  }
  if (m_detname.size()>0) {
    log << MSG::INFO << "Number of detector status requirements set: " <<
      m_detname.size() << endreq;
    for (unsigned int i=0;i<m_detname.size();++i)
      log << m_detname[i] << ": >= " << m_detreq[i] << endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode DetStatusSvc::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode DetStatusSvc::parseDetReqString() {
  // parse the detreq string into pairs of status flags and values
  MsgStream log(msgSvc(), name());
  size_t iofs0,iofs1,iofs2;
  iofs0=0;
  while (iofs0!=std::string::npos) {
    iofs1=par_detreqs.find(' ',iofs0);
    iofs2=std::string::npos;
    if (iofs1!=std::string::npos) {
      iofs2=par_detreqs.find(' ',iofs1+1);
      std::string nums=par_detreqs.substr(iofs1+1,1);
      const char* numv=nums.c_str();
      int status=0;
      if (isdigit(*numv)) {
        status=atoi(numv);
      } else if (nums=="B" || nums=="b") {
        status=-1;
      } else if (nums=="U" || nums=="u") {
        status=0;
      } else if (nums=="R" || nums=="r") {
        status=1;
      } else if (nums=="Y" || nums=="y") {
        status=2;
      } else if (nums=="G" || nums=="g") {
        status=3;
      } else {
        log << MSG::ERROR << "Character " << *numv << " does not define status"
	  << endreq;
        return StatusCode::FAILURE;
      }
      m_detname.push_back(par_detreqs.substr(iofs0,iofs1-iofs0));
      m_detreq.push_back(status);
    }
    if (iofs2!=std::string::npos) {
      iofs0=iofs2+1;
    } else {
      iofs0=std::string::npos;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode DetStatusSvc::update( IOVSVC_CALLBACK_ARGS_P( /* I */ ,keys) ) {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Update callback invoked for keys: ";
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end(); ++itr) log << " " << *itr;
  log << endreq;
  // read the conditions object
  const CondAttrListCollection* cptr=0;
  if (StatusCode::SUCCESS==p_detstore->retrieve(cptr,par_conddbfolder) &&
      cptr!=0) {
    // find status object in TDS and reset it ready for new status info
    DetStatusMap* statmap=0;
    if (StatusCode::SUCCESS!=p_detstore->retrieve(statmap,m_sgkey)) {
      log << MSG::ERROR << "Cannot find DetStatus map to update" << endreq;
      return StatusCode::FAILURE;
    }
    statmap->clear();
    // loop over all the elements in the collection
    for (CondAttrListCollection::const_iterator citr=cptr->begin();
	 citr!=cptr->end();++citr) {
      unsigned int chan=citr->first;
      const coral::AttributeList& alist=citr->second;
      // find the string name for this channel
      const std::string name=cptr->chanName(chan);
      if (name!="") {
        statmap->add(name,DetStatus(alist["Code"].data<int>(),
				  alist["deadFrac"].data<float>(),
				  alist["Thrust"].data<float>()));
      } else {
	log << MSG::ERROR << 
	  "COOL data problem: no status name defined for channel " <<
	  chan << endreq;
      }
    }
    log << MSG::DEBUG << "Updated DetStatusMap has size " << statmap->size()
	<< endreq;
    statmap->toOutputStream(log);
    log << endreq;
  } else {
    log << MSG::ERROR << "Problem reading detector status from CondDB"
	  << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

const DetStatusMap* DetStatusSvc::getStatusMap() const {
  const DetStatusMap* statmap=0;
  // return DetStatusMap maintained in the TDS
  if (StatusCode::SUCCESS == p_detstore->retrieve(statmap,m_sgkey)) 
    return statmap;
  // No map found, return empty one
  static DetStatusMap empty;
  return &empty;
}

void DetStatusSvc::print() const {
  MsgStream log(msgSvc(), name());
  const DetStatusMap* statmap=getStatusMap();
  if (statmap!=0) {
    log << MSG::INFO;
    statmap->toOutputStream(log);
  } else {
    log << MSG::ERROR << "Cannot find DetStatusMap to print" << endreq;
  }
}

const DetStatus* DetStatusSvc::findStatus(const std::string& name) 
  const {
  // return a pointer to the DetStatus object for a given name
  // or null pointer if nothing found
  const DetStatusMap* statmap=getStatusMap();
  if (statmap!=0) {
    DetStatusMap::const_iterator itr=statmap->find(name);
    if (itr!=statmap->end()) return &(itr->second);
  }
  return 0;
}

void DetStatusSvc::getIter(DetStatusMap::const_iterator& begin,
			   DetStatusMap::const_iterator& end) const {
  const DetStatusMap* statmap=getStatusMap();
  begin=statmap->begin();
  end=statmap->end();
}

bool DetStatusSvc::vetoed() const {
  MsgStream log(msgSvc(), name());
  bool veto=false;
  DetStatusMap::const_iterator begin,end;
  getIter(begin,end);
  // for each detector status, check if there is a requirement on it
  // also check that all requirements were satisfied in case there
  // is no status flag seen corresponding to a requirement
  // this will not catch all status flags satisfying a wildcard requirement
  std::vector<int> usedreq(m_detname.size());
  for (unsigned int i=0;i<m_detname.size();++i) usedreq[i]=0;
  unsigned int nseen=0;
  for (DetStatusMap::const_iterator itr=begin;itr!=end;++itr) {
    const std::string& name=itr->first;
    for (unsigned int i=0;i<m_detname.size();++i) {
      if (m_detname[i]==name.substr(0,m_detname[i].size())) {
	if (usedreq[i]==0) ++nseen;
	++usedreq[i];
	if (itr->second.code()<m_detreq[i]) {
	  log << MSG::DEBUG << "Event vetoed by status " << name << ", code "
	      << itr->second.code() << " below requirement of " << 
	    m_detreq[i] << endreq;
	  veto=true;
	}
	break;
      }
    }
  }
  // check all requirements saw a status flag
  if (nseen<m_detname.size()) {
    log << MSG::DEBUG << "Only " << nseen << " of " << m_detname.size()
	<< " requirements saw status flags - event vetoed" << endreq;
    veto=true;
  }
  return veto;
}
