/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PileUpEventInfoCnv.cxx
 * @brief  Implementation file for AthenaPool converter for PileUpEventInfo 
 * @author Marcin.Nowak@cern.ch
 */

#include "PileUpEventInfoCnv.h"


PileUpEventInfo_PERS* PileUpEventInfoCnv::createPersistent(PileUpEventInfo* transObj) {
    MsgStream log(messageService(), "PileUpEventInfoConverter" ); 
    static PileUpEventInfoCnv_p5   TPconverter;
    PileUpEventInfo_PERS *persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
PileUpEventInfo* PileUpEventInfoCnv::createTransient() {
   static pool::Guid   p5_guid("F53A1735-F61A-4CA4-B25F-DB0E277E2DDC");
   static pool::Guid   p4_guid("7D72CE05-E814-4A70-B3B2-B54B05069103");
   static pool::Guid   p3_guid("32A54198-BCDF-0AB5-DC0B-542CAE9866C2");
   static pool::Guid   p2_guid("C2A54FC3-759C-4F01-AF7E-668D9198E10F");
   static pool::Guid   p1_guid("1A399605-8E31-4150-B766-2049FD689485");
   static pool::Guid   p0_guid("6A58157B-6B6E-430F-A72A-EAAA4BDC4AE2");
   if( compareClassGuid(p5_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< PileUpEventInfo_p5 > col_vect( poolReadObject< PileUpEventInfo_p5 >() );
      MsgStream log(messageService(), "PileUpEventInfoConverter" );
      //log << MSG::DEBUG << "Reading PileUpEventInfo_p5" << endreq; 
	  static PileUpEventInfoCnv_p5   TPconverter;
      return TPconverter.createTransient( col_vect.get(), log );
   }	
   else if( compareClassGuid(p4_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< PileUpEventInfo_p4 > col_vect( poolReadObject< PileUpEventInfo_p4 >() );
      MsgStream log(messageService(), "PileUpEventInfoConverter" );
      //log << MSG::DEBUG << "Reading PileUpEventInfo_p4" << endreq; 
	  static PileUpEventInfoCnv_p4   TPconverter;
      return TPconverter.createTransient( col_vect.get(), log );
   }	
  if( compareClassGuid(p3_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< PileUpEventInfo_p3 > col_vect( poolReadObject< PileUpEventInfo_p3 >() );
      MsgStream log(messageService(), "PileUpEventInfoConverter" );
      //log << MSG::DEBUG << "Reading PileUpEventInfo_p3" << endreq; 
      static PileUpEventInfoCnv_p3   TPconverter;
      return TPconverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p2_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< PileUpEventInfo_p2 > col_vect( poolReadObject< PileUpEventInfo_p2 >() );
      MsgStream log(messageService(), "PileUpEventInfoConverter" );
      //log << MSG::DEBUG << "Reading PileUpEventInfo_p2" << endreq; 
	  static PileUpEventInfoCnv_p2   TPconverter;
      return TPconverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< PileUpEventInfo_p1 > col_vect( poolReadObject< PileUpEventInfo_p1 >() );
      MsgStream log(messageService(), "PileUpEventInfoConverter" );
      //log << MSG::DEBUG << "Reading PileUpEventInfo_p1" << endreq; 
	  static PileUpEventInfoCnv_p1   TPconverter;
      return TPconverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it

      //MsgStream log(messageService(), "PileUpEventInfoConverter" );
      //log << MSG::DEBUG << "Reading PileUpEventInfo (original)" << endreq; 
      return poolReadObject< PileUpEventInfo >();
   } 
   throw std::runtime_error("Unsupported persistent version of PileUpEventInfo");
}
