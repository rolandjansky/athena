################################################################################
# Package: LumiBlockData
################################################################################

# Declare the package name:
atlas_subdir( LumiBlockData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LumiBlockData
                   src/*.cxx
                   PUBLIC_HEADERS LumiBlockData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel AthContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( LumiBlockDataDict
                      LumiBlockData/LumiBlockDataDict.h
                      LumiBlockData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel AthContainers LumiBlockData )

atlas_add_test( LBDurationCondData_test
  SOURCES test/LBDurationCondData_test.cxx
  LINK_LIBRARIES LumiBlockData )

atlas_add_test( LuminosityCondData_test
  SOURCES test/LuminosityCondData_test.cxx
  LINK_LIBRARIES LumiBlockData )

atlas_add_test( TrigLiveFractionCondData_test
  SOURCES test/TrigLiveFractionCondData_test.cxx
  LINK_LIBRARIES LumiBlockData )

