/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


    typedef InDetSimDataCollection TRANS;

    InDetSimDataCnv_p1  simDataCnv;

    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    persCont->m_simdata.resize(transCont->size());
    MSG_DEBUG(log," Preparing " << persCont->m_simdata.size() << "Collections");
    for (int collIndex=0 ; it_Coll != it_CollEnd; it_Coll++, collIndex++)  {
        // Add in new collection
        (persCont->m_simdata[collIndex]).first = (*it_Coll).first.get_identifier32().get_compact();
        const InDetSimData& simData = (*it_Coll).second;
        InDetSimData_p1& psimData = persCont->m_simdata[collIndex].second;
        simDataCnv.transToPers(&simData,&psimData,log);
    }
    MSG_DEBUG(log," ***  Writing InDetSimdataCollection");
}

void  InDetSimDataCollectionCnv_p1::persToTrans(const InDetSimDataCollection_p1* persCont, InDetSimDataCollection* transCont, MsgStream &log)
{

    //typedef InDetSimDataCollection_p1 PERS;
    typedef std::vector<std::pair<Identifier32::value_type, InDetSimData_p1 > >::const_iterator COLLITER;
    InDetSimDataCnv_p1  simDataCnv;

    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize InDetSimDataCollectionCnv_p1 " << endreq;
        }
    }

    COLLITER it_Coll     = persCont->m_simdata.begin();
    COLLITER it_CollEnd  = persCont->m_simdata.end();
    MSG_DEBUG(log," Preparing " << persCont->m_simdata.size() << "Collections");
    for (int collIndex=0 ; it_Coll != it_CollEnd; it_Coll++, collIndex++)  {

        // Add in new collection
        const InDetSimData_p1& psimData = persCont->m_simdata[collIndex].second;
        InDetSimData simData;
        simDataCnv.persToTrans(&psimData,&simData,log);
        
        // For 64-bit ids, when reading in the old 32-bit ids, we must
        // treat pixel channel ids specially before creating a new
        // 64-bit id
        Identifier id;
        if (m_pixId->is_shortened_pixel_id(it_Coll->first)) { 
            id = m_pixId->pixel_id_from_shortened(it_Coll->first); }
        else { id = it_Coll->first; }
        transCont->insert( transCont->end(), std :: make_pair( id, simData ) );
    }
    MSG_DEBUG(log," ***  Reading InDetSimdataCollection");

}


StatusCode InDetSimDataCollectionCnv_p1::initialize(MsgStream &log) {
    // Do not initialize again:
    m_isInitialized=true;

    // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();

    // get DetectorStore service
    StoreGateSvc *detStore;
    StatusCode sc = svcLocator->service("DetectorStore", detStore);
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

    return StatusCode::SUCCESS;
}
