/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTriggerTool/CaloTTOnOffIdMap.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/CaloID_Exception.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include <iostream>

CaloTTOnOffIdMap::CaloTTOnOffIdMap() {
}


CaloTTOnOffIdMap::~CaloTTOnOffIdMap() {
}

//------------------------------------------//
void CaloTTOnOffIdMap::set( const CaloTTOnOffId& m ) {

  convert_to_P(m);

  IMessageSvc *msgSvc;
  StatusCode status =Gaudi::svcLocator()->service("MessageSvc",msgSvc);
  if(status.isFailure()){
    std::cout <<  "Cannot locate MessageSvc" << std::endl;
  }
  MsgStream log( msgSvc, "CaloTTOnOffIdMap");
  bool dump=false;
  if (log.level()<=MSG::VERBOSE) dump=true;
  bool dump2=false;
  if (log.level()<=MSG::DEBUG) dump2=true;


  log<<MSG::DEBUG<<" CaloTTOnOffId size = "<<m.size() <<endreq;
  StoreGateSvc * detStore;
  status = Gaudi::svcLocator()->service("DetectorStore",detStore);
  if(status.isFailure()){
     log << MSG::ERROR <<  "Cannot locate DetectorStore" << endreq;
  }

  const TTOnlineID* online_id = nullptr;
  const CaloLVL1_ID* offline_id = nullptr;

  status=detStore->retrieve(online_id);
  if(status.isFailure()){
    log << MSG::ERROR <<  "Cannot retrieve online_id" << endreq;
  }
  status=detStore->retrieve(offline_id);
  if(status.isFailure()){
    log << MSG::ERROR <<  "Cannot retrieve offline_id" << endreq;
  }

  CaloTTOnOffId::const_iterator it  = m.begin();
  CaloTTOnOffId::const_iterator it_e  = m.end();

    try {
        for (; it!=it_e; ++it) {
            const CaloTTOnOffId_t& t = *it;

            Identifier id = offline_id->tower_id(t.pn, t.sampling, t.region, t.eta, t.phi);
            HWIdentifier sid = online_id->channelId(t.crate, t.module, t.submodule, t.channel);

            if (dump) {
              log<<MSG::VERBOSE
              << " db struct= "
              <<" pn="<<t.pn<<" sampling="<<t.sampling
              <<" region="<<t.region
              <<" eta="<<t.eta<<" phi="<<t.phi<<" layer="<<t.layer<<" | "
              <<" crate="<<t.crate<<" module="<<t.module
              <<" submodule="<<t.submodule
              <<" channel="<<t.channel
              << endreq;

              log<<MSG::VERBOSE<< " onl id = " << sid<<" offline id ="<<id<<endreq;
            }

            m_off2onIdMap[id] = sid;
            m_on2offIdMap[sid] = id;
        }
        if (dump2) log<<MSG::DEBUG<<" CaloTTOnOffIdMap::set : number of Ids="<<m_on2offIdMap.size()<<std::endl;
    } catch (CaloID_Exception& except) {
        log<<MSG::ERROR<<" Failed in CaloTTOnOffIdMap::set " << endreq;
        log<<MSG::ERROR<< (std::string) except  << endreq ;
    }
  return;
}

//--------------------------------------------------------------------------//
HWIdentifier CaloTTOnOffIdMap::createSignalChannelID(const Identifier& id, bool bQuiet) const {

    std::map<Identifier,HWIdentifier>::const_iterator it =m_off2onIdMap.find(id);

    if(it!=m_off2onIdMap.end()){
        return (*it).second;
    }

    if(bQuiet) {
    	return HWIdentifier(0);

    } else {

		IMessageSvc *msgSvc;
		StatusCode status = Gaudi::svcLocator()->service("MessageSvc",msgSvc);
		if(status.isFailure()){
			std::cout <<  "Cannot locate MessageSvc" << std::endl;
		}

		MsgStream log( msgSvc, "CaloTTOnOffIdMap");
		log<<MSG::ERROR<<" Offline ID not found "<< id <<endreq;

		return HWIdentifier(0);
	}
}


//---------------------------------------------------------------------//
Identifier CaloTTOnOffIdMap::cnvToIdentifier(const HWIdentifier & sid, bool bQuiet)const {

    std::map<HWIdentifier,Identifier>::const_iterator it=m_on2offIdMap.find(sid);

    if(it!=m_on2offIdMap.end()){
        return (*it).second;
    }

    if(bQuiet) {
    	return Identifier(0);

    } else {

		// ERROR, can not find the id.
		IMessageSvc *msgSvc;
		StatusCode status =Gaudi::svcLocator()->service("MessageSvc",msgSvc);
		if(status.isFailure()){
			std::cout <<  "Cannot locate MessageSvc" << std::endl;
		}

		MsgStream log( msgSvc, "CaloTTOnOffIdMap");
		log<<MSG::ERROR<<" Online ID not found, id = " <<sid.get_compact()<< endreq;

		return Identifier(0) ;
    }
}

CaloTTOnOffId_P* CaloTTOnOffIdMap::getP() {
        return &m_persData;
}

void CaloTTOnOffIdMap::convert_to_P( const CaloTTOnOffId& d ) {

    CaloTTOnOffId::const_iterator it = d.begin();
    CaloTTOnOffId::const_iterator it_e = d.end();

    for ( ;it!=it_e;++it){
        const CaloTTOnOffId_t& t = *it;
        CaloTTOnOffId_P::__t t2 ;

        t2.pn  = t.pn;
        t2.sampling = t.sampling;
        t2.region = t.region;
        t2.eta = t.eta;
        t2.phi = t.phi;
        t2.layer = t.layer;

        t2.crate = t.crate ;
        t2.module = t.module;
        t2.submodule= t.submodule;
        t2.channel = t.channel;

        m_persData.m_v.push_back(t2);
    }
}


void CaloTTOnOffIdMap::convert_to_D( const CaloTTOnOffId_P& p,  CaloTTOnOffId& d ) {

    std::vector<CaloTTOnOffId_P::__t>::const_iterator it = p.m_v.begin();
    std::vector<CaloTTOnOffId_P::__t>::const_iterator it_e = p.m_v.end();

    d.clear();
    for ( ;it!=it_e;++it){
        const CaloTTOnOffId_P::__t& t = *it;
        CaloTTOnOffId_t t2 ;

        t2.pn  = t.pn;
        t2.sampling = t.sampling;
        t2.region = t.region;
        t2.eta = t.eta;
        t2.phi = t.phi;
        t2.layer = t.layer;

        t2.crate = t.crate ;
        t2.module = t.module;
        t2.submodule= t.submodule;
        t2.channel = t.channel;

        d.push_back(t2);
    }
}

void CaloTTOnOffIdMap::set (const CaloTTOnOffId_P& p) {

    CaloTTOnOffId d;
    convert_to_D(p,d);
    set(d);
    m_persData.m_version = p.m_version;
    return;
}
