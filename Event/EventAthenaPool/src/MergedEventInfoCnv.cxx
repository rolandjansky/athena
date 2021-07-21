/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MergedEventInfoCnv.cxx
 * @brief  Implementation file for AthenaPool converter for MergedEventInfo 
 * @author Marcin.Nowak@cern.ch
 */

#include "MergedEventInfoCnv.h"

MergedEventInfo_PERS* MergedEventInfoCnv::createPersistent(MergedEventInfo* transObj) {
    MsgStream log(msgSvc(), "MergedEventInfoConverter" ); 
    static const MergedEventInfoCnv_p2  TPconv;
    MergedEventInfo_PERS *persObj = TPconv.createPersistentConst( transObj, log );
    //log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}
    
MergedEventInfo* MergedEventInfoCnv::createTransient() {
   static const pool::Guid   p2_guid("5B0C1A92-6836-4B3C-9EE6-5E69792A28B5");
   static const pool::Guid   p1_guid("9540DED6-51E8-48A3-8F86-05CB1D9CC812");
   static const pool::Guid   p0_guid("FA93B80D-82C7-4096-8A04-62885A679A6F");
   if( compareClassGuid(p2_guid) ) {
      std::unique_ptr< MergedEventInfo_p2 > col_vect( poolReadObject< MergedEventInfo_p2 >() );
      MsgStream log(msgSvc(), "MergedEventInfoConverter" );
      //log << MSG::DEBUG << "Reading MergedEventInfo_p2" << endmsg; 
      static const MergedEventInfoCnv_p2   TPconverter;
      return TPconverter.createTransientConst( col_vect.get(), log );
   } else 
   if( compareClassGuid(p1_guid) ) {
      std::unique_ptr< MergedEventInfo_p1 > col_vect( poolReadObject< MergedEventInfo_p1 >() );
      MsgStream log(msgSvc(), "MergedEventInfoConverter" );
      //log << MSG::DEBUG << "Reading MergedEventInfo_p1" << endmsg; 
      static const MergedEventInfoCnv_p1   TPconverter;
      return TPconverter.createTransientConst( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      //MsgStream log(msgSvc(), "MergedEventInfoConverter" );
      //log << MSG::DEBUG << "Reading MergedEventInfo (original)" << endmsg; 
      return poolReadObject< MergedEventInfo >();
   } 
   throw std::runtime_error("Unsupported persistent version of MergedEventInfo");
}
