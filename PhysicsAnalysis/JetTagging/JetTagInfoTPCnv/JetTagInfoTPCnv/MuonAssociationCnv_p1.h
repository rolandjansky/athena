/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_MuonAssociationCNV_P1_H
#define JETTAGINFOTPCNV_MuonAssociationCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/NavAssociationCommon_p1.h"
#include "MuonIDEvent/MuonAssociation.h"
#include "DataModelAthenaPool/NavigableCnv_p1.h"

class MsgStream;

///
/// Converter to move the track assocation into a real persistent object.
///


namespace Analysis {
  class MuonAssociationCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetAssociationBase, MuonAssociation, NavAssociationCommon_p1>
    {
    public:
      inline MuonAssociationCnv_p1 (void)
	: m_navAssCnv(0)
	{}

      virtual void persToTrans(const NavAssociationCommon_p1 *persObj,
			       MuonAssociation *transObj,
			       MsgStream &log);
      virtual void transToPers(const MuonAssociation *persObj,
			       NavAssociationCommon_p1 *transObj,
			       MsgStream &log);
    private:
      NavigableCnv_p1<Navigable<MuonContainer,double>,float> *m_navAssCnv;
    };
}


#endif
