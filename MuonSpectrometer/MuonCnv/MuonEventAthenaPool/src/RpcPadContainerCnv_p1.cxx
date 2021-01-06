/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcPadContainerCnv_p1.h"

#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonEventAthenaPool/RpcPad_p1.h"
#include "MuonEventAthenaPool/RpcPadContainer_p1.h"
#include "RpcPadCnv_p1.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthAllocators/DataPool.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h" // to be removed
#include "RPCcablingInterface/RpcPadIdHash.h" // to be removed
#include "RPCcablingInterface/IRPCcablingSvc.h" // to be removed

RpcPadContainerCnv_p1::RpcPadContainerCnv_p1() :
  m_rpcCabling(nullptr), // to be removed
  m_isInitialized(false),
  m_rpcCondData(nullptr) {   
}


StatusCode RpcPadContainerCnv_p1::initialize(MsgStream &log, IRPCcablingSvc* cabling) {

    // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();

    // to be removed
    if (cabling)
      m_rpcCabling = cabling;
    else {
      // get RPC cablingSvc
      const IRPCcablingServerSvc* RpcCabGet = 0;
      StatusCode sc =  svcLocator->service("RPCcablingServerSvc", RpcCabGet);
      if (sc.isFailure()) {
        log<<MSG::FATAL << "Could not get RPCcablingServerSvc !" << endmsg;
        return StatusCode::FAILURE;
      }
      else log <<MSG::VERBOSE << " RPCcablingServerSvc retrieved" << endmsg;

      sc = RpcCabGet->giveCabling(m_rpcCabling);
      if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get RPCcablingSvc from the Server !" << endmsg;
        m_rpcCabling = nullptr;
        return StatusCode::FAILURE;
      }
      else {
        log <<MSG::VERBOSE << " RPCcablingSvc obtained " << endmsg;
      }
    }

    log << MSG::DEBUG << "Converter initialized." << endmsg;

    m_isInitialized=true;
    return StatusCode::SUCCESS;
}

void RpcPadContainerCnv_p1::transToPers(const RpcPadContainer* transCont,  RpcPadContainer_p1* persCont, MsgStream &log) 
{

    typedef RpcPadContainer TRANS;

    RpcPadCnv_p1  cnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;

    int numColl = transCont->numberOfCollections();
    persCont->resize(numColl);    log << MSG::DEBUG  << " Preparing " << persCont->size() << "Collections" << endmsg;

    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        RpcPad_p1* pcoll = &( (*persCont)[collIndex] );
        cnv.transToPers( &(**it_Coll), pcoll , log);
    }
    log << MSG::DEBUG  << " ***  Writing RpcPadContainer ***" << endmsg;
}

void  RpcPadContainerCnv_p1::persToTrans(const RpcPadContainer_p1* persCont, RpcPadContainer* transCont, MsgStream &log) 
{


    RpcPadCnv_p1  cnv;

    log << MSG::DEBUG  << " Reading " << persCont->size() << "Collections" << endmsg;
    for (unsigned int icoll = 0; icoll < persCont->size(); ++icoll) {

        RpcPad* coll = new RpcPad;
        cnv.persToTrans( &( (*persCont)[icoll]), coll, log);

        int temp=-1;
        if (m_rpcCondData) temp = m_rpcCondData->operator()(coll->identify());
        else {
            const RpcPadIdHash* pdHashFn = m_rpcCabling->padHashFunction();
            temp = (*pdHashFn)(coll->identify());
        }
        if (temp==-1) {
            coll->m_idHash = IdentifierHash(); 
            log<<MSG::WARNING<<"get_hash failed - nothing else we can try!"<<endmsg;
        } else{
            coll->m_idHash = temp; 
        }
        
        // another check - see if already added
        auto ptr = transCont->indexFindPtr(coll->identifyHash());
        if (ptr!=nullptr ) {
            log << MSG::WARNING<<"Collection with hash="<<coll->identifyHash()<<" already exists in container (with "<<ptr->size()<<" elements). "
                << "Will therefore DISCARD this collection which has "<<coll->size()<<" elements)!"<<endmsg;
            delete coll;
        } else {
            StatusCode sc = transCont->addCollection(coll, coll->identifyHash());
            if (sc.isFailure()) {
                if (m_rpcCondData) {
                    log << MSG::WARNING<<"Could not add collection with hash="<<coll->identifyHash()
                    <<" to IDC which has hash max of "<<transCont->size()<<" (PadHashFunction gives "<<m_rpcCondData->max()<<")"<<endmsg;
                } else {
                    log << MSG::WARNING<<"Could not add collection with hash="<<coll->identifyHash()
                    <<" to IDC which has hash max of "<<transCont->size()<<" (PadHashFunction gives "<<m_rpcCabling->padHashFunction()->max()<<")"<<endmsg;
                }
                throw std::runtime_error("Failed to add collection to ID Container. Hash = "+std::to_string(coll->identifyHash()));
            }
        }
    }

    log << MSG::DEBUG  << " ***  Reading RpcPadContainer" << endmsg;
}



//================================================================
RpcPadContainer* RpcPadContainerCnv_p1::createTransient(const RpcPadContainer_p1* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize RpcPadContainerCnv_p1 " << endmsg;
            return 0;
        } 
    }
    int hashMax=-1;
    if (m_rpcCondData) hashMax = m_rpcCondData->max();
    else hashMax = m_rpcCabling->padHashFunction()->max();
    std::unique_ptr<RpcPadContainer> trans(new RpcPadContainer(hashMax));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


