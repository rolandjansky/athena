/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcPadContainerCnv_p2.h"

#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonEventAthenaPool/RpcPad_p1.h"
#include "MuonEventAthenaPool/RpcPadContainer_p2.h"
#include "RpcPadCnv_p1.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthAllocators/DataPool.h"

RpcPadContainerCnv_p2::RpcPadContainerCnv_p2() :
  m_isInitialized(false){
}


StatusCode RpcPadContainerCnv_p2::initialize(MsgStream &log) {
                                             
   // Do not initialize again:
    m_isInitialized=true;

    // Get the helper from the detector store
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StoreGateSvc *detStore;
    StatusCode sc = svcLocator->service("DetectorStore", detStore);
    if (sc.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
    }
    
    sc = detStore->retrieve(m_rpcIdHelper);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get ID helper !" << endmsg;
        return StatusCode::FAILURE;
    }
    else log<<MSG::DEBUG<<" got RpcIdHelper"<<endmsg;

    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
    return sc;
}

void RpcPadContainerCnv_p2::transToPers(const RpcPadContainer* transCont,  RpcPadContainer_p2* persCont, MsgStream &log) 
{

    typedef RpcPadContainer TRANS;

    RpcPadCnv_p1  cnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;

    int numColl = transCont->numberOfCollections();
    persCont->m_pads.resize(numColl);    log << MSG::DEBUG  << " Preparing " << persCont->m_pads.size() << "Collections" << endmsg;

    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        RpcPad_p1* pcoll = &( persCont->m_pads[collIndex] );
        cnv.transToPers( &(**it_Coll), pcoll , log);
    }
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Writing RpcPadContainer ***" << endmsg;
}

void  RpcPadContainerCnv_p2::persToTrans(const RpcPadContainer_p2* persCont, RpcPadContainer* transCont, MsgStream &log) 
{
    RpcPadCnv_p1  cnv;

    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Reading " << persCont->m_pads.size() << "Collections" << endmsg;
    for (unsigned int icoll = 0; icoll < persCont->m_pads.size(); ++icoll) {

        RpcPad* coll = new RpcPad;
        cnv.persToTrans( &( persCont->m_pads[icoll]), coll, log);
        // another check - see if already added
        auto ptr = transCont->indexFindPtr(coll->identifyHash());
        if (ptr!=nullptr ) {
            log << MSG::WARNING<<"Collection with hash="<<coll->identifyHash()<<" already exists in container (with "<<ptr->size()<<" elements). "
                << "Will therefore DISCARD this collection which has "<<coll->size()<<" elements)!"<<endmsg;
            delete coll;
        } else {
            StatusCode sc = transCont->addCollection(coll, coll->identifyHash());
            if (sc.isFailure()) {
                log << MSG::WARNING<<"Could not add collection with hash="<<coll->identifyHash()
                    <<" to IDC which has hash max of "<<transCont->size()<<" (PadHashFunction gives "<<m_rpcIdHelper->module_hash_max()<<endmsg;
                throw std::runtime_error("Failed to add collection to ID Container. Hash = "+std::to_string(coll->identifyHash()));
            }
        }
    }

    log << MSG::DEBUG  << " ***  Reading RpcPadContainer" << endmsg;
}



//================================================================
RpcPadContainer* RpcPadContainerCnv_p2::createTransient(const RpcPadContainer_p2* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize RpcPadContainerCnv_p2 " << endmsg;
            return 0;
        }
    }
    std::unique_ptr<RpcPadContainer> trans(new RpcPadContainer(m_rpcIdHelper->module_hash_max()));
    
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


