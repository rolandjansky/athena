/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTriggerTool/CaloTTOnAttrIdMap.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/CaloID_Exception.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include <iostream>

CaloTTOnAttrIdMap::CaloTTOnAttrIdMap() {
}


CaloTTOnAttrIdMap::~CaloTTOnAttrIdMap() {
}

//------------------------------------------//
void CaloTTOnAttrIdMap::set( const CaloTTOnAttrId& m ) {

  convert_to_P(m);

  IMessageSvc *msgSvc;
  StatusCode status =Gaudi::svcLocator()->service("MessageSvc",msgSvc);
  if(status.isFailure()){
    std::cout <<  "Cannot locate MessageSvc" << std::endl;
  }
  MsgStream log( msgSvc, "CaloTTOnAttrIdMap");
  bool dump=false;
  if (log.level()<=MSG::VERBOSE) dump=true;
  bool dump2=false;
  if (log.level()<=MSG::DEBUG) dump2=true;

  log<<MSG::DEBUG<<" CaloTTOnAttrId size = "<<m.size() <<endmsg;
  StoreGateSvc * detStore;
  status = Gaudi::svcLocator()->service("DetectorStore",detStore);
  if(status.isFailure()){
     log << MSG::ERROR <<  "Cannot locate DetectorStore" << endmsg;
  }

  const TTOnlineID* online_id = nullptr;

  status=detStore->retrieve(online_id);
  if(status.isFailure()){
    log << MSG::ERROR <<  "Cannot retrieve online_id" << endmsg;
  }

  CaloTTOnAttrId::const_iterator it  = m.begin();
  CaloTTOnAttrId::const_iterator it_e  = m.end();

    try {
        for (; it!=it_e; ++it) {
            const CaloTTOnAttrId_t& t = *it;

            HWIdentifier sid = online_id->channelId(t.crate, t.module, t.submodule, t.channel);

            if (dump) {
             log<<MSG::VERBOSE
             << " db struct= "
             <<" crate="<<t.crate<<" module="<<t.module
             <<" submodule="<<t.submodule
             <<" channel="<<t.channel
             <<" em_had="<<t.em_had
             <<" pos_neg="<<t.pos_neg
             <<" barrel_endcap_fcal"<<t.barrel_endcap_fcal
             << endmsg;

             log<<MSG::VERBOSE<< " onl id = " << sid <<endmsg;
            }

            AttrStruct attrStruct;
            attrStruct.em_had = t.em_had;
            attrStruct.pos_neg = t.pos_neg;
            attrStruct.barrel_endcap_fcal = t.barrel_endcap_fcal;
            attrStruct.module_type = t.module_type;

            m_on2attrIdMap[sid] = attrStruct;
        }
        if (dump2) log<<MSG::DEBUG<<" CaloTTOnAttrIdMap::set : number of Ids="<<m_on2attrIdMap.size()<<std::endl;
    } catch (CaloID_Exception& except) {
        log<<MSG::ERROR<<" Failed in CaloTTOnAttrIdMap::set " << endmsg;
        log<<MSG::ERROR<< (std::string) except  << endmsg ;
    }
  return;
}

unsigned int CaloTTOnAttrIdMap::em_had( HWIdentifier channelId ) const {
    std::map<HWIdentifier,AttrStruct>::const_iterator it=m_on2attrIdMap.find(channelId);

    if(it!=m_on2attrIdMap.end()){
        return ((*it).second).em_had;
    }

    // ERROR, can not find the channelId.
    IMessageSvc *msgSvc;
    StatusCode status =Gaudi::svcLocator()->service("MessageSvc",msgSvc);
    if(status.isFailure()){
        std::cout <<  "Cannot locate MessageSvc" << std::endl;
    }

    MsgStream log( msgSvc, "CaloTTOnAttrIdMap");
    log<<MSG::ERROR<<" Online ID not found, channelId = " <<channelId.get_compact()<< endmsg;
    return 0;
}
unsigned int CaloTTOnAttrIdMap::pos_neg( HWIdentifier channelId ) const {
    std::map<HWIdentifier,AttrStruct>::const_iterator it=m_on2attrIdMap.find(channelId);

    if(it!=m_on2attrIdMap.end()){
        return ((*it).second).pos_neg;
    }

    // ERROR, can not find the channelId.
    IMessageSvc *msgSvc;
    StatusCode status =Gaudi::svcLocator()->service("MessageSvc",msgSvc);
    if(status.isFailure()){
        std::cout <<  "Cannot locate MessageSvc" << std::endl;
    }

    MsgStream log( msgSvc, "CaloTTOnAttrIdMap");
    log<<MSG::ERROR<<" Online ID not found, channelId = " <<channelId.get_compact()<< endmsg;
    return 0;
}

unsigned int CaloTTOnAttrIdMap::barrel_endcap_fcal( HWIdentifier channelId ) const {
    std::map<HWIdentifier,AttrStruct>::const_iterator it=m_on2attrIdMap.find(channelId);

    if(it!=m_on2attrIdMap.end()){
        return ((*it).second).barrel_endcap_fcal;
    }

    // ERROR, can not find the channelId.
    IMessageSvc *msgSvc;
    StatusCode status =Gaudi::svcLocator()->service("MessageSvc",msgSvc);
    if(status.isFailure()){
        std::cout <<  "Cannot locate MessageSvc" << std::endl;
    }

    MsgStream log( msgSvc, "CaloTTOnAttrIdMap");
    log<<MSG::ERROR<<" Online ID not found, channelId = " <<channelId.get_compact()<< endmsg;
    return 0;
}

unsigned int CaloTTOnAttrIdMap::module_type( HWIdentifier channelId ) const {
    std::map<HWIdentifier,AttrStruct>::const_iterator it=m_on2attrIdMap.find(channelId);

    if(it!=m_on2attrIdMap.end()){
        return ((*it).second).module_type;
    }

    // ERROR, can not find the channelId.
    IMessageSvc *msgSvc;
    StatusCode status =Gaudi::svcLocator()->service("MessageSvc",msgSvc);
    if(status.isFailure()){
        std::cout <<  "Cannot locate MessageSvc" << std::endl;
    }

    MsgStream log( msgSvc, "CaloTTOnAttrIdMap");
    log<<MSG::ERROR<<" Online ID not found, channelId = " <<channelId.get_compact()<< endmsg;
    return 0;
}

CaloTTOnAttrId_P* CaloTTOnAttrIdMap::getP() {
        return &m_persData;
}

void CaloTTOnAttrIdMap::convert_to_P( const CaloTTOnAttrId& d ) {

    CaloTTOnAttrId::const_iterator it = d.begin();
    CaloTTOnAttrId::const_iterator it_e = d.end();

    for ( ;it!=it_e;++it){
        const CaloTTOnAttrId_t& t = *it;
        CaloTTOnAttrId_P::__t t2 ;

        t2.crate = t.crate ;
        t2.module = t.module;
        t2.submodule= t.submodule;
        t2.channel = t.channel;

        t2.em_had = t.em_had;
        t2.pos_neg = t.pos_neg;
        t2.barrel_endcap_fcal = t.barrel_endcap_fcal;
        t2.module_type = t.module_type;

        m_persData.m_v.push_back(t2);
    }
}


void CaloTTOnAttrIdMap::convert_to_D( const CaloTTOnAttrId_P& p,  CaloTTOnAttrId& d ) {

    std::vector<CaloTTOnAttrId_P::__t>::const_iterator it = p.m_v.begin();
    std::vector<CaloTTOnAttrId_P::__t>::const_iterator it_e = p.m_v.end();

    d.clear();
    for ( ;it!=it_e;++it){
        const CaloTTOnAttrId_P::__t& t = *it;
        CaloTTOnAttrId_t t2 ;

        t2.crate = t.crate ;
        t2.module = t.module;
        t2.submodule= t.submodule;
        t2.channel = t.channel;

        t2.em_had = t.em_had;
        t2.pos_neg = t.pos_neg;
        t2.barrel_endcap_fcal = t.barrel_endcap_fcal;
        t2.module_type = t.module_type;

        d.push_back(t2);
    }
}

void CaloTTOnAttrIdMap::set (const CaloTTOnAttrId_P& p) {

    CaloTTOnAttrId d;
    convert_to_D(p,d);
    set(d);
    m_persData.m_version = p.m_version;
    return;
}
