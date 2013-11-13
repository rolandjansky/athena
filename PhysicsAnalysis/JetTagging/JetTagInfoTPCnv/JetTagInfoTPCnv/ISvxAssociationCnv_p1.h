/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_ISvxAssociationCNV_P1_H
#define JETTAGINFOTPCNV_ISvxAssociationCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/ISvxAssociation_p1.h"
#include "JetTagEvent/ISvxAssociation.h"

class MsgStream;

///
/// Converter to move the track assocation into a real persistent object.
///


namespace Analysis {
  class ISvxAssociationCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetAssociationBase, ISvxAssociation, ISvxAssociation_p1>
    {
    public:
      inline ISvxAssociationCnv_p1 (void)
	{}

      virtual void persToTrans(const ISvxAssociation_p1 *persObj,
			       ISvxAssociation *transObj,
			       MsgStream &log);
      virtual void transToPers(const ISvxAssociation *persObj,
			       ISvxAssociation_p1 *transObj,
			       MsgStream &log);

    private:
    };
}


#endif
