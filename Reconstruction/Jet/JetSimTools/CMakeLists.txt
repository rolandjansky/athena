################################################################################
# Package: JetSimTools
################################################################################

# Declare the package name:
atlas_subdir( JetSimTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          Reconstruction/Jet/JetRec
                          PRIVATE
                          Reconstruction/Jet/JetUtils )

# Component(s) in the package:
atlas_add_component( JetSimTools
                     Root/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools xAODTruth GaudiKernel JetRecLib JetUtils )

# Install files from the package:
atlas_install_headers( JetSimTools )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

