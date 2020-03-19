/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimData/InDetSimData.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p2.h"
#include "InDetSimDataCollectionCnv_p2.h"
#include "InDetSimDataCnv_p1.h"
#include "Identifier/Identifier.h"
#include "SGTools/CurrentEventStore.h"
#include "MsgUtil.h"

void InDetSimDataCollectionCnv_p2::transToPers(const InDetSimDataCollection* transCont, InDetSimDataCollection_p2* persCont, MsgStream &log)
{
  InDetSimDataCnv_p1  simDataCnv;
  persCont->m_simdata.resize(transCont->size());
  MSG_DEBUG(log," Preparing " << persCont->m_simdata.size() << "Collections");
  unsigned int collIndex(0);
  for (const auto& transSimDataPair : *transCont) {
    // Add in new collection
    (persCont->m_simdata[collIndex]).first = transSimDataPair.first.get_compact();
    const InDetSimData& simData = transSimDataPair.second;
    InDetSimData_p1& psimData = persCont->m_simdata[collIndex].second;
    simDataCnv.transToPers(&simData,&psimData,log);
    ++collIndex;
  }
  MSG_DEBUG(log," ***  Writing InDetSimdataCollection");
}

void  InDetSimDataCollectionCnv_p2::persToTrans(const InDetSimDataCollection_p2* persCont, InDetSimDataCollection* transCont, MsgStream &log)
{
  InDetSimDataCnv_p1  simDataCnv;
  MSG_DEBUG(log," Preparing " << persCont->m_simdata.size() << "Collections");
  simDataCnv.setCurrentStore (SG::CurrentEventStore::store());
  for (const auto& persSimDataPair : persCont->m_simdata) {
    // Add in new collection
    const InDetSimData_p1& psimData = persSimDataPair.second;
    InDetSimData simData;
    simDataCnv.persToTrans(&psimData,&simData,log);
    transCont->insert( transCont->end(), std :: make_pair( Identifier( persSimDataPair.first ), simData ) );
  }
  MSG_DEBUG(log," ***  Reading InDetSimdataCollection");
}


