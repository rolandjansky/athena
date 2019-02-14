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
                          Event/EventInfoUtils
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODCnvInterfaces
                          Event/xAOD/xAODEventInfoCnv
                          GaudiKernel )

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
                     PileUpToolsLib StoreGateLib SGtests EventInfo
                     EventInfoUtils xAODEventInfo xAODEventInfoCnv GaudiKernel )


