################################################################################
# Package: PileUpComps
################################################################################

# Declare the package name:
atlas_subdir( PileUpComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Control/StoreGate
                          Event/EventInfo
                          Event/xAOD/xAODCnvInterfaces
                          Event/xAOD/xAODEventInfo
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( GTest )

# Component(s) in the package:
atlas_add_component( PileUpComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib StoreGateLib SGtests EventInfo xAODCnvInterfaces xAODEventInfo GaudiKernel )

atlas_add_test( PileUpHashHelper_test
                SOURCES src/PileUpHashHelper.cxx test/PileUpHashHelper_test.cxx
                INCLUDE_DIRS src ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES xAODEventInfo ${GTEST_LIBRARIES} )
