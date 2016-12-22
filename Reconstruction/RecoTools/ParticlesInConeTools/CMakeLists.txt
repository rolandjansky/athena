################################################################################
# Package: ParticlesInConeTools
################################################################################

# Declare the package name:
atlas_subdir( ParticlesInConeTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools
                          Control/AthLinks
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Control/AthContainers
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Reconstruction/PFlow/PFlowUtils )

# Component(s) in the package:
atlas_add_component( ParticlesInConeTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks xAODCaloEvent xAODPFlow xAODTracking xAODTruth GaudiKernel AthContainers AsgTools AthenaBaseComps )

# Install files from the package:
atlas_install_headers( ParticlesInConeTools )

