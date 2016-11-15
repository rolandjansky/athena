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
                          #Event/xAOD/xAODEventInfo
                          Event/EventInfo
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigEvent/TrigNavigation
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
                   LINK_LIBRARIES AthContainers AthLinks DataModelTestDataCommon SGTools xAODCore GaudiKernel DataModelAthenaPoolLib StoreGateLib SGtests TrigNavigationLib EventInfo TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils )

atlas_add_component( DataModelTestDataWrite
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib DataModelTestDataCommon SGTools xAODCore GaudiKernel AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests TrigSteeringEvent DataModelTestDataWriteLib )

atlas_add_dictionary( DataModelTestDataWriteDict
                      DataModelTestDataWrite/DataModelTestDataWriteDict.h
                      DataModelTestDataWrite/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib DataModelTestDataCommon SGTools xAODCore GaudiKernel AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests DataModelTestDataWriteLib
                      NO_ROOTMAP_MERGE
                      EXTRA_FILES src/dict/*.cxx
                      ELEMENT_LINKS DataVector<DMTest::B> )



atlas_add_sercnv_library ( DataModelTestDataWriteSerCnv
  FILES DataModelTestDataWrite/HVec.h DataModelTestDataWrite/HView.h DataModelTestDataWrite/HAuxContainer.h
  TYPES_WITH_NAMESPACE DMTest::HVec DMTest::HView DMTest::HAuxContainer
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers SGTools xAODCore xAODTrigger GaudiKernel AthLinks TrigSerializeCnvSvcLib DataModelTestDataCommon DataModelTestDataWriteLib )
