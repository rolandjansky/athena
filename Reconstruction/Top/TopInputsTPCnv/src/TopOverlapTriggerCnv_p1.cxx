/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "TopInputsTPCnv/TopOverlapTriggerCnv_p1.h"


void TopOverlapTriggerCnv_p1::transToPers( const TopRec::TopOverlapTrigger* transObj,
					   TopOverlapTrigger_p1* persObj,
					   MsgStream &msg ) 
{
  msg << MSG::DEBUG << typeid(*transObj).name() << " called" << endreq;

  // First, do the base class object
  m_base_cnv.transToPers(transObj, persObj, msg);

  // Now, the trigger flag
  persObj->m_inTrigger = transObj->inTrigger();

  msg << MSG::DEBUG << "Created persistent state of " << typeid(*transObj).name() 
      << endreq;
}


void TopOverlapTriggerCnv_p1::persToTrans( const TopOverlapTrigger_p1* persObj,
				    TopRec::TopOverlapTrigger* transObj,
				    MsgStream &msg ) 
{
  msg << MSG::DEBUG << "Creating transient state of " << typeid(*persObj).name()
      << endreq;

  // First, do the base class object
  m_base_cnv.persToTrans(persObj, transObj, msg);

  // Now, the trigger flag
  transObj->setInTrigger(persObj->m_inTrigger);

  msg << MSG::DEBUG << "Loading " << typeid(*transObj).name() << endreq;
}

