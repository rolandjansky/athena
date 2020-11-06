################################################################################
# Package: TrackParticleAssociationAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrackParticleAssociationAlgs )

# Component(s) in the package:
atlas_add_component( TrackParticleAssociationAlgs
                     src/*.cxx
                     src/components/*.cxx
		     INCLUDE_DIRS 
                     LINK_LIBRARIES  CaloUtilsLib AthenaBaseComps xAODAssociations xAODCaloEvent xAODTracking FourMomUtils GaudiKernel RecoToolInterfaces TrackToCaloLib TrkToolInterfaces )

