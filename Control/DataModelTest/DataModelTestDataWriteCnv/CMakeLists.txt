################################################################################
# Package: DataModelTestDataWriteCnv
################################################################################

# Declare the package name:
atlas_subdir( DataModelTestDataWriteCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Control/DataModelTest/DataModelTestDataCommon
                          Control/DataModelTest/DataModelTestDataWrite
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( DataModelTestDataWriteCnv
                   DataModelTestDataWriteCnv/CLinksAOD_p1.h
                   INTERFACE
                   PUBLIC_HEADERS DataModelTestDataWriteCnv
                   LINK_LIBRARIES DataModelAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( DataModelTestDataWriteCnvPoolCnv
                           src/*.cxx
                           FILES DataModelTestDataWrite/BVec.h DataModelTestDataWrite/BDer.h DataModelTestDataWrite/DVec.h DataModelTestDataWrite/DDer.h DataModelTestDataWrite/ELVec.h DataModelTestDataWrite/G.h DataModelTestDataWrite/GVec.h DataModelTestDataWrite/GAuxContainer.h DataModelTestDataWrite/H.h DataModelTestDataWrite/HVec.h DataModelTestDataWrite/HAuxContainer.h DataModelTestDataWrite/HView.h DataModelTestDataCommon/BAux.h DataModelTestDataCommon/BAuxVec.h DataModelTestDataCommon/BAuxStandalone.h DataModelTestDataCommon/C.h DataModelTestDataCommon/CVec.h DataModelTestDataCommon/CAuxContainer.h DataModelTestDataCommon/CView.h DataModelTestDataCommon/CVecWithData.h DataModelTestDataCommon/CInfoAuxContainer.h DataModelTestDataCommon/CTrigAuxContainer.h DataModelTestDataCommon/S1.h DataModelTestDataCommon/S2.h DataModelTestDataCommon/CLinks.h DataModelTestDataCommon/CLinksContainer.h DataModelTestDataCommon/CLinksAuxInfo.h DataModelTestDataCommon/CLinksAuxContainer.h DataModelTestDataCommon/CLinksAOD.h
                           TYPES_WITH_NAMESPACE DMTest::BVec DMTest::DVec DMTest::DDer DMTest::BDer DMTest::ELVec DMTest::BAux DMTest::BAuxVec DMTest::BAuxStandalone DMTest::C DMTest::CVec DMTest::CAuxContainer DMTest::G DMTest::GVec DMTest::GAuxContainer DMTest::CVecWithData DMTest::CInfoAuxContainer DMTest::CTrigAuxContainer DMTest::CView DMTest::H DMTest::HVec DMTest::HAuxContainer DMTest::HView DMTest::S1 DMTest::S2 DMTest::CLinks DMTest::CLinksContainer DMTest::CLinksAuxInfo DMTest::CLinksAuxContainer DMTest::CLinksAOD
                           LINK_LIBRARIES AthenaPoolUtilities DataModelTestDataCommonLib DataModelTestDataWriteLib AthenaPoolCnvSvcLib )


atlas_add_dictionary( DataModelTestDataWriteCnvDict
                      DataModelTestDataWriteCnv/DataModelTestDataWriteCnvDict.h
                      DataModelTestDataWriteCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelTestDataCommonLib
                      NO_ROOTMAP_MERGE )

