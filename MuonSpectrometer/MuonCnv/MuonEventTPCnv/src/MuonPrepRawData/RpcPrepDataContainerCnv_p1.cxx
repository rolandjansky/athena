/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepData_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataCnv_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataContainerCnv_p1.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "AthAllocators/DataPool.h"

StatusCode Muon::RpcPrepDataContainerCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
    m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
    StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
        log << MSG::FATAL << "StoreGate service not found !" << endmsg;
        return StatusCode::FAILURE;
    }

   // get DetectorStore service
    StoreGateSvc *detStore;
    sc = svcLocator->service("DetectorStore", detStore);
    if (sc.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
    }

   // Get the pixel helper from the detector store
    sc = detStore->retrieve(m_RpcId);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get Rpc ID helper !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the Rpc ID helper." << endmsg;
    }

    sc = detStore->retrieve(m_muonDetMgr);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get PixelDetectorDescription" << endmsg;
        return sc;
    }

    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
    return StatusCode::SUCCESS;
}

void Muon::RpcPrepDataContainerCnv_p1::transToPers(const Muon::RpcPrepDataContainer* transCont,  Muon::MuonPRD_Container_p1* persCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all RDO
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

    typedef Muon::RpcPrepDataContainer TRANS;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

    RpcPrepDataCnv_p1  chanCnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    int numColl = transCont->numberOfCollections();
    // if(numColl == transCont->hashFunc().max() ) { // let's count how many collections we have:
    //  numColl = 0;
    //  for ( ; it_Coll != it_CollEnd; it_Coll++)
    //     numColl++;
    //  it_Coll     = transCont->begin(); // reset the iterator, we used it!
    // }
    persCont->m_collections.resize(numColl);    
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Preparing " << persCont->m_collections.size() << "Collections" << endmsg;

    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " New collection" << endmsg;
        const Muon::RpcPrepDataCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        Muon::MuonPRD_Collection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_identifier32().get_compact();
        pcollection.m_hashId = (unsigned int) collection.identifyHash();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_PRD.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            const Muon::RpcPrepData* chan = collection[i];
            persCont->m_PRD[i + chanBegin] = toPersistent((CONV**)0, chan, log );
        }
    }
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Writing RpcPrepDataContainer ***" << endmsg;
}

void  Muon::RpcPrepDataContainerCnv_p1::persToTrans(const Muon::MuonPRD_Container_p1* persCont, Muon::RpcPrepDataContainer* transCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all channels
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and extract their channels
    // from the vector.


    Muon::RpcPrepDataCollection* coll = 0;

    RpcPrepDataCnv_p1  chanCnv;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

    log << MSG::DEBUG  << " Reading " << persCont->m_collections.size() << "Collections" << endmsg;
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {
        // Create trans collection - is NOT owner of RpcPrepData (SG::VIEW_ELEMENTS)
    // IDet collection don't have the Ownership policy c'tor
        const Muon::MuonPRD_Collection_p1& pcoll = persCont->m_collections[icoll];        
        Identifier collID(Identifier(pcoll.m_id));
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        coll = new Muon::RpcPrepDataCollection(collIDHash);
        coll->setIdentifier(Identifier(pcoll.m_id));
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);
//        MuonDD::RpcReadoutElement * de = m_muonDetMgr->getDetectorElement(collIDHash);
// No hash based lookup for Rpcs?
//        const MuonGM::RpcReadoutElement * de = m_muonDetMgr->getRpcReadoutElement(collID);
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const TPObjRef pchan = persCont->m_PRD[ichan + pcoll.m_begin];
            Muon::RpcPrepData* chan = dynamic_cast<Muon::RpcPrepData*>(createTransFromPStore((CONV**)0, pchan, log ) );
            if (!chan) {
               log << MSG::ERROR << "RpcPrepDataContainerCnv_p1::persToTrans: Cannot get RpcPrepData!" << endmsg;
               continue;
            }
            const MuonGM::RpcReadoutElement * de = m_muonDetMgr->getRpcReadoutElement(chan->identify());
            chan->m_detEl = de;
            (*coll)[ichan] = chan;
            
            // check to handle cases explained in bug#56227
            if (de->Nphigasgaps() == 1 && de->NphiStripPanels() == 2){
                // okay, so this was originally on a large surface and needs correcting.
                const Amg::Vector3D& globalposHIT = de->stripPos( chan->identify() );
                
                double tolerance = 3.0;
                const Trk::PlaneSurface& surf = de->surface( chan->identify() );
                const Amg::Vector2D* pos = surf.Trk::Surface::globalToLocal(globalposHIT,tolerance); 
                if (!pos){
                  log << MSG::WARNING << "RpcPrepDataContainerCnv_p1::persToTrans - globalToLocal failed!"<<endmsg;
                  chan->m_localPos = Amg::Vector2D(0,0);
                }
                else chan->m_localPos = *pos;
            }
        }

        // register the rdo collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }
        if (log.level() <= MSG::DEBUG) {
            log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " << 
                collID.get_compact() << ", added to Identifiable container." << endmsg;
        }
    }

    log << MSG::DEBUG  << " ***  Reading RpcPrepDataContainer" << endmsg;
}



//================================================================
Muon::RpcPrepDataContainer* Muon::RpcPrepDataContainerCnv_p1::createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize RpcPrepDataContainerCnv_p1 " << endmsg;
            return 0;
        } 
    }
    std::auto_ptr<Muon::RpcPrepDataContainer> trans(new Muon::RpcPrepDataContainer(m_RpcId->module_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


