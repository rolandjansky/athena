################################################################################
# Package: TrackParticleAssociationAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrackParticleAssociationAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          Event/xAOD/xAODAssociations
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/RecoTools/TrackToCalo
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrackParticleAssociationAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloUtilsLib AthenaBaseComps xAODAssociations xAODCaloEvent xAODTracking GaudiKernel RecoToolInterfaces TrackToCaloLib TrkToolInterfaces )

