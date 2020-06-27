################################################################################
# Package: ParticlesInConeTools
################################################################################

# Declare the package name:
atlas_subdir( ParticlesInConeTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
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

atlas_add_library( ParticlesInConeToolsLib
                   ParticlesInConeTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS ParticlesInConeTools
                   LINK_LIBRARIES GaudiKernel xAODCaloEvent xAODTracking xAODTruth xAODPFlow AthLinks  )

# Component(s) in the package:
atlas_add_component( ParticlesInConeTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ParticlesInConeToolsLib AthenaKernel AthContainers AsgTools AthenaBaseComps PFlowUtilsLib )


