/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Write out a MuonAssociation
///

#define private public
#define protected public
#include "MuonIDEvent/MuonAssociation.h"
#undef private
#undef protected

#include "JetTagInfoTPCnv/MuonAssociationCnv_p1.h"

namespace Analysis {
  ///
  /// Convert to trans form
  ///
  void MuonAssociationCnv_p1::persToTrans(const NavAssociationCommon_p1 *persObj,
					   MuonAssociation *transObj,
					   MsgStream &log)
  {
    transObj->m_keyIndex = persObj->m_keyIndex;
    fillTransFromPStore(&m_navAssCnv,
			persObj->m_navBase,
			transObj,
			log);
  }

  void MuonAssociationCnv_p1::transToPers(const MuonAssociation *transObj,
					   NavAssociationCommon_p1 *persObj,
					   MsgStream &log)
  {
    persObj->m_keyIndex = transObj->m_keyIndex;
    persObj->m_navBase = baseToPersistent(&m_navAssCnv,
					  transObj,
					  log);
  }
}


