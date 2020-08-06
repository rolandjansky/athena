# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataModelTestDataWriteCnv )

atlas_add_library( DataModelTestDataWriteCnv
                   DataModelTestDataWriteCnv/CLinksAOD_p1.h
                   INTERFACE
                   PUBLIC_HEADERS DataModelTestDataWriteCnv
                   LINK_LIBRARIES DataModelAthenaPoolLib )

# Component(s) in the package:
atlas_add_poolcnv_library( DataModelTestDataWriteCnvPoolCnv
                           src/*.cxx
                           FILES DataModelTestDataWrite/BVec.h DataModelTestDataWrite/BDer.h DataModelTestDataWrite/DVec.h DataModelTestDataWrite/DDer.h DataModelTestDataWrite/ELVec.h DataModelTestDataWrite/G.h DataModelTestDataWrite/GVec.h DataModelTestDataWrite/GAuxContainer.h DataModelTestDataWrite/H.h DataModelTestDataWrite/HVec.h DataModelTestDataWrite/HAuxContainer.h DataModelTestDataWrite/HView.h DataModelTestDataCommon/BAux.h DataModelTestDataCommon/BAuxVec.h DataModelTestDataCommon/BAuxStandalone.h DataModelTestDataCommon/C.h DataModelTestDataCommon/CVec.h DataModelTestDataCommon/CAuxContainer.h DataModelTestDataCommon/CView.h DataModelTestDataCommon/CVecWithData.h DataModelTestDataCommon/CInfoAuxContainer.h DataModelTestDataCommon/CTrigAuxContainer.h DataModelTestDataCommon/S1.h DataModelTestDataCommon/S2.h DataModelTestDataCommon/CLinks.h DataModelTestDataCommon/CLinksContainer.h DataModelTestDataCommon/CLinksAuxInfo.h DataModelTestDataCommon/CLinksAuxContainer.h DataModelTestDataCommon/CLinksAOD.h
                           TYPES_WITH_NAMESPACE DMTest::BVec DMTest::DVec DMTest::DDer DMTest::BDer DMTest::ELVec DMTest::BAux DMTest::BAuxVec DMTest::BAuxStandalone DMTest::C DMTest::CVec DMTest::CAuxContainer DMTest::G DMTest::GVec DMTest::GAuxContainer DMTest::CVecWithData DMTest::CInfoAuxContainer DMTest::CTrigAuxContainer DMTest::CView DMTest::H DMTest::HVec DMTest::HAuxContainer DMTest::HView DMTest::S1 DMTest::S2 DMTest::CLinks DMTest::CLinksContainer DMTest::CLinksAuxInfo DMTest::CLinksAuxContainer DMTest::CLinksAOD
                           LINK_LIBRARIES AthenaPoolUtilities DataModelTestDataCommonLib DataModelTestDataWriteLib AthenaPoolCnvSvcLib DataModelTestDataWriteCnv )

atlas_add_dictionary( DataModelTestDataWriteCnvDict
                      DataModelTestDataWriteCnv/DataModelTestDataWriteCnvDict.h
                      DataModelTestDataWriteCnv/selection.xml
                      LINK_LIBRARIES DataModelTestDataCommonLib DataModelTestDataWriteCnv
                      NO_ROOTMAP_MERGE )

