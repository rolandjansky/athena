################################################################################
# Package: ForwardTransport
################################################################################

# Declare the package name:
atlas_subdir( ForwardTransport )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          ForwardDetectors/ForwardTracker
                          ForwardDetectors/ForwardTransportSvc
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasTools
                          Generators/AtlasHepMC
                           )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ForwardTransport
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ForwardTracker GaudiKernel G4AtlasToolsLib ForwardTransportSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

