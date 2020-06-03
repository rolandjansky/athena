/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JetElementCNV_P1_H
#define JetElementCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JetElement.h"

// Local include(s):
#include "TrigT1EventTPCnv/JetElement_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JetElement and JetElement_p1
 *
 *          Simple converter that can create JetElement objects from
 *          JetElement_p1 ones, and vice versa. It is used in JetElementCnv
 *          to do the T/P conversion before/after writing/reading the JetElement
 *          object.
 */
class JetElementCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::JetElement, JetElement_p1 > {

public:
  JetElementCnv_p1() : m_JetElementCnv(0) {}

  virtual void persToTrans( const JetElement_p1* persObj, LVL1::JetElement* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::JetElement* transObj, JetElement_p1* persObj, MsgStream &log );

protected:
  JetElementCnv_p1 *m_JetElementCnv ;

}; // class JetElementCnv_p1

#endif // JetElementCNV_P1_H
