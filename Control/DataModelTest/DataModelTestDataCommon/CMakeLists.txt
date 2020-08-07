# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataModelTestDataCommon )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DataModelTestDataCommonLib
                   src/*.cxx
                   PUBLIC_HEADERS DataModelTestDataCommon
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthContainersInterfaces AthenaPoolUtilities AthLinks AthenaKernel EventInfo GaudiKernel RegistrationServicesLib xAODCore xAODTrigger
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps CxxUtils PersistentDataModel StoreGateLib )

atlas_add_component( DataModelTestDataCommon
                     src/components/*.cxx
                     LINK_LIBRARIES DataModelTestDataCommonLib )

atlas_add_dictionary( DataModelTestDataCommonDict
                      DataModelTestDataCommon/DataModelTestDataCommonDict.h
                      DataModelTestDataCommon/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES DataModelTestDataCommonLib
                      NO_ROOTMAP_MERGE
                      EXTRA_FILES src/dict/*.cxx
                      ELEMENT_LINKS DMTest::BAuxVec DMTest::CVec_v1 )

atlas_add_sercnv_library ( DataModelTestDataCommonSerCnv
                           FILES DataModelTestDataCommon/CVec.h DataModelTestDataCommon/CView.h DataModelTestDataCommon/CAuxContainer.h
                           TYPES_WITH_NAMESPACE DMTest::CVec DMTest::CView DMTest::CAuxContainer
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelTestDataCommonLib TrigSerializeCnvSvcLib )

# Install files from the package:
atlas_install_scripts( scripts/*.py )
