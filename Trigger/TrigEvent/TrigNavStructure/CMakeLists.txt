################################################################################
# Package: TrigNavStructure
################################################################################

# Declare the package name:
atlas_subdir( TrigNavStructure )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthToolSupport/AsgTools
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TrigNavStructure
                   Root/*.cxx
                   PUBLIC_HEADERS TrigNavStructure
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AsgTools
                   PRIVATE_LINK_LIBRARIES TestTools )

atlas_add_test( ut_iterators_test
                SOURCES
                test/ut_iterators_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AsgTools TestTools TrigNavStructure
                EXTRA_PATTERNS ".*" )

