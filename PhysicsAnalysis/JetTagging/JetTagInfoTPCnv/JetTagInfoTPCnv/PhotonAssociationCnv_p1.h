/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_PhotonAssociationCNV_P1_H
#define JETTAGINFOTPCNV_PhotonAssociationCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/NavAssociationCommon_p1.h"
#include "DataModelAthenaPool/NavigableCnv_p1.h"
#include "egammaEvent/PhotonAssociation.h"

class MsgStream;

///
/// Converter to move the track assocation into a real persistent object.
///


namespace Analysis {
  class PhotonAssociationCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetAssociationBase, PhotonAssociation, NavAssociationCommon_p1>
    {
    public:
      inline PhotonAssociationCnv_p1 (void)
	: m_navAssCnv(0)
	{}

      virtual void persToTrans(const NavAssociationCommon_p1 *persObj,
			       PhotonAssociation *transObj,
			       MsgStream &log);
      virtual void transToPers(const PhotonAssociation *persObj,
			       NavAssociationCommon_p1 *transObj,
			       MsgStream &log);

    private:
      NavigableCnv_p1<Navigable<PhotonContainer,double>,float> *m_navAssCnv;
    };
}


#endif
