/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Write out a PhotonAssociation
///

#include "egammaEvent/PhotonAssociation.h"
#include "JetTagInfoTPCnv/PhotonAssociationCnv_p1.h"

namespace Analysis {
  ///
  /// Convert to trans form
  ///
  void PhotonAssociationCnv_p1::persToTrans(const NavAssociationCommon_p1 *persObj,
					   PhotonAssociation *transObj,
					   MsgStream &log)
  {
    transObj->m_keyIndex = persObj->m_keyIndex;
    fillTransFromPStore(&m_navAssCnv,
			persObj->m_navBase,
			transObj,
			log);
  }

  void PhotonAssociationCnv_p1::transToPers(const PhotonAssociation *transObj,
					   NavAssociationCommon_p1 *persObj,
					   MsgStream &log)
  {
    persObj->m_keyIndex = transObj->keyIndex();
    persObj->m_navBase = baseToPersistent(&m_navAssCnv,
					  transObj,
					  log);
  }
}


