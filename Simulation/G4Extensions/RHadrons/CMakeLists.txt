################################################################################
# Package: RHadrons
################################################################################

# Declare the package name:
atlas_subdir( RHadrons )

if( NOT GENERATIONBASE )
    # Declare the package's dependencies:
    atlas_depends_on_subdirs( PUBLIC
                              GaudiKernel
                              PRIVATE
                              Control/AthenaBaseComps
                              Control/CxxUtils
                              Simulation/G4Atlas/G4AtlasInterfaces
                              Simulation/G4Atlas/G4AtlasTools
                              Simulation/G4Extensions/G4ExternalDecay
                              Simulation/G4Sim/SimHelpers )

    # External dependencies:
    find_package( CLHEP )
    find_package( Geant4 )
    find_package( XercesC )
    find_package( Pythia8 )

    # Component(s) in the package:
    atlas_add_component( RHadrons
                         src/*.cxx
                         src/components/*.cxx
                         INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS}
                         LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA8_LIBRARIES} GaudiKernel AthenaBaseComps G4AtlasInterfaces G4AtlasToolsLib G4ExternalDecay SimHelpers Pythia8_iLib )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
