/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries

#include "AthenaKernel/TPCnvFactory.h"

#include "ISF_FatrasEventTPCnv/PlanarClusterContainer_p1.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainerCnv_p1.h"

#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarCluster_p1.h"

#include "ISF_FatrasEventTPCnv/PlanarClusterContainer_p2.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainerCnv_p2.h"

#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarCluster_p2.h"
using namespace iFatras;

DECLARE_TPCNV_FACTORY(PlanarClusterContainerCnv_p1,
		      iFatras::PlanarClusterContainer,
		      iFatras::PlanarClusterContainer_p1,
		      Athena::TPCnvVers::Old)
		      
DECLARE_TPCNV_FACTORY(PlanarClusterContainerCnv_p2,
		      iFatras::PlanarClusterContainer,
		      iFatras::PlanarClusterContainer_p2,
		      Athena::TPCnvVers::Current)
