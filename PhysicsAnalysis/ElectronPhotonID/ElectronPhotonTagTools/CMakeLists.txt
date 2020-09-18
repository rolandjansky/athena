################################################################################
# Package: ElectronPhotonTagTools
################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonTagTools )

# Component(s) in the package:
atlas_add_library( ElectronPhotonTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS ElectronPhotonTagTools
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities EventInfo xAODEgamma xAODEventInfo xAODMissingET GaudiKernel TagEvent StoreGateLib SGtests IsolationSelectionLib EgammaAnalysisInterfacesLib AnalysisUtilsLib IsolationCorrectionsLib
                   PRIVATE_LINK_LIBRARIES xAODCore xAODParticleEvent xAODTracking )

atlas_add_component( ElectronPhotonTagTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel ElectronPhotonTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

