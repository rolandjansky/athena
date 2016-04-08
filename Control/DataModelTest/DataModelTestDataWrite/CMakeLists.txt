################################################################################
# Package: DataModelTestDataWrite
################################################################################

# Declare the package name:
atlas_subdir( DataModelTestDataWrite )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/CLIDSvc
                          Control/DataModelAthenaPool
                          Control/DataModelTest/DataModelTestDataCommon
                          Control/SGTools
                          Event/xAOD/xAODCore
                          GaudiKernel
                          PRIVATE
                          Control/AthContainersInterfaces
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DataModelTestDataWriteLib
                   src/*.cxx
                   PUBLIC_HEADERS DataModelTestDataWrite
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks DataModelTestDataCommon SGTools xAODCore GaudiKernel DataModelAthenaPoolLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils )

atlas_add_component( DataModelTestDataWrite
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib DataModelTestDataCommon SGTools xAODCore GaudiKernel AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests DataModelTestDataWriteLib )

atlas_add_dictionary( DataModelTestDataWriteDict
                      DataModelTestDataWrite/DataModelTestDataWriteDict.h
                      DataModelTestDataWrite/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib DataModelTestDataCommon SGTools xAODCore GaudiKernel AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests DataModelTestDataWriteLib
                      NO_ROOTMAP_MERGE
                      EXTRA_FILES src/dict/*.cxx
                      ELEMENT_LINKS DataVector<DMTest::B> )

