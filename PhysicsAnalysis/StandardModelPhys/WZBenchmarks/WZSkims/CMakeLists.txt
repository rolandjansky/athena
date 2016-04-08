################################################################################
# Package: WZSkims
################################################################################

# Declare the package name:
atlas_subdir( WZSkims )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MissingETEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Trigger/TrigEvent/TrigSteeringEvent )

# Component(s) in the package:
atlas_add_component( WZSkims
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel JetEvent MissingETEvent muonEvent Particle egammaEvent TrigDecisionToolLib TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( WZSkims )
atlas_install_joboptions( share/*.py )

