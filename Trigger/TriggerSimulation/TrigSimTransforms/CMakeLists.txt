################################################################################
# Package: TrigSimTransforms
################################################################################

# Declare the package name:
atlas_subdir( TrigSimTransforms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Event/EventInfo
                          GaudiKernel
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigSimTransforms
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel SGTools StoreGateLib SGtests EventInfo GaudiKernel TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle TrigSteeringEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.sh scripts/*.py )

