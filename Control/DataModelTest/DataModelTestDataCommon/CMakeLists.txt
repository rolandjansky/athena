################################################################################
# Package: DataModelTestDataCommon
################################################################################

# Declare the package name:
atlas_subdir( DataModelTestDataCommon )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthContainersInterfaces
                          Control/AthenaKernel
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          PRIVATE
                          Event/EventInfo
                          Control/AthLinks
                          Control/StoreGate
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/RegistrationServices
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DataModelTestDataCommonLib
                   src/*.cxx
                   PUBLIC_HEADERS DataModelTestDataCommon
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthenaKernel xAODCore xAODTrigger EventInfo GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks StoreGateLib AthenaBaseComps AthenaPoolUtilities )

atlas_add_component( DataModelTestDataCommon
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib AthenaKernel xAODCore GaudiKernel AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests TrigSteeringEvent DataModelTestDataCommonLib )

atlas_add_dictionary( DataModelTestDataCommonDict
                      DataModelTestDataCommon/DataModelTestDataCommonDict.h
                      DataModelTestDataCommon/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel xAODCore xAODTrigger GaudiKernel AthLinks DataModelTestDataCommonLib
                      NO_ROOTMAP_MERGE
                      EXTRA_FILES src/dict/*.cxx
                      ELEMENT_LINKS DMTest::BAuxVec DMTest::CVec_v1 )


atlas_add_sercnv_library ( DataModelTestDataCommonSerCnv
  FILES DataModelTestDataCommon/CVec.h DataModelTestDataCommon/CView.h DataModelTestDataCommon/CAuxContainer.h
  TYPES_WITH_NAMESPACE DMTest::CVec DMTest::CView DMTest::CAuxContainer
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
  LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel xAODCore xAODTrigger GaudiKernel AthLinks TrigSerializeCnvSvcLib DataModelTestDataCommonLib )

# Install files from the package:
atlas_install_scripts( scripts/*.py )
