################################################################################
# Package: DataModelTestDataCommon
################################################################################

# Declare the package name:
atlas_subdir( DataModelTestDataCommon )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthContainersInterfaces
                          Control/SGTools
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          PRIVATE
                          Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DataModelTestDataCommon
                   src/*.cxx
                   PUBLIC_HEADERS DataModelTestDataCommon
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers SGTools xAODCore xAODTrigger GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks )

atlas_add_dictionary( DataModelTestDataCommonDict
                      DataModelTestDataCommon/DataModelTestDataCommonDict.h
                      DataModelTestDataCommon/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers SGTools xAODCore xAODTrigger GaudiKernel AthLinks DataModelTestDataCommon
                      NO_ROOTMAP_MERGE
                      EXTRA_FILES src/dict/*.cxx
                      ELEMENT_LINKS DMTest::BAuxVec DMTest::CVec_v1 )

