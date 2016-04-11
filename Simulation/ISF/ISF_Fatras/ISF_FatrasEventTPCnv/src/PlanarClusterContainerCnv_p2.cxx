/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FatrasEvent/PlanarCluster.h"
#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarCluster_p2.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainer_p2.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p2.h"
#include "ISF_FatrasEvent/PlanarClusterContainer.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarClusterCnv_p2.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainerCnv_p2.h"
#include "DataModel/DataPool.h"

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

PlanarClusterContainerCnv_p2::PlanarClusterContainerCnv_p2() :
  m_isInitialized (0),
  m_pixelDetElementMapName ("Pixel_IdHashDetElementMap"),
  m_sctDetElementMapName ("SCT_IdHashDetElementMap")
{}

void PlanarClusterContainerCnv_p2::transToPers(const iFatras::PlanarClusterContainer* transCont, iFatras::PlanarClusterContainer_p2* persCont, MsgStream &log) {

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

    typedef iFatras::PlanarClusterContainer TRANS;
        
    // this is the id of the latest collection read in
    // This starts from the base of the TRT identifiers
    unsigned int idLast(0);

    // 
    //PlanarClusterCnv_p2  chanCnv;
    if(!m_isInitialized) {
      if (this->initialize(log) != StatusCode::SUCCESS) {
	log << MSG::FATAL << "Could not initialize PlanarClusterContainerCnv_p2 " << endreq;
      }
    }
    PlanarClusterCnv_p2  chanCnv(m_pixId, m_sctId);

    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    persCont->m_collections.resize(transCont->numberOfCollections());
    
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        const iFatras::PlanarClusterCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        InDet::InDetPRD_Collection_p2& pcollection = persCont->m_collections[collIndex];
	unsigned int deltaId = (collection.identifyHash()-idLast);
	
        pcollection.m_hashId = deltaId;
	idLast=collection.identifyHash();
        pcollection.m_size = collection.size();
        
	// Add in channels
        persCont->m_rawdata.resize(chanEnd);
	
	for (unsigned int i = 0; i < collection.size(); ++i) {
	  iFatras::PlanarCluster_p2* pchan = &(persCont->m_rawdata[i + chanBegin]);
	  const iFatras::PlanarCluster* chan = dynamic_cast<const iFatras::PlanarCluster*>(collection[i]);
	  chanCnv.transToPers(chan, pchan, log);
        }
    }
   
}

void  PlanarClusterContainerCnv_p2::persToTrans(const iFatras::PlanarClusterContainer_p2* persCont, iFatras::PlanarClusterContainer* transCont, MsgStream &log) 
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


    iFatras::PlanarClusterCollection* coll = 0;

    //PlanarClusterCnv_p2  chanCnv;
    PlanarClusterCnv_p2  chanCnv(m_pixId, m_sctId);
    unsigned int collBegin(0);
    // this is the id of the latest collection read in
    // This starts from the base of the TRT identifiers
    unsigned int idLast(0);
    
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {
      
      // Create trans collection - in NOT owner of PlanarDriftCircle (SG::VIEW_ELEMENTS)
      // IDet collection don't have the Ownership policy c'tor
      const InDet::InDetPRD_Collection_p2& pcoll = persCont->m_collections[icoll];       
      idLast += pcoll.m_hashId;
      IdentifierHash collIDHash=IdentifierHash((unsigned int) idLast);
      // Let's check which coll we have first
      bool isPixelColl = (persCont->m_rawdata[collBegin]).m_isFromPixel;
      Identifier collID = isPixelColl ? m_pixId->wafer_id(collIDHash) : m_sctId->wafer_id(collIDHash);
      coll = new iFatras::PlanarClusterCollection(collIDHash);
      coll->setIdentifier(Identifier(collID));
      unsigned int nchans           = pcoll.m_size;
      coll->resize(nchans);
      
      // Get the corresponding PlanarDetectorElement
      iFatras::IdHashDetElementCollection::iterator it_map = isPixelColl ? m_pixelDetElementMap->find(collIDHash) : m_sctDetElementMap->find(collIDHash);
      iFatras::PlanarDetElement * de = it_map->second;
      
      // Fill with channels:
      // This is used to read the vector of errMat
      // values and lenght of the value are specified in separate vectors
      //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Reading collection with " << nchans << "Channels " << endreq;
      for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
	const iFatras::PlanarCluster_p2* pchan = &(persCont->m_rawdata[ichan + collBegin]);
	// In preparation for 64-bit ids, set the initial cluster
	// id to the wafer id. Actual cluster id will be set in
	// the converter from diff.
        iFatras::PlanarCluster* chan = new iFatras::PlanarCluster
          (chanCnv.createPlanarCluster (pchan, collID, de, log));
	// DC Bugfix: Set the hash index!
	chan->setHashAndIndex(collIDHash, ichan);
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
iFatras::PlanarClusterContainer* PlanarClusterContainerCnv_p2::createTransient(const iFatras::PlanarClusterContainer_p2* persObj, MsgStream& log) {
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "PlanarClusterContainerCnv_p2::createTransient called " << endreq;
  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize PlanarClusterContainerCnv_p2 " << endreq;
    }
  }
  std::auto_ptr<iFatras::PlanarClusterContainer> trans(new iFatras::PlanarClusterContainer(m_pixId->wafer_hash_max()+m_sctId->wafer_hash_max()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


StatusCode PlanarClusterContainerCnv_p2::initialize(MsgStream &log) {
  // Do not initialize again:
  m_isInitialized=true;
  
  // Get Storegate, ID helpers, and so on
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  // get StoreGate service
  StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    log << MSG::FATAL << "StoreGate service not found !" << endreq;
    return StatusCode::FAILURE;
  }
  
  // get DetectorStore service
  StoreGateSvc *detStore;
  sc = svcLocator->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  } 
  
  // Get the sct helper from the detector store
  sc = detStore->retrieve(m_pixId, "PixelID");
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get PixelID helper !" << endreq;
    return StatusCode::FAILURE;
  } else log << MSG::INFO << "PixelID helper retrieved !" << endreq;      
  
  
  //Retrieve the map with IdHash to DetElement 
  if ((detStore->contains<iFatras::IdHashDetElementCollection>(m_pixelDetElementMapName))){
    m_pixelDetElementMap = nullptr;
    if((detStore->retrieve(m_pixelDetElementMap, m_pixelDetElementMapName)).isFailure()){
      log << MSG::FATAL <<"Could not retrieve collection " << m_pixelDetElementMapName << endreq;
      return StatusCode::FAILURE;
    }
    else 
      log << MSG::INFO <<"Found and Retrieved collection " << m_pixelDetElementMapName << endreq;
  }
  else
    m_pixelDetElementMap = new iFatras::IdHashDetElementCollection;
  
  // Get the sct helper from the detector store
  if ((detStore->contains<SCT_ID>("SCT_ID"))){
    sc = detStore->retrieve(m_sctId, "SCT_ID");
    if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get SCT_ID helper !" << endreq;
      return StatusCode::FAILURE;
    } 
    else log << MSG::INFO << "SCT_ID helper retrieved !" << endreq;      
  } else log << MSG::INFO << "SCT_ID helper NOT  in the detStore !" << endreq;

  //Retrieve the map with IdHash to DetElement 
  if ((detStore->contains<iFatras::IdHashDetElementCollection>(m_sctDetElementMapName))){
    m_sctDetElementMap = nullptr;
    if((detStore->retrieve(m_sctDetElementMap, m_sctDetElementMapName)).isFailure()){
      log << MSG::FATAL <<"Could not retrieve collection " << m_sctDetElementMapName<< endreq;
      return StatusCode::FAILURE;
    }
    else
      log << MSG::INFO <<"Found and Retrieved collection " << m_sctDetElementMapName<< endreq;
  }
  else
    m_sctDetElementMap = new iFatras::IdHashDetElementCollection;
  
  return StatusCode::SUCCESS;
}

