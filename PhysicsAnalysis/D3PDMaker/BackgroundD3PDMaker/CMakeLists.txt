################################################################################
# Package: BackgroundD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( BackgroundD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaKernel
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          GaudiKernel
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/EventTag/TagEvent
                          Reconstruction/RecBackground/RecBackgroundEvent )

# Component(s) in the package:
atlas_add_component( BackgroundD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel xAODCaloEvent xAODEventInfo xAODJet LUCID_RawEvent GaudiKernel D3PDMakerUtils TagEvent RecBackgroundEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

