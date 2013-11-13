/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_ElectronAssociationCNV_P1_H
#define JETTAGINFOTPCNV_ElectronAssociationCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/NavAssociationCommon_p1.h"
#include "DataModelAthenaPool/NavigableCnv_p1.h"
#include "egammaEvent/ElectronAssociation.h"

class MsgStream;

///
/// Converter to move the track assocation into a real persistent object.
///


namespace Analysis {
  class ElectronAssociationCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetAssociationBase, ElectronAssociation, NavAssociationCommon_p1>
    {
    public:
      inline ElectronAssociationCnv_p1 (void)
	: m_navAssCnv(0)
	{}

      virtual void persToTrans(const NavAssociationCommon_p1 *persObj,
			       ElectronAssociation *transObj,
			       MsgStream &log);
      virtual void transToPers(const ElectronAssociation *persObj,
			       NavAssociationCommon_p1 *transObj,
			       MsgStream &log);

    private:
      NavigableCnv_p1<Navigable<ElectronContainer,double>,float> *m_navAssCnv;
    };
}


#endif
