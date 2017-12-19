################################################################################
# Package: AthenaROOTAccess
################################################################################

# Declare the package name:
atlas_subdir( AthenaROOTAccess )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainersInterfaces
                          Control/AthenaKernel
                          Control/SGTools
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthAllocators
                          Control/AthContainers
                          Control/AthContainersRoot
                          Control/AthLinks
                          Control/RootUtils
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/PersistentDataModel
                          Database/PersistentDataModelTPCnv
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking )

# External dependencies:
find_package( ROOT COMPONENTS PyROOT Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_generate_reflex_dictionary( dict_cpp AthenaROOTAccess
   HEADER  ${CMAKE_CURRENT_SOURCE_DIR}/AthenaROOTAccess/AthenaROOTAccessDict.h
   SELECTION ${CMAKE_CURRENT_SOURCE_DIR}/AthenaROOTAccess/selection.xml
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
)

atlas_add_library( AthenaROOTAccess
                   src/*.cxx ${dict_cpp}
                   PUBLIC_HEADERS AthenaROOTAccess
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel SGTools GaudiKernel RootUtilsPyROOT AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthAllocators AthContainers AthContainersRoot AthLinks RootUtils PersistentDataModel PersistentDataModelTPCnv xAODHIEvent xAODMuon xAODTracking )

add_dependencies( AthenaROOTAccess AthenaROOTAccessDictGen )


atlas_add_dictionary( AthenaROOTAccessTestDict
                      AthenaROOTAccess/AthenaROOTAccessTestDict.h
                      AthenaROOTAccess/test_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel SGTools GaudiKernel TestTools AthAllocators AthContainers AthContainersRoot AthLinks RootUtils RootUtilsPyROOT AthenaPoolCnvSvcLib PersistentDataModel PersistentDataModelTPCnv xAODHIEvent xAODMuon xAODTracking AthenaROOTAccess )

atlas_add_test( DVCollectionProxy_test
                SOURCES
                test/DVCollectionProxy_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel SGTools GaudiKernel TestTools AthAllocators AthContainers AthContainersRoot AthLinks RootUtils RootUtilsPyROOT AthenaPoolCnvSvcLib PersistentDataModel PersistentDataModelTPCnv xAODHIEvent xAODMuon xAODTracking AthenaROOTAccess )

atlas_add_test( DataBucketVoid_test
                SOURCES
                test/DataBucketVoid_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel SGTools GaudiKernel TestTools AthAllocators AthContainers AthContainersRoot AthLinks RootUtils RootUtilsPyROOT AthenaPoolCnvSvcLib PersistentDataModel PersistentDataModelTPCnv xAODHIEvent xAODMuon xAODTracking AthenaROOTAccess )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
