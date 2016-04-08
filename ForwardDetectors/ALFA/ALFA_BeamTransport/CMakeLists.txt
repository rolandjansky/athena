################################################################################
# Package: ALFA_BeamTransport
################################################################################

# Declare the package name:
atlas_subdir( ALFA_BeamTransport )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          ForwardDetectors/FPTracker
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          Generators/GeneratorObjects )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ALFA_BeamTransport
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps FPTracker GaudiKernel StoreGateLib SGtests EventInfo GeneratorObjects FPTracker )

# Install files from the package:
atlas_install_headers( ALFA_BeamTransport )
atlas_install_joboptions( share/*.py )

