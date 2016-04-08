################################################################################
# Package: GeneratorUtils
################################################################################

# Declare the package name:
atlas_subdir( GeneratorUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_test( StringParse_test
                SOURCES
                test/StringParse_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} TestTools )

# Install files from the package:
atlas_install_headers( GeneratorUtils )
atlas_install_joboptions( share/*.py )

