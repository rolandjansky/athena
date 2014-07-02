/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// T/P translate the JetAssocationBase object (which is an abstract object).
///

#define private public
#define protected public
#include "JetEvent/JetAssociationBase.h"
#undef private
#undef protected

#include "JetEventTPCnv/JetAssociationBaseCnv_p1.h"

///
/// Persitify the object
///
void JetAssociationBaseCnv_p1::persToTrans(const JetAssociationBase_p1* persObj,
					   JetAssociationBase *transObj,
					   MsgStream &/*log*/)
{
  transObj->m_keyIndex = persObj->m_keyIndex;
}
  
void JetAssociationBaseCnv_p1::transToPers(const JetAssociationBase* transObj,
					   JetAssociationBase_p1* persObj,
					   MsgStream &/*log*/)
{
  persObj->m_keyIndex = transObj->m_keyIndex;
}

