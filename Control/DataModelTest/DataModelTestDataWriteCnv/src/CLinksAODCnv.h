// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataWriteCnv/src/CLinksAODCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief POOL converter for CLinksAOD.
 */


#ifndef DATAMODELTESTDATAWRITECNV_CLINKSAODCNV_H
#define DATAMODELTESTDATAWRITECNV_CLINKSAODCNV_H


#include "CLinksAODCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"


typedef T_AthenaPoolTPCnvCnv<DMTest::CLinksAOD, DMTest::CLinksAODCnv_p1> CLinksAODCnv;


#endif // not DATAMODELTESTDATAWRITECNV_CLINKSAODCNV_H
