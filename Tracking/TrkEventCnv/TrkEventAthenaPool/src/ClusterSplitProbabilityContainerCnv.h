/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _Trk_ClusterSplitProbabilityContainerCnv_h_
#define _Trk_ClusterSplitProbabilityContainerCnv_h_

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrkEventUtils/ClusterSplitProbabilityContainer.h"

#include "TrkEventTPCnv/TrkClusterSplitProbabilityContainer/ClusterSplitProbabilityContainer_p1.h"
#include "TrkEventTPCnv/TrkClusterSplitProbabilityContainer/ClusterSplitProbabilityContainerCnv_p1.h"


using ClusterSplitProbabilityContainer_PERS = Trk::ClusterSplitProbabilityContainer_p1;
using ClusterSplitProbabilityContainerCnvBase = T_AthenaPoolCustomCnv<Trk::ClusterSplitProbabilityContainer, ClusterSplitProbabilityContainer_PERS>;

/** TP converter for the ClusterSplitProbabiltyContainer
 */
class ClusterSplitProbabilityContainerCnv
   : public ClusterSplitProbabilityContainerCnvBase
{
friend class CnvFactory<ClusterSplitProbabilityContainerCnv>;

public:
  ClusterSplitProbabilityContainerCnv( ISvcLocator *svcloc );

protected:
  virtual ClusterSplitProbabilityContainer_PERS *createPersistent( Trk::ClusterSplitProbabilityContainer *transCont);
  virtual Trk::ClusterSplitProbabilityContainer      *createTransient();

  Trk::ClusterSplitProbabilityContainerCnv_p1 m_converter;
  static const std::string s_name;
};
#endif // _Trk_ClusterSplitProbabilityContainerCnv_h_
