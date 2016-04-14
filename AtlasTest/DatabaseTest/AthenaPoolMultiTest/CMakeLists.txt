################################################################################
# Package: AthenaPoolMultiTest
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolMultiTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/MinimalRunTime
                          Database/AthenaPOOL/AthenaPoolExample/AthenaPoolExampleAlgorithms
                          PRIVATE
                          AtlasTest/DatabaseTest/AthenaPoolTestData
                          Control/AthenaBaseComps
                          Control/SGTools
                          Control/StoreGate
                          DataQuality/GoodRunsLists
                          Database/APR/CollectionBase
                          Database/APR/CollectionUtilities
                          Database/APR/POOLCore
                          Database/APR/PersistencySvc
                          Database/APR/StorageSvc
                          Database/AthenaPOOL/AthenaPoolExample/AthenaPoolExampleData
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/DBDataModel
                          Event/ByteStreamCnvSvc
                          Event/ByteStreamData
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          TestPolicy )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( AthenaPoolMultiTestLib
                   src/*.cxx
                   PUBLIC_HEADERS AthenaPoolMultiTest
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES StoreGateLib SGtests GoodRunsListsLib ByteStreamData_test
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaPoolTestData AthenaBaseComps SGTools CollectionBase CollectionUtilities POOLCore PersistencySvc StorageSvc AthenaPoolExampleData AthenaPoolUtilities DBDataModel ByteStreamData EventInfo xAODEventInfo GaudiKernel )

atlas_add_component( AthenaPoolMultiTest
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaPoolTestData AthenaBaseComps SGTools StoreGateLib SGtests GoodRunsListsLib CollectionBase CollectionUtilities POOLCore PersistencySvc StorageSvc AthenaPoolExampleData AthenaPoolUtilities DBDataModel ByteStreamData ByteStreamData_test EventInfo xAODEventInfo GaudiKernel AthenaPoolMultiTestLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

