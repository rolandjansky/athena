/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionCnv.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p1.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p2.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p3.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p4.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p5.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p1.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p2.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p3.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p4.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p5.h"


TrigDecisionCnv::TrigDecisionCnv (ISvcLocator* svcloc)
  : TrigDecisionCnvBase(svcloc)
{}
TrigDecisionCnv::~TrigDecisionCnv () {}


TrigDecision_PERS* TrigDecisionCnv::createPersistent(TrigDec::TrigDecision* transCont)
{
  MsgStream log(messageService(), "TrigDecisionConverter" );

  //  TrigDec::TrigDecisionCnv_p1  converter;
  TrigDec::TrigDecisionCnv_p5  converter;
  TrigDecision_PERS* persObj = converter.createPersistent( transCont, log );

  return persObj;
}


TrigDec::TrigDecision* TrigDecisionCnv::createTransient()
{
  MsgStream log(messageService(), "TrigDecisionConverter" );

  static pool::Guid p1_guid("83D7E08D-CF8C-47A0-963F-5618A9509334");
  static pool::Guid p2_guid("6BE81E51-B42B-4783-957D-84E7F383E9D5");
  static pool::Guid p3_guid("1C0527B6-1B94-4F47-ADE4-D24E7BF91D3E");
  static pool::Guid p4_guid("78F57689-0902-4E47-BB04-9BD10C7EF8B5");
  static pool::Guid p5_guid("DA3DEF39-2401-4A8E-ACD2-851B0DF10AFA");

  if ( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< TrigDec::TrigDecision_p1 > col_vect( poolReadObject< TrigDec::TrigDecision_p1 >() );
    TrigDec::TrigDecisionCnv_p1 converter;
    return converter.createTransient( col_vect.get(), log );
  } else if ( compareClassGuid(p2_guid) ) {
    std::auto_ptr< TrigDec::TrigDecision_p2 > col_vect( poolReadObject< TrigDec::TrigDecision_p2 >() );
    TrigDec::TrigDecisionCnv_p2 converter;
    return converter.createTransient( col_vect.get(), log );
  } else if ( compareClassGuid(p3_guid) ) {
    std::auto_ptr< TrigDec::TrigDecision_p3 > col_vect( poolReadObject< TrigDec::TrigDecision_p3 >() );
    TrigDec::TrigDecisionCnv_p3 converter;
    return converter.createTransient( col_vect.get(), log );
  } else if ( compareClassGuid(p4_guid) ) {
    std::auto_ptr< TrigDec::TrigDecision_p4 > col_vect( poolReadObject< TrigDec::TrigDecision_p4 >() );
    TrigDec::TrigDecisionCnv_p4 converter;
    return converter.createTransient( col_vect.get(), log );
  } else if ( compareClassGuid(p5_guid) ) {
    std::auto_ptr< TrigDec::TrigDecision_p5 > col_vect( poolReadObject< TrigDec::TrigDecision_p5 >() );
    TrigDec::TrigDecisionCnv_p5 converter;
    return converter.createTransient( col_vect.get(), log );
  }
  throw std::runtime_error("Unsupported persistent version of Data Collection");
}
