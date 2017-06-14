/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinData_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataContainer_p1.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataCnv_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataContainerCnv_p1.h"

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



#include "AthAllocators/DataPool.h"

StatusCode Muon::TgcCoinDataContainerCnv_p1::initialize(MsgStream &log) {
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
        log << MSG::DEBUG << "Found DetectorStore." << endmsg;
    }

   // Get the pixel helper from the detector store
    sc = detStore->retrieve(m_TgcId);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get Tgc ID helper !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        log << MSG::DEBUG << "Found the Tgc ID helper." << endmsg;
    }

    sc = detStore->retrieve(m_muonDetMgr);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get PixelDetectorDescription" << endmsg;
        return sc;
    }

    log << MSG::DEBUG << "Converter initialized." << endmsg;
    return StatusCode::SUCCESS;
}

void Muon::TgcCoinDataContainerCnv_p1::transToPers(const Muon::TgcCoinDataContainer* transCont,  Muon::MuonCoinDataContainer_p1* persCont, MsgStream &log) 
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

    typedef Muon::TgcCoinDataContainer TRANS;
    typedef ITPConverterFor<Muon::TgcCoinData> CONV;

    TgcCoinDataCnv_p1  chanCnv;
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
    persCont->m_collections.resize(numColl);    log << MSG::DEBUG  << " Preparing " << persCont->m_collections.size() << "Collections" << endmsg;

    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        log << MSG::DEBUG  << " New collection" << endmsg;
        const Muon::TgcCoinDataCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        Muon::MuonCoinDataCollection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_identifier32().get_compact();
        pcollection.m_hashId = (unsigned int) collection.identifyHash();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_CoinData.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            const Muon::TgcCoinData* chan = collection[i];
            persCont->m_CoinData[i + chanBegin] = toPersistent((CONV**)0, chan, log );
        }
    }
    log << MSG::DEBUG  << " ***  Writing TgcCoinDataContainer ***" << endmsg;
}

void  Muon::TgcCoinDataContainerCnv_p1::persToTrans(const Muon::MuonCoinDataContainer_p1* persCont, Muon::TgcCoinDataContainer* transCont, MsgStream &log) 
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


    Muon::TgcCoinDataCollection* coll = 0;

    TgcCoinDataCnv_p1  chanCnv;
    typedef ITPConverterFor<Muon::TgcCoinData> CONV;

    log << MSG::DEBUG  << " Reading " << persCont->m_collections.size() << "Collections" << endmsg;
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - is NOT owner of TgcCoinData (SG::VIEW_ELEMENTS)
    // IDet collection don't have the Ownership policy c'tor
        const Muon::MuonCoinDataCollection_p1& pcoll = persCont->m_collections[icoll];        
        Identifier collID(Identifier(pcoll.m_id));
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        coll = new Muon::TgcCoinDataCollection(collIDHash);
        coll->setIdentifier(Identifier(pcoll.m_id));
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);

        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const TPObjRef pchan = persCont->m_CoinData[ichan + pcoll.m_begin];
            Muon::TgcCoinData* chan = dynamic_cast<Muon::TgcCoinData*>(createTransFromPStore((CONV**)0, pchan, log ) );
	    if(chan->type()!=Muon::TgcCoinData::TYPE_TRACKLET_EIFI) {
	      const MuonGM::TgcReadoutElement * deOut = m_muonDetMgr->getTgcReadoutElement(Identifier(chan->channelIdOut()));
	      chan->m_detElOut = deOut;
	    }
	    if(chan->type()==Muon::TgcCoinData::TYPE_TRACKLET || chan->type()==Muon::TgcCoinData::TYPE_HIPT || 
	       chan->type()==Muon::TgcCoinData::TYPE_TRACKLET_EIFI) {
	      const MuonGM::TgcReadoutElement * deIn = m_muonDetMgr->getTgcReadoutElement(Identifier(chan->channelIdIn()));
	      chan->m_detElIn = deIn;
	    }
	    else {
	      chan->m_detElIn = 0;
	    }
            (*coll)[ichan] = chan;
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

    log << MSG::DEBUG  << " ***  Reading TgcCoinDataContainer" << endmsg;
}



//================================================================
Muon::TgcCoinDataContainer* Muon::TgcCoinDataContainerCnv_p1::createTransient(const Muon::MuonCoinDataContainer_p1* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize TgcCoinDataContainerCnv_p1 " << endmsg;
            return 0;
        } 
    }
    std::auto_ptr<Muon::TgcCoinDataContainer> trans(new Muon::TgcCoinDataContainer(m_TgcId->module_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


