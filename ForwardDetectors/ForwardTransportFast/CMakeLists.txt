################################################################################
# Package: ForwardTransportFast
################################################################################

# Declare the package name:
atlas_subdir( ForwardTransportFast )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          ForwardDetectors/ForwardTracker
                          ForwardDetectors/ForwardTransportSvc
                          GaudiKernel
                          PRIVATE
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( ForwardTransportFast
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ${GEANT4_LIBRARIES} AthenaBaseComps ForwardTracker GaudiKernel GeneratorObjects ForwardTracker ForwardTransportSvcLib )

# Install files from the package:
atlas_install_headers( ForwardTransportFast )
atlas_install_joboptions( share/*.py )

