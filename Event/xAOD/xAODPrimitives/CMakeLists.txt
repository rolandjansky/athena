################################################################################
# Package: xAODPrimitives
################################################################################

# Declare the package name:
atlas_subdir( xAODPrimitives )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODPrimitives
                   Root/*.cxx
                   PUBLIC_HEADERS xAODPrimitives
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools )

atlas_add_dictionary( xAODPrimitivesDict
                      xAODPrimitives/xAODPrimitivesDict.h
                      xAODPrimitives/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers TestTools xAODPrimitives )

atlas_add_test( ut_xAOD_primitives_enums_test
                SOURCES
                test/ut_xAOD_primitives_enums_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers TestTools xAODPrimitives )

