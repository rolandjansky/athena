/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetEventTPCnv/InDetPrepRawData/SCT_Cluster_p3.h"
#include "InDetEventTPCnv/SCT_ClusterContainer_p3.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p2.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterCnv_p3.h"
#include "InDetEventTPCnv/SCT_ClusterContainerCnv_p3.h"
#include "AthAllocators/DataPool.h"

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


void SCT_ClusterContainerCnv_p3::transToPers(const InDet::SCT_ClusterContainer* transCont, InDet::SCT_ClusterContainer_p3* persCont, MsgStream &log) {

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all PRD
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

    typedef InDet::SCT_ClusterContainer TRANS;
        
    // this is the id of the latest collection read in
    // This starts from the base of the TRT identifiers
    unsigned int idLast(0);

    // 

    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;

    //to retrieve the SCT_ID helper
    if(!m_isInitialized) {
      if (this->initialize(log) != StatusCode::SUCCESS) {
        log << MSG::FATAL << "Could not initialize SCT_ClusterContainerCnv_p2 " << endmsg;
      }
    }

    SCT_ClusterCnv_p3  chanCnv(m_sctId);

    persCont->m_collections.resize(transCont->numberOfCollections());

    // to avoid the inside-loop resize
    int totSize = 0; 
    //for ( ; it_Coll != it_CollEnd;  it_Coll++)  {
    for ( it_Coll=transCont->begin(); it_Coll != it_CollEnd;  it_Coll++)  {
      const InDet::SCT_ClusterCollection& collection = (**it_Coll);
      totSize+=collection.size();
    }
    persCont->m_rawdata.resize(totSize);
    persCont->m_prdDeltaId.resize(totSize);

    //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Preparing " << persCont->m_collections.size() << "Collections" << endmsg;
    //for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
    for (collIndex = 0, it_Coll=transCont->begin(); it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        const InDet::SCT_ClusterCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        InDet::InDetPRD_Collection_p2& pcollection = persCont->m_collections[collIndex];
        unsigned int deltaId = (collection.identifyHash()-idLast);

        pcollection.m_hashId = deltaId;
        idLast=collection.identifyHash();
        pcollection.m_size = collection.size();
        // Add in channels

        for (unsigned int i = 0; i < collection.size(); ++i) {
            InDet::SCT_Cluster_p3* pchan = &(persCont->m_rawdata[i + chanBegin]);
            const InDet::SCT_Cluster* chan = dynamic_cast<const InDet::SCT_Cluster*>(collection[i]);
            chanCnv.transToPers(chan, pchan, log);

            persCont->m_prdDeltaId[i+chanBegin]=m_sctId->calc_offset(collection.identify(), chan->identify() );
        }
    }

}

void  SCT_ClusterContainerCnv_p3::persToTrans(const InDet::SCT_ClusterContainer_p3* persCont, InDet::SCT_ClusterContainer* transCont, MsgStream &log) 
{
    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize SCT_ClusterContainerCnv_p3 from persToTrans" << endmsg;
        }
    }

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


    InDet::SCT_ClusterCollection* coll = 0;

    //SCT_ClusterCnv_p2  chanCnv;
    SCT_ClusterCnv_p3  chanCnv(m_sctId);
    unsigned int collBegin(0);
    // this is the id of the latest collection read in
    // This starts from the base of the TRT identifiers
    unsigned int idLast(0);
    //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Reading " << persCont->m_collections.size() << "Collections" << endmsg;
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - in NOT owner of SCT_DriftCircle (SG::VIEW_ELEMENTS)
        // IDet collection don't have the Ownership policy c'tor
        const InDet::InDetPRD_Collection_p2& pcoll = persCont->m_collections[icoll];       
        idLast += pcoll.m_hashId;
        // Identifier collID= Identifier(idLast);
        IdentifierHash collIDHash=IdentifierHash((unsigned int) idLast);
        Identifier collID = m_sctId->wafer_id(collIDHash);
        coll = new InDet::SCT_ClusterCollection(collIDHash);
        coll->setIdentifier(Identifier(collID));
        unsigned int nchans           = pcoll.m_size;
        coll->resize(nchans);
        InDetDD::SiDetectorElement * de = m_sctMgr->getDetectorElement(collIDHash);
        // Fill with channels:
        // This is used to read the vector of errMat
        // values and lenght of the value are specified in separate vectors
        //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Reading collection with " << nchans << "Channels " << endmsg;
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const InDet::SCT_Cluster_p3* pchan = &(persCont->m_rawdata[ichan + collBegin]);
            //chan->m_clusId=Identifier(collID.get_compact()+persCont->m_prdDeltaId[ichan + collBegin]);
            Identifier clusId=m_sctId->strip_id_offset(coll->identify() , persCont->m_prdDeltaId[ichan + collBegin]);
            InDet::SCT_Cluster* chan = new InDet::SCT_Cluster
              (chanCnv.createSCT_Cluster (pchan, clusId, de, log));
            
            //            chan->m_rdoList.resize(1);
            //            chan->m_rdoList[0]=chan->m_clusId;
            //DC Bugfix: Set the idhash for this channel
            chan->setHashAndIndex(collIDHash,ichan);
            (*coll)[ichan] = chan;
        }
        collBegin += pcoll.m_size;

        // register the PRD collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }

    }


}



//================================================================
InDet::SCT_ClusterContainer* SCT_ClusterContainerCnv_p3::createTransient(const InDet::SCT_ClusterContainer_p3* persObj, MsgStream& log) {
    //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "SCT_ClusterContainerCnv_p2::createTransient called " << endmsg;
    if(!m_isInitialized) {
     if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize SCT_ClusterContainerCnv_p3 " << endmsg;
     }
    }
    std::auto_ptr<InDet::SCT_ClusterContainer> trans(new InDet::SCT_ClusterContainer(m_sctId->wafer_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


StatusCode SCT_ClusterContainerCnv_p3::initialize(MsgStream &log) {
   // Do not initialize again:
   m_isInitialized=true;
   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "SCT_ClusterContainerCnv_p2::initialize called " << endmsg;
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
   } 
   //   else {
   //        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
   //   }

   // Get the sct helper from the detector store
   sc = detStore->retrieve(m_sctId, "SCT_ID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get SCT_ID helper !" << endmsg;
      return StatusCode::FAILURE;
   } 
   //   else {
   //     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the SCT_ID helper." << endmsg;
   //   }

   sc = detStore->retrieve(m_sctMgr);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get SCT_DetectorDescription" << endmsg;
      return sc;
   }

   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
   return StatusCode::SUCCESS;
}



