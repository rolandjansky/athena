################################################################################
# Package: InDetZVTOP_Alg
################################################################################

# Declare the package name:
atlas_subdir( InDetZVTOP_Alg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkParticleBase
                          PRIVATE
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex )

# Component(s) in the package:
atlas_add_component( InDetZVTOP_Alg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkParticleBase InDetRecToolInterfaces TrkTrack VxVertex )

# Install files from the package:
atlas_install_headers( InDetZVTOP_Alg )

