################################################################################
# Package: PileUpComps
################################################################################

# Declare the package name:
atlas_subdir( PileUpComps )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( GTest )

# Component(s) in the package:
atlas_add_component( PileUpComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES}
                     ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel
                     PileUpToolsLib StoreGateLib SGtests EventInfo
                     EventInfoUtils xAODEventInfo GaudiKernel )


atlas_add_test( PileUpHashHelper_test
                SOURCES test/PileUpHashHelper_test.cxx src/PileUpHashHelper.cxx
                INCLUDE_DIRS src ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES xAODEventInfo ${GTEST_LIBRARIES} )
