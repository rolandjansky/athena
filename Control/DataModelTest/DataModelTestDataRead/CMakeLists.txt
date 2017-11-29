################################################################################
# Package: DataModelTestDataRead
################################################################################

# Declare the package name:
atlas_subdir( DataModelTestDataRead )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthenaKernel
                          Control/DataModelAthenaPool
                          Control/DataModelTest/DataModelTestDataCommon
                          GaudiKernel
                          PRIVATE
                          Control/AthContainersInterfaces
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Event/xAOD/xAODCore
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DataModelTestDataReadLib
                   src/*.cxx
                   PUBLIC_HEADERS DataModelTestDataRead
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks DataModelTestDataCommonLib GaudiKernel DataModelAthenaPoolLib StoreGateLib SGtests TrigSteeringEvent TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils xAODCore )

atlas_add_component( DataModelTestDataRead
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib DataModelTestDataCommonLib GaudiKernel AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests xAODCore TrigSteeringEvent DataModelTestDataReadLib )

atlas_add_dictionary( DataModelTestDataReadDict
                      DataModelTestDataRead/DataModelTestDataReadDict.h
                      DataModelTestDataRead/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib DataModelTestDataCommonLib GaudiKernel AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests xAODCore DataModelTestDataReadLib
                      NO_ROOTMAP_MERGE
                      EXTRA_FILES src/dict/*.cxx
                      ELEMENT_LINKS DataVector<DMTest::B> )


atlas_add_sercnv_library ( DataModelTestDataReadSerCnv
  FILES DataModelTestDataRead/HVec.h DataModelTestDataRead/HView.h DataModelTestDataRead/HAuxContainer.h
  TYPES_WITH_NAMESPACE DMTest::HVec DMTest::HView DMTest::HAuxContainer
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
  LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore xAODTrigger GaudiKernel AthLinks TrigSerializeCnvSvcLib DataModelTestDataCommonLib DataModelTestDataReadLib )
