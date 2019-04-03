################################################################################
# Package: OverlayCommonAlgs
################################################################################

# Declare the package name:
atlas_subdir( OverlayCommonAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          Event/xAOD/xAODJet
                          Generators/GeneratorObjects
                          Reconstruction/RecEvent
                          Simulation/G4Sim/TrackRecord )

# Component(s) in the package:
atlas_add_component( OverlayCommonAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps
                     CaloSimEvent GeneratorObjects RecEvent xAODJet )

# Install files from the package:
atlas_install_python_modules( python/*.py )
