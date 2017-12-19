/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimData/CscSimData.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollectionCnv_p2.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollection_p2.h"
#include "Identifier/Identifier.h"

void CscSimDataCollectionCnv_p2::persToTrans(const Muon::CscSimDataCollection_p2* persCol, CscSimDataCollection* transCol, MsgStream &log) {
   log << MSG::DEBUG << "CscSimDataCollectionCnv_p2::persToTrans" << endmsg;

   std::vector<std::pair<Identifier32::value_type, Muon::CscSimData_p2 > >::const_iterator it_Coll = persCol->m_cscsimdata.begin();
   std::vector<std::pair<Identifier32::value_type, Muon::CscSimData_p2 > >::const_iterator it_CollEnd = persCol->m_cscsimdata.end();
   log << MSG::DEBUG << " Preparing " << persCol->m_cscsimdata.size() << " Collections" << endmsg;

   for(int collIdx=0 ; it_Coll != it_CollEnd; it_Coll++, collIdx++)  {
      const Muon::CscSimData_p2& perssimData = persCol->m_cscsimdata[collIdx].second;
      CscSimData transsimData;
      m_cscsimdataCnv.persToTrans(&perssimData,&transsimData,log);
      transCol->insert( transCol->end(), std::make_pair( Identifier( it_Coll->first ), transsimData ) );
   }

   log << MSG::DEBUG << " Reading CscSimDataCollection" << endmsg;

}

void CscSimDataCollectionCnv_p2::transToPers(const CscSimDataCollection*, Muon::CscSimDataCollection_p2*, MsgStream &/*log*/) {
  throw std::runtime_error("CscSimDataCollectionCnv_p2::transToPers is not supported in this release!");
}
