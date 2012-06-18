/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "egammaEventTPCnv/ElectronContainerCnv_p1.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p2.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p3.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p4.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p5.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p1.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p2.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p3.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p4.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p5.h"
#include "egammaEventTPCnv/egDetailContainerCnv_p1.h"
#include "egammaEventTPCnv/egDetailContainerCnv_p2.h"
#include "egammaEventTPCnv/egammaContainerCnv_p1.h"
#include "egammaEventTPCnv/egammaContainerCnv_p2.h"
#include "egammaEventTPCnv/egammaContainerCnv_p3.h"
#include "egammaEventTPCnv/egammaContainerCnv_p4.h"
#include "egammaEventTPCnv/egammaContainerCnv_p5.h"
#include "egammaEventTPCnv/CaloRingsContainerCnv_p1.h"

DECLARE_TPCNV_FACTORY(ElectronContainerCnv_p1,
                      ElectronContainer,
                      ElectronContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(ElectronContainerCnv_p2,
                      ElectronContainer,
                      ElectronContainer_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(ElectronContainerCnv_p3,
                      ElectronContainer,
                      ElectronContainer_p3,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(ElectronContainerCnv_p4,
                      ElectronContainer,
                      ElectronContainer_p4,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(ElectronContainerCnv_p5,
                      ElectronContainer,
                      ElectronContainer_p5,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(PhotonContainerCnv_p1,
                      PhotonContainer,
                      PhotonContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(PhotonContainerCnv_p2,
                      PhotonContainer,
                      PhotonContainer_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(PhotonContainerCnv_p3,
                      PhotonContainer,
                      PhotonContainer_p3,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(PhotonContainerCnv_p4,
                      PhotonContainer,
                      PhotonContainer_p4,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(PhotonContainerCnv_p5,
                      PhotonContainer,
                      PhotonContainer_p5,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(egDetailContainerCnv_p1,
                      egDetailContainer,
                      egDetailContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(egDetailContainerCnv_p2,
                      egDetailContainer,
                      egDetailContainer_p2,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(egammaContainerCnv_p1,
                      egammaContainer,
                      egammaContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(egammaContainerCnv_p2,
                      egammaContainer,
                      egammaContainer_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(egammaContainerCnv_p3,
                      egammaContainer,
                      egammaContainer_p3,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(egammaContainerCnv_p4,
                      egammaContainer,
                      egammaContainer_p4,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(egammaContainerCnv_p5,
                      egammaContainer,
                      egammaContainer_p5,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(CaloRingsContainerCnv_p1,
                      CaloRingsContainer,
                      CaloRingsContainer_p1,
                      Athena::TPCnvVers::Current)

