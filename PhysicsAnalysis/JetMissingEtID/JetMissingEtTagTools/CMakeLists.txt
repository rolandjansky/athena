################################################################################
# Package: JetMissingEtTagTools
################################################################################

# Declare the package name:
atlas_subdir( JetMissingEtTagTools )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( JetMissingEtTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS JetMissingEtTagTools
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities xAODJet GaudiKernel TagEvent AnalysisUtilsLib JetSelectorToolsLib FTagAnalysisInterfacesLib JetCalibToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers xAODCore xAODMissingET xAODParticleEvent JetUtils METInterface )

atlas_add_component( JetMissingEtTagTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaPoolUtilities xAODJet GaudiKernel TagEvent AthContainers xAODCore xAODMissingET xAODParticleEvent AnalysisUtilsLib JetSelectorToolsLib JetUtils JetMissingEtTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

