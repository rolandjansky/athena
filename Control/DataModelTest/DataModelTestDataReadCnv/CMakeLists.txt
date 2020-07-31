# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataModelTestDataReadCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( DataModelTestDataReadCnvPoolCnv
                           src/*.cxx
                           FILES DataModelTestDataRead/BVec.h DataModelTestDataRead/BDer.h DataModelTestDataRead/DVec.h DataModelTestDataRead/DDer.h DataModelTestDataRead/ELVec.h DataModelTestDataRead/G.h DataModelTestDataRead/GVec.h DataModelTestDataRead/GAuxContainer.h DataModelTestDataRead/H.h DataModelTestDataRead/HVec.h DataModelTestDataRead/HAuxContainer.h DataModelTestDataRead/HView.h DataModelTestDataCommon/BAux.h DataModelTestDataCommon/BAuxVec.h DataModelTestDataCommon/BAuxStandalone.h DataModelTestDataCommon/C.h DataModelTestDataCommon/CVec.h DataModelTestDataCommon/CAuxContainer.h DataModelTestDataCommon/CView.h DataModelTestDataCommon/CVecWithData.h DataModelTestDataCommon/CInfoAuxContainer.h DataModelTestDataCommon/CTrigAuxContainer.h DataModelTestDataCommon/S1.h DataModelTestDataCommon/S2.h
                           TYPES_WITH_NAMESPACE DMTest::BVec DMTest::DVec DMTest::DDer DMTest::BDer DMTest::ELVec DMTest::BAux DMTest::BAuxVec DMTest::BAuxStandalone DMTest::C DMTest::CVec DMTest::CAuxContainer DMTest::G DMTest::GVec DMTest::GAuxContainer DMTest::CVecWithData DMTest::CInfoAuxContainer DMTest::CTrigAuxContainer DMTest::CView DMTest::H DMTest::HVec DMTest::HAuxContainer DMTest::HView DMTest::S1 DMTest::S2
                           LINK_LIBRARIES AthenaPoolUtilities AthenaKernel DataModelTestDataCommonLib DataModelTestDataReadLib AthenaPoolCnvSvcLib )

