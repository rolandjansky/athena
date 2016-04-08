################################################################################
# Package: TrigConfxAOD
################################################################################

# Declare the package name:
atlas_subdir( TrigConfxAOD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODTrigger
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( TrigConfxAOD
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools xAODTrigger TrigConfHLTData TrigConfL1Data AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel )

# Install files from the package:
atlas_install_headers( TrigConfxAOD )
atlas_install_joboptions( share/*.py )

