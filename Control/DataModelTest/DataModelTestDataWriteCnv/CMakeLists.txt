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

# tag no_merge_componentslist was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_poolcnv_library( DataModelTestDataWriteCnvPoolCnv
                           src/*.cxx
                           FILES DataModelTestDataWrite/BVec.h DataModelTestDataWrite/BDer.h DataModelTestDataWrite/DVec.h DataModelTestDataWrite/DDer.h DataModelTestDataWrite/ELVec.h DataModelTestDataWrite/G.h DataModelTestDataWrite/GVec.h DataModelTestDataWrite/GAuxContainer.h DataModelTestDataWrite/H.h DataModelTestDataWrite/HVec.h DataModelTestDataWrite/HAuxContainer.h DataModelTestDataWrite/HView.h DataModelTestDataCommon/BAux.h DataModelTestDataCommon/BAuxVec.h DataModelTestDataCommon/BAuxStandalone.h DataModelTestDataCommon/C.h DataModelTestDataCommon/CVec.h DataModelTestDataCommon/CAuxContainer.h DataModelTestDataCommon/CView.h DataModelTestDataCommon/CVecWithData.h DataModelTestDataCommon/CInfoAuxContainer.h DataModelTestDataCommon/CTrigAuxContainer.h
                           TYPES_WITH_NAMESPACE DMTest::BVec DMTest::DVec DMTest::DDer DMTest::BDer DMTest::ELVec DMTest::BAux DMTest::BAuxVec DMTest::BAuxStandalone DMTest::C DMTest::CVec DMTest::CAuxContainer DMTest::G DMTest::GVec DMTest::GAuxContainer DMTest::CVecWithData DMTest::CInfoAuxContainer DMTest::CTrigAuxContainer DMTest::CView DMTest::H DMTest::HVec DMTest::HAuxContainer DMTest::HView
                           LINK_LIBRARIES AthenaPoolUtilities DataModelTestDataCommon DataModelTestDataWriteLib AthenaPoolCnvSvcLib )

