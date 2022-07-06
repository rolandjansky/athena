/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "IDCInDetBSErrContainerCnv_p1.h"
#include "InDetBSErrContainerCnv_p1.h"
#include "InDetBSErrContainerCnv_p2.h"
#include "TRT_BSErrContainerCnv_p1.h"
#include "TRT_BSErrContainerCnv_p2.h"
#include "TRT_BSIdErrContainerCnv_p1.h"

DECLARE_TPCNV_FACTORY(IDCInDetBSErrContainerCnv_p1,
                      IDCInDetBSErrContainer,
                      InDetBSErrContainer64_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(InDetBSErrContainerCnv_p1,
                      InDetBSErrContainer,
                      InDetBSErrContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(InDetBSErrContainerCnv_p2,
                      InDetBSErrContainer,
                      InDetBSErrContainer_p2,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TRT_BSErrContainerCnv_p1,
                      TRT_BSErrContainer,
                      TRT_BSErrContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TRT_BSErrContainerCnv_p2,
                      TRT_BSErrContainer,
                      TRT_BSErrContainer_p2,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TRT_BSIdErrContainerCnv_p1,
                      TRT_BSIdErrContainer,
                      TRT_BSIdErrContainer_p1,
                      Athena::TPCnvVers::Current)

