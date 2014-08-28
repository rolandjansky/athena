/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// TopOverlapTriggerCnv_p1 - class to do T<->P conversion for TopOverlapTrigger

#ifndef TOPINPUTSTPCNV_TOPOVERLAPTRIGGERCNV_P1_H
#define TOPINPUTSTPCNV_TOPOVERLAPTRIGGERCNV_P1_H

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TopInputs/TopOverlapTrigger.h"
#include "TopInputsTPCnv/TopOverlapTrigger_p1.h"
#include "TopInputsTPCnv/TopOverlapCnv_p1.h"

class TopOverlapTriggerCnv_p1 : 
  public T_AthenaPoolTPCnvBase< TopRec::TopOverlapTrigger, TopOverlapTrigger_p1 >
{
  
public:

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
  
  TopOverlapTriggerCnv_p1() {;}
  virtual ~TopOverlapTriggerCnv_p1() {;}

  /////////////////////////////////////////////////////////////////// 
  // Conversion methods: 
  ///////////////////////////////////////////////////////////////////
  
  virtual void persToTrans( const TopOverlapTrigger_p1* persObj,
			    TopRec::TopOverlapTrigger* transObj,
			    MsgStream &msg );

  virtual void transToPers( const TopRec::TopOverlapTrigger* transObj,
			    TopOverlapTrigger_p1* persObj,
			    MsgStream &msg);
  
private:

  TopOverlapCnv_p1 m_base_cnv;
  
};

#endif // not TOPINPUTSTPCNV_TOPOVERLAPTRIGGERCNV_P1_H
