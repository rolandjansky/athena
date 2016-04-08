################################################################################
# Package: xAODBase
################################################################################

# Declare the package name:
atlas_subdir( xAODBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/SGTools
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( xAODBase
                   Root/*.cxx
                   PUBLIC_HEADERS xAODBase
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers SGTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks )

atlas_add_dictionary( xAODBaseDict
                      xAODBase/xAODBaseDict.h
                      xAODBase/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers SGTools TestTools AthLinks xAODBase )

atlas_add_test( ut_xAODObjectType_test
                SOURCES
                test/ut_xAODObjectType_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers SGTools TestTools AthLinks xAODBase )

