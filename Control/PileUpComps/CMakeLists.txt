################################################################################
# Package: PileUpComps
################################################################################

# Declare the package name:
atlas_subdir( PileUpComps )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( PileUpComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES}
                     ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel
                     PileUpToolsLib StoreGateLib EventInfo
                     EventInfoUtils xAODEventInfo GaudiKernel )
