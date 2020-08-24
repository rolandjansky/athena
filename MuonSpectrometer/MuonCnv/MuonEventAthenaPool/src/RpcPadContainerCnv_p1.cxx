/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonEventAthenaPool/RpcPad_p1.h"
#include "MuonEventAthenaPool/RpcPadContainer_p1.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "RpcPadCnv_p1.h"
#include "RpcPadContainerCnv_p1.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "AthAllocators/DataPool.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/RpcPadIdHash.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"



StatusCode RpcPadContainerCnv_p1::initialize(MsgStream &log,
                                             IRPCcablingSvc* cabling /*= nullptr*/) {
   // Do not initialize again:
    m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();


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
        m_rpcCabling = 0;
        return StatusCode::FAILURE;
      }
      else {
        log <<MSG::VERBOSE << " RPCcablingSvc obtained " << endmsg;
      }
    }

    log << MSG::DEBUG << "Converter initialized." << endmsg;
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
        //log << MSG::DEBUG  << " New collection" << endmsg;
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

        // Identifier collID(Identifier(pcoll.m_id));
        // IdentifierHash collIDHash(IdentifierHash(persCont[icoll].m_hashId));
        RpcPad* coll = new RpcPad;
        cnv.persToTrans( &( (*persCont)[icoll]), coll, log);
         // register the rdo collection in IDC with hash - faster addCollection
// log<<MSG::INFO<<"coll has size "<<coll->size()<<endmsg;
// log << MSG::INFO<<" coll has hash: "<<static_cast<unsigned int>(coll->identifyHash())<<endmsg;
// log << MSG::INFO<<" coll has id: "<<static_cast<unsigned int>(coll->identify())<<endmsg;

// hack to fix bug#56429 - always use identifier to get hash

        // sanity check
//        if (coll->identifyHash() > static_cast<unsigned int>(m_rpcCabling->padHashFunction()->max() ) ) {
//            m_errorCount++;

//            if (m_errorCount<m_maxNumberOfErrors) 
//               log<<MSG::WARNING<< "The persistent RDO had an IdentifierHash of "<<coll->identifyHash()
//                <<" which is larger than the hash max of "<<m_rpcCabling->padHashFunction()->max()
//                <<". Will attempt to correct from Identifier. This is message "<<m_errorCount<<" of "
//                <<m_maxNumberOfErrors<<" maximum."<<endmsg;
//            if (m_errorCount==m_maxNumberOfErrors) log<<MSG::WARNING<<"Exceeding max errors. Further WARNINGs will be suppressed!"<<endmsg;
            const RpcPadIdHash* pdHashFn = m_rpcCabling->padHashFunction();
            int temp = (*pdHashFn)(coll->identify());
            if (temp==-1) {
                coll->m_idHash = IdentifierHash(); 
                log<<MSG::WARNING<<"get_hash failed - nothing else we can try!"<<endmsg;
            } else{
                coll->m_idHash = temp; 
//                if (m_errorCount<m_maxNumberOfErrors) log<<MSG::WARNING<<"New hash is: "<<coll->identifyHash()<<endmsg;
            }
//        }
        
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
                    <<" to IDC which has hash max of "<<transCont->size()<<" (PadHashFunction gives "<<m_rpcCabling->padHashFunction()->max()<<")"<<endmsg;
                throw std::runtime_error("Failed to add collection to ID Container. Hash = "+std::to_string(coll->identifyHash()));
            }
        }
        // if (log.level() <= MSG::DEBUG) {
        //     log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " << 
        //         persCont[icoll].m_id << ", added to Identifiable container." << endmsg;
        // }
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
    // log<<MSG::INFO<<"creating new pad container with hashmax= "<<m_RpcId->module_hash_max()<<endmsg;
    
    std::unique_ptr<RpcPadContainer> trans(new RpcPadContainer(m_rpcCabling->padHashFunction()->max() ));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


