/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_TrackAssociationCNV_P1_H
#define JETTAGINFOTPCNV_TrackAssociationCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagEvent/TrackAssociation.h"
#include "JetTagInfoTPCnv/NavAssociationCommon_p1.h"
#include "DataModelAthenaPool/NavigableCnv_p1.h"

class MsgStream;

///
/// Converter to move the track assocation into a real persistent object.
///


namespace Analysis {
  class TrackAssociationCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetAssociationBase, TrackAssociation,  NavAssociationCommon_p1>
    {
    public:
      inline TrackAssociationCnv_p1 (void)
	: m_navAssCnv(0)
	{}

      virtual void persToTrans(const NavAssociationCommon_p1 *persObj,
			       TrackAssociation *transObj,
			       MsgStream &log);
      virtual void transToPers(const TrackAssociation *transObj,
			       NavAssociationCommon_p1 *persObj,
			       MsgStream &log);

    private:

      NavigableCnv_p1<Navigable<Rec::TrackParticleContainer,double>,float> *m_navAssCnv;
    };
}


#endif
