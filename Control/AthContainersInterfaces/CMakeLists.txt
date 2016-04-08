################################################################################
# Package: AthContainersInterfaces
################################################################################

# Declare the package name:
atlas_subdir( AthContainersInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CxxUtils
                          Control/SGTools
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_test( AuxStore_traits_test
                SOURCES
                test/AuxStore_traits_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils SGTools TestTools )

atlas_add_test( AuxDataOption_test
                SOURCES
                test/AuxDataOption_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils SGTools TestTools )

# Install files from the package:
atlas_install_headers( AthContainersInterfaces )

