/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Athena
#include "AthenaKernel/errorcheck.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Container_p1.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterContainerCnv_p1.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

void InDet::PixelClusterContainerCnv_p1::transToPers(const InDet::PixelClusterContainer* transCont,  InDet::InDetPRD_Container_p1* persCont, MsgStream &log) 
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

    typedef InDet::PixelClusterContainer TRANS;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

    PixelClusterCnv_p1  chanCnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    persCont->m_collections.resize(transCont->numberOfCollections());
  
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, ++it_Coll)  {
        // Add in new collection
        const InDet::PixelClusterCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        InDet::InDetPRD_Collection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_compact();
        pcollection.m_hashId = (unsigned int) collection.identifyHash();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_PRD.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            const InDet::PixelCluster* chan = collection[i];
            persCont->m_PRD[i + chanBegin] = toPersistent((CONV**)0, chan, log );
        }
    }
}

void  InDet::PixelClusterContainerCnv_p1::persToTrans(const InDet::InDetPRD_Container_p1* persCont, InDet::PixelClusterContainer* transCont, MsgStream &log) 
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

    const InDetDD::SiDetectorElementCollection* elements(nullptr);
    if (m_useDetectorElement) {
        SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
        const InDetDD::SiDetectorElementCollection* elements = *pixelDetEleHandle;
        if (not pixelDetEleHandle.isValid() or elements==nullptr) {
            log << MSG::FATAL << m_pixelDetEleCollKey.fullKey() << " is not available." << endmsg;
            return;
        }
    }

    InDet::PixelClusterCollection* coll = 0;

    PixelClusterCnv_p1  chanCnv;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - is NOT owner of PixelCluster (SG::VIEW_ELEMENTS)
	// IDet collection don't have the Ownership policy c'tor
        const InDet::InDetPRD_Collection_p1& pcoll = persCont->m_collections[icoll];        
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        coll = new InDet::PixelClusterCollection(collIDHash);
        coll->setIdentifier(Identifier(pcoll.m_id));
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);
        const InDetDD::SiDetectorElement * de = (m_useDetectorElement ? elements->getDetectorElement(collIDHash) : nullptr);
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const TPObjRef pchan = persCont->m_PRD[ichan + pcoll.m_begin];
            InDet::PixelCluster* chan = dynamic_cast<InDet::PixelCluster*>(createTransFromPStore((CONV**)0, pchan, log ) );
            if (chan){
              chan->m_detEl = de;
              (*coll)[ichan] = chan;
            } else {
              log << MSG::WARNING  << "Cast to InDet::PixelCluster* failed in PixelClusterContainerCnv_p1" << endmsg;
            }
        }
        
        // register the rdo collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }

    }

}



//================================================================
InDet::PixelClusterContainer* InDet::PixelClusterContainerCnv_p1::createTransient(const InDet::InDetPRD_Container_p1* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
     if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize PixelClusterContainerCnv_p1 " << endmsg;
      return nullptr;
     } 
    }
    if (not m_pixId) {
      log << MSG::FATAL << "PixelID helper is null in  PixelClusterContainerCnv_p1::createTransient" << endmsg;
      return nullptr;
    }
    std::unique_ptr<InDet::PixelClusterContainer> trans(std::make_unique<InDet::PixelClusterContainer>(m_pixId->wafer_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}

StatusCode InDet::PixelClusterContainerCnv_p1::initialize(MsgStream &log) {
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
   } 
  

   // Get the pixel helper from the detector store
   sc = detStore->retrieve(m_pixId, "PixelID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get PixelID helper !" << endmsg;
      return StatusCode::FAILURE;
   } 

   CHECK(m_pixelDetEleCollKey.initialize(m_useDetectorElement));

   return StatusCode::SUCCESS;
}

// Method for test/PixelClusterContainerCnv_p1_test.cxx
void InDet::PixelClusterContainerCnv_p1::setUseDetectorElement(const bool useDetectorElement) {
   m_useDetectorElement = useDetectorElement;
}
