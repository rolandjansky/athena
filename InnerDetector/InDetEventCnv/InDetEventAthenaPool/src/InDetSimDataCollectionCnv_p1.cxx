/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimData/InDetSimData.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p1.h"
#include "InDetSimDataCollectionCnv_p1.h"
#include "InDetSimDataCnv_p1.h"
#include "Identifier/Identifier.h"
#include "MsgUtil.h"
#include "StoreGate/StoreGateSvc.h"

void InDetSimDataCollectionCnv_p1::transToPers(const InDetSimDataCollection* transCont, InDetSimDataCollection_p1* persCont, MsgStream &log)
{
  InDetSimDataCnv_p1  simDataCnv;
  persCont->m_simdata.resize(transCont->size());
  MSG_DEBUG(log," Preparing " << persCont->m_simdata.size() << "Collections");
  unsigned int collIndex(0);
  for (const auto& transSimDataPair : *transCont) {
    // Add in new collection
    (persCont->m_simdata[collIndex]).first = transSimDataPair.first.get_identifier32().get_compact();
    const InDetSimData& simData = transSimDataPair.second;
    InDetSimData_p1& psimData = persCont->m_simdata[collIndex].second;
    simDataCnv.transToPers(&simData,&psimData,log);
    ++collIndex;
  }
  MSG_DEBUG(log," ***  Writing InDetSimdataCollection");
}

void  InDetSimDataCollectionCnv_p1::persToTrans(const InDetSimDataCollection_p1* persCont, InDetSimDataCollection* transCont, MsgStream &log)
{
  InDetSimDataCnv_p1  simDataCnv;
  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize InDetSimDataCollectionCnv_p1 " << endmsg;
    }
  }
  MSG_DEBUG(log," Preparing " << persCont->m_simdata.size() << "Collections");
  simDataCnv.setCurrentStore (SG::CurrentEventStore::store());
  for (const auto& persSimDataPair : persCont->m_simdata) {
    // Add in new collection
    const InDetSimData_p1& psimData = persSimDataPair.second;
    InDetSimData simData;
    simDataCnv.persToTrans(&psimData,&simData,log);
    // For 64-bit ids, when reading in the old 32-bit ids, we must
    // treat pixel channel ids specially before creating a new
    // 64-bit id
    Identifier id;
    if (m_pixId->is_shortened_pixel_id(persSimDataPair.first)) {
      id = m_pixId->pixel_id_from_shortened(persSimDataPair.first); }
    else { id = persSimDataPair.first; }
    transCont->insert( transCont->end(), std :: make_pair( id, simData ) );
    transCont->insert( transCont->end(), std :: make_pair( Identifier( persSimDataPair.first ), simData ) );
  }
  MSG_DEBUG(log," ***  Reading InDetSimdataCollection");
}


StatusCode InDetSimDataCollectionCnv_p1::initialize(MsgStream &log) {
  // Do not initialize again:
  m_isInitialized=true;

  // Get Storegate, ID helpers, and so on
  ISvcLocator* svcLocator = Gaudi::svcLocator();

  // get DetectorStore service
  StoreGateSvc *detStore{};
  if (svcLocator->service("DetectorStore", detStore).isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get the sct helper from the detector store
  if (detStore->retrieve(m_pixId, "PixelID").isFailure()) {
    log << MSG::FATAL << "Could not get PixelID helper !" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
