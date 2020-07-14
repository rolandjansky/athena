################################################################################
# Package: RHadrons
################################################################################

# Declare the package name:
atlas_subdir( RHadrons )

if( NOT GENERATIONBASE )

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
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
