################################################################################
# Package: xAODTruth
################################################################################

# Declare the package name:
atlas_subdir( xAODTruth )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          PRIVATE
                          AtlasTest/TestTools
                          Generators/TruthUtils )

# External dependencies:
find_package( ROOT COMPONENTS Physics Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODTruth
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTruth
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools TruthUtils )

atlas_add_dictionary( xAODTruthDict
                      xAODTruth/xAODTruthDict.h
                      xAODTruth/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore TestTools TruthUtils xAODTruth
                      EXTRA_FILES Root/dict/*.cxx )

atlas_add_test( ut_xaodtruth_particle_test
                SOURCES
                test/ut_xaodtruth_particle_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore TestTools TruthUtils xAODTruth )

