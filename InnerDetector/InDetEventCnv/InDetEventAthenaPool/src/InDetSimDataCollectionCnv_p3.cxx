/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimData/InDetSimData.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p3.h"
#include "InDetSimDataCollectionCnv_p3.h"
#include "InDetSimDataCnv_p2.h"
#include "Identifier/Identifier.h"
#include "SGTools/CurrentEventStore.h"
#include "MsgUtil.h"

void InDetSimDataCollectionCnv_p3::transToPers(const InDetSimDataCollection* transCont, InDetSimDataCollection_p3* persCont, MsgStream &log)
{


    typedef InDetSimDataCollection TRANS;

    InDetSimDataCnv_p2  simDataCnv;

    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    persCont->m_simdata.resize(transCont->size());
    MSG_DEBUG(log," Preparing " << persCont->m_simdata.size() << "Collections");
    for (int collIndex=0 ; it_Coll != it_CollEnd; it_Coll++, collIndex++)  {
        // Add in new collection
        (persCont->m_simdata[collIndex]).first = (*it_Coll).first.get_compact();
        const InDetSimData& simData = (*it_Coll).second;
        InDetSimData_p2& psimData = persCont->m_simdata[collIndex].second;
        simDataCnv.transToPers(&simData,&psimData,log);
    }
    MSG_DEBUG(log," ***  Writing InDetSimdataCollection");
}

void  InDetSimDataCollectionCnv_p3::persToTrans(const InDetSimDataCollection_p3* persCont, InDetSimDataCollection* transCont, MsgStream &log)
{

    typedef std::vector<std::pair<Identifier::value_type, InDetSimData_p2 > >::const_iterator COLLITER;
    InDetSimDataCnv_p2  simDataCnv;

    COLLITER it_Coll     = persCont->m_simdata.begin();
    COLLITER it_CollEnd  = persCont->m_simdata.end();
    MSG_DEBUG(log," Preparing " << persCont->m_simdata.size() << "Collections");
    simDataCnv.setCurrentStore (SG::CurrentEventStore::store());
    for (int collIndex=0 ; it_Coll != it_CollEnd; it_Coll++, collIndex++)  {
        // Add in new collection
        const InDetSimData_p2& psimData = persCont->m_simdata[collIndex].second;
        InDetSimData simData;
        simDataCnv.persToTrans(&psimData,&simData,log);
        transCont->insert( transCont->end(), std :: make_pair( Identifier( it_Coll->first ), simData ) );
    }
    MSG_DEBUG(log," ***  Reading InDetSimdataCollection");

}


