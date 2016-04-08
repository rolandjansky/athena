################################################################################
# Package: TBBExamples
################################################################################

# Declare the package name:
atlas_subdir( TBBExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModel
                          TestPolicy )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( TBB )

# Component(s) in the package:
atlas_add_component( TBBExamples
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${TBB_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel DataModel )

# Install files from the package:
atlas_install_headers( TBBExamples )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/TBBExamples_TestConfiguration.xml )

