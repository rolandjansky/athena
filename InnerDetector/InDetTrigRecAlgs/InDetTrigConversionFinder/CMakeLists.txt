################################################################################
# Package: InDetTrigConversionFinder
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigConversionFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParticleBase
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( InDetTrigConversionFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel VxVertex TrigInterfacesLib xAODTracking InDetRecToolInterfaces Particle TrkParticleBase TrigParticle )

# Install files from the package:
atlas_install_headers( InDetTrigConversionFinder )
atlas_install_python_modules( python/*.py )

