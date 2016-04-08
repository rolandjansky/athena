################################################################################
# Package: TrigT2IDTau
################################################################################

# Declare the package name:
atlas_subdir( TrigT2IDTau )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Reconstruction/Particle
                          Trigger/TrigEvent/TrigParticle )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT2IDTauLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2IDTau
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigInDetEvent TrigSteeringEvent TrigInterfacesLib
                   PRIVATE_LINK_LIBRARIES Particle TrigParticle )

atlas_add_component( TrigT2IDTau
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigInDetEvent TrigSteeringEvent TrigInterfacesLib Particle TrigParticle TrigT2IDTauLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

