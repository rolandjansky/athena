/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Write out a ISvxAssociation
///

#include "JetTagEvent/ISvxAssociation.h"
#include "JetTagInfoTPCnv/ISvxAssociationCnv_p1.h"

namespace Analysis {
  ///
  /// Convert to trans form
  ///
  void ISvxAssociationCnv_p1::persToTrans(const ISvxAssociation_p1 *persObj,
					   ISvxAssociation *transObj,
					   MsgStream &)
  {
    transObj->m_keyIndex = persObj->m_keyIndex;
  }

  void ISvxAssociationCnv_p1::transToPers(const ISvxAssociation *transObj,
					   ISvxAssociation_p1 *persObj,
					   MsgStream &)
  {
    persObj->m_keyIndex = transObj->keyIndex();
  }
}


