/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ClusterSplitProbabilityContainerCnv_p1_H
#define _ClusterSplitProbabilityContainerCnv_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkEventUtils/ClusterSplitProbabilityContainer.h"
#include "TrkEventTPCnv/TrkClusterSplitProbabilityContainer/ClusterSplitProbabilityContainer_p1.h"
#include "GaudiKernel/MsgStream.h"

namespace Trk {
class ClusterSplitProbabilityContainerCnv_p1 :
   public T_AthenaPoolTPCnvBase<ClusterSplitProbabilityContainer, ClusterSplitProbabilityContainer_p1>
{
public:
  ClusterSplitProbabilityContainerCnv_p1() {}
  virtual void persToTrans(const ClusterSplitProbabilityContainer_p1* persObj,
                           ClusterSplitProbabilityContainer* transObj, MsgStream& log) override;
  virtual void transToPers(const ClusterSplitProbabilityContainer*   transObj,
                           ClusterSplitProbabilityContainer_p1* persObj, MsgStream& log) override;
};
}

template<>
class T_TPCnv<Trk::ClusterSplitProbabilityContainer, Trk::ClusterSplitProbabilityContainer_p1>
   : public Trk::ClusterSplitProbabilityContainerCnv_p1
{
public:
};

#endif

