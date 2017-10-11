################################################################################
# Package: DerivationFrameworkTileCal
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkTileCal )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthContainers
                          Control/AthenaKernel
                          Control/CxxUtils
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          Reconstruction/RecoEvent/ParticleCaloExtension
                          Reconstruction/RecoTools/ParticlesInConeTools
                          Reconstruction/RecoTools/RecoToolInterfaces
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkTileCalLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkTileCal
                   LINK_LIBRARIES AthenaBaseComps xAODMuon xAODTracking exctrace_collector calg AthDSoCallBacks
                   PRIVATE_LINK_LIBRARIES CaloEvent CaloIdentifier AthContainers AthenaKernel CxxUtils xAODBase xAODCaloEvent GaudiKernel ParticleCaloExtension RecoToolInterfaces TileEvent TileIdentifier TrkParameters TrkParametersIdentificationHelpers )

atlas_add_component( DerivationFrameworkTileCal
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODMuon xAODTracking CaloEvent CaloIdentifier AthContainers AthenaKernel CxxUtils exctrace_collector calg AthDSoCallBacks xAODBase xAODCaloEvent GaudiKernel ParticleCaloExtension RecoToolInterfaces TileEvent TileIdentifier TrkParameters TrkParametersIdentificationHelpers DerivationFrameworkTileCalLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

