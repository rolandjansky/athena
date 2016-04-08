################################################################################
# Package: TrigJetUtils
################################################################################

# Declare the package name:
atlas_subdir( TrigJetUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Reconstruction/Jet/JetEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( TrigJetUtils
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib SGtests JetEvent TrigNavigationLib TrigParticle )

# Install files from the package:
atlas_install_headers( TrigJetUtils )

