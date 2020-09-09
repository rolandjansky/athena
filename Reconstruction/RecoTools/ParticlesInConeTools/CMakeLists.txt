################################################################################
# Package: ParticlesInConeTools
################################################################################

# Declare the package name:
atlas_subdir( ParticlesInConeTools )

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


