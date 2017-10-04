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
  m_detStore ("DetectorStore", name),
  m_par_conddbfolder("/GLOBAL/DETSTATUS/LBSUMM"),
  m_par_detreqs(""),
  m_sgkey("DetectorStatus")
{
  // declare properties
  declareProperty("CondDBFolder",m_par_conddbfolder);
  declareProperty("StatusNames",m_par_detname);
  declareProperty("StatusReqs",m_par_detreq);
  declareProperty("StringReqs",m_par_detreqs);
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

  if (StatusCode::SUCCESS!=Service::initialize())
    ATH_MSG_ERROR("Service initialisation failed"  );

  ATH_MSG_DEBUG( "in initialize()"  );

  ATH_CHECK( m_detStore.retrieve() );

  // register callback if needed
  const DataHandle<CondAttrListCollection> cptr;
  if (StatusCode::SUCCESS==m_detStore->regFcn(&DetStatusSvc::update,this,
						cptr,m_par_conddbfolder)) {
    ATH_MSG_INFO( "Register callback to read detector status from CondDB folder "
                  << m_par_conddbfolder  );
  } else {
    ATH_MSG_FATAL( "Could not register callback to CondDB folder "
                   << m_par_conddbfolder  );
    return StatusCode::FAILURE;
  }
  // put (empty) DetStatusMap in TDS ready for callback to update
  DetStatusMap* statmap=new DetStatusMap();
  ATH_CHECK( m_detStore->record(statmap,m_sgkey) );

  // check status requirements - list from separate folders
  if (m_par_detname.size()!=m_par_detreq.size()) {
    ATH_MSG_INFO( 
      "Inconsistent setting of StatusNames and StatusReqs properities" );
    return StatusCode::FAILURE;
  }
  m_detname.clear();
  m_detreq.clear();
  for (unsigned int i=0;i<m_par_detname.size();++i) {
    m_detname.push_back(m_par_detname[i]);
    m_detreq.push_back(m_par_detreq[i]);
  }
  // add in list from string
  if (m_par_detreqs!="") {
    if (StatusCode::FAILURE==parseDetReqString()) return StatusCode::FAILURE;
  }
  if (m_detname.size()>0) {
    ATH_MSG_INFO( "Number of detector status requirements set: " <<
                  m_detname.size()  );
    for (unsigned int i=0;i<m_detname.size();++i)
      ATH_MSG_VERBOSE( m_detname[i] << ": >= " << m_detreq[i]  );
  }
  return StatusCode::SUCCESS;
}

StatusCode DetStatusSvc::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode DetStatusSvc::parseDetReqString() {
  // parse the detreq string into pairs of status flags and values
  size_t iofs0,iofs1,iofs2;
  iofs0=0;
  while (iofs0!=std::string::npos) {
    iofs1=m_par_detreqs.find(' ',iofs0);
    iofs2=std::string::npos;
    if (iofs1!=std::string::npos) {
      iofs2=m_par_detreqs.find(' ',iofs1+1);
      std::string nums=m_par_detreqs.substr(iofs1+1,1);
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
        ATH_MSG_ERROR( "Character " << *numv << " does not define status" );
        return StatusCode::FAILURE;
      }
      m_detname.push_back(m_par_detreqs.substr(iofs0,iofs1-iofs0));
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
  if (msgLvl(MSG::DEBUG)) {
    msg() << MSG::DEBUG << "Update callback invoked for keys: ";
    for (std::list<std::string>::const_iterator itr=keys.begin();
         itr!=keys.end(); ++itr) msg() << " " << *itr;
    msg() << endmsg;
  }
  // read the conditions object
  const CondAttrListCollection* cptr=0;
  if (StatusCode::SUCCESS==m_detStore->retrieve(cptr,m_par_conddbfolder) &&
      cptr!=0) {
    // find status object in TDS and reset it ready for new status info
    DetStatusMap* statmap=0;
    ATH_CHECK( m_detStore->retrieve(statmap,m_sgkey) );
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
	ATH_MSG_ERROR( 
	  "COOL data problem: no status name defined for channel " <<
	  chan  );
      }
    }
    ATH_MSG_DEBUG( "Updated DetStatusMap has size " << statmap->size() );
    statmap->toOutputStream(msg());
    msg() << endmsg;
  } else {
    ATH_MSG_ERROR( "Problem reading detector status from CondDB" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

const DetStatusMap* DetStatusSvc::getStatusMap() const {
  const DetStatusMap* statmap=0;
  // return DetStatusMap maintained in the TDS
  if (StatusCode::SUCCESS == m_detStore->retrieve(statmap,m_sgkey)) 
    return statmap;
  // No map found, return empty one
  static DetStatusMap empty;
  return &empty;
}

void DetStatusSvc::print() const {
  const DetStatusMap* statmap=getStatusMap();
  if (statmap!=0) {
    msg() << MSG::INFO;
    statmap->toOutputStream(msg());
  } else {
    ATH_MSG_ERROR( "Cannot find DetStatusMap to print"  );
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
	  ATH_MSG_DEBUG( "Event vetoed by status " << name << ", code "
                         << itr->second.code() << " below requirement of " << 
                         m_detreq[i]  );
	  veto=true;
	}
	break;
      }
    }
  }
  // check all requirements saw a status flag
  if (nseen<m_detname.size()) {
    ATH_MSG_DEBUG( "Only " << nseen << " of " << m_detname.size()
                   << " requirements saw status flags - event vetoed"  );
    veto=true;
  }
  return veto;
}
