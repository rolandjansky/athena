# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaROOTAccess )

# External dependencies:
find_package( ROOT COMPONENTS PyROOT Core Tree MathCore Hist RIO pthread ROOTTPython)

# Component(s) in the package:
atlas_generate_reflex_dictionary( dict_cpp AthenaROOTAccess
   HEADER  ${CMAKE_CURRENT_SOURCE_DIR}/AthenaROOTAccess/AthenaROOTAccessDict.h
   SELECTION ${CMAKE_CURRENT_SOURCE_DIR}/AthenaROOTAccess/selection.xml
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
)

atlas_add_library( AthenaROOTAccess
                   src/*.cxx ${dict_cpp}
                   PUBLIC_HEADERS AthenaROOTAccess
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthContainersRoot AthenaKernel AthenaPoolCnvSvcLib GaudiKernel SGTools
                   PRIVATE_LINK_LIBRARIES AthAllocators AthContainersInterfaces AthLinks AthenaPoolUtilities CxxUtils PersistentDataModel PersistentDataModelTPCnv RootUtils xAODHIEvent xAODMuon xAODTracking )

add_dependencies( AthenaROOTAccess AthenaROOTAccessDictGen )


atlas_add_dictionary( AthenaROOTAccessTestDict
                      AthenaROOTAccess/AthenaROOTAccessTestDict.h
                      AthenaROOTAccess/test_selection.xml
                      LINK_LIBRARIES AthenaROOTAccess )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests in the package:
atlas_add_test( DVCollectionProxy_test
                SOURCES
                test/DVCollectionProxy_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaROOTAccess )

atlas_add_test( DataBucketVoid_test
                SOURCES
                test/DataBucketVoid_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaROOTAccess )

