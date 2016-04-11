/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FatrasEvent/PlanarCluster.h"
#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarCluster_p1.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainer_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p2.h"
#include "ISF_FatrasEvent/PlanarClusterContainer.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarClusterCnv_p1.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainerCnv_p1.h"
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

PlanarClusterContainerCnv_p1::PlanarClusterContainerCnv_p1() :
  m_isInitialized (0),
  m_detElementMapName ("Pixel_IdHashDetElementMap") 
{}

void PlanarClusterContainerCnv_p1::transToPers(const iFatras::PlanarClusterContainer* transCont, iFatras::PlanarClusterContainer_p1* persCont, MsgStream &log) {

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
    //PlanarClusterCnv_p1  chanCnv;
    if(!m_isInitialized) {
      if (this->initialize(log) != StatusCode::SUCCESS) {
	log << MSG::FATAL << "Could not initialize PlanarClusterContainerCnv_p1 " << endreq;
      }
    }
    PlanarClusterCnv_p1  chanCnv(m_pixId);

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
	  iFatras::PlanarCluster_p1* pchan = &(persCont->m_rawdata[i + chanBegin]);
	  const iFatras::PlanarCluster* chan = dynamic_cast<const iFatras::PlanarCluster*>(collection[i]);
	  chanCnv.transToPers(chan, pchan, log);
        }
    }
   
}

void  PlanarClusterContainerCnv_p1::persToTrans(const iFatras::PlanarClusterContainer_p1* persCont, iFatras::PlanarClusterContainer* transCont, MsgStream &log) 
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

    //PlanarClusterCnv_p1  chanCnv;
    PlanarClusterCnv_p1  chanCnv(m_pixId);
    unsigned int collBegin(0);
    // this is the id of the latest collection read in
    // This starts from the base of the TRT identifiers
    unsigned int idLast(0);
    
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {
      
      // Create trans collection - in NOT owner of PlanarDriftCircle (SG::VIEW_ELEMENTS)
      // IDet collection don't have the Ownership policy c'tor
      const InDet::InDetPRD_Collection_p2& pcoll = persCont->m_collections[icoll];       
      idLast += pcoll.m_hashId;
      // Identifier collID= Identifier(idLast);
      IdentifierHash collIDHash=IdentifierHash((unsigned int) idLast);
      Identifier collID = m_pixId->wafer_id(collIDHash);
      coll = new iFatras::PlanarClusterCollection(collIDHash);
      coll->setIdentifier(Identifier(collID));
      unsigned int nchans           = pcoll.m_size;
      coll->resize(nchans);
      iFatras::IdHashDetElementCollection::iterator it_map = m_detElementMap->find(collIDHash);

      iFatras::PlanarDetElement * de = it_map->second;
      // Fill with channels:
      // This is used to read the vector of errMat
      // values and lenght of the value are specified in separate vectors
      //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Reading collection with " << nchans << "Channels " << endreq;
      for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
	const iFatras::PlanarCluster_p1* pchan = &(persCont->m_rawdata[ichan + collBegin]);
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
iFatras::PlanarClusterContainer* PlanarClusterContainerCnv_p1::createTransient(const iFatras::PlanarClusterContainer_p1* persObj, MsgStream& log) {
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "PlanarClusterContainerCnv_p1::createTransient called " << endreq;
  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize PlanarClusterContainerCnv_p1 " << endreq;
    }
  }
  std::auto_ptr<iFatras::PlanarClusterContainer> trans(new iFatras::PlanarClusterContainer(m_pixId->wafer_hash_max()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


StatusCode PlanarClusterContainerCnv_p1::initialize(MsgStream &log) {
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
  } 

  //Retrieve and/or store the map with IdHash to DetElement 
  if ((detStore->contains<iFatras::IdHashDetElementCollection>(m_detElementMapName))){
    m_detElementMap = nullptr;
    if((detStore->retrieve(m_detElementMap, m_detElementMapName)).isFailure()){
      log << MSG::FATAL <<"Could not retrieve collection " << m_detElementMapName<< endreq;
      return StatusCode::FAILURE;
    }
    else
      log << MSG::DEBUG <<"Found and Retrieved collection " << m_detElementMapName<< endreq;
  }
  else
    m_detElementMap = new iFatras::IdHashDetElementCollection;
  
  return StatusCode::SUCCESS;
}

