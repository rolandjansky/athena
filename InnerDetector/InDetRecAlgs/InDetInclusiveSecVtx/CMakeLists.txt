################################################################################
# Package: InDetInclusiveSecVtx
################################################################################

# Declare the package name:
atlas_subdir( InDetInclusiveSecVtx )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
			  Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PRIVATE
                          Tracking/TrkEvent/TrkTrack
			  Tracking/TrkEvent/TrkParticleBase
			  Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
			  Tracking/TrkVertexFitter/TrkVxEdmCnv )

# Component(s) in the package:
atlas_add_component( InDetInclusiveSecVtx
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces TrkTrack TrkParticleBase TrkVertexFitterInterfaces TrkVxEdmCnvLib )

# Install files from the package:
atlas_install_headers( InDetInclusiveSecVtx )