################################################################################
# Package: TrigT2Tau
################################################################################

# Declare the package name:
atlas_subdir( TrigT2Tau )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Event/FourMom )

# Component(s) in the package:
atlas_add_library( TrigT2TauLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2Tau
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigCaloEvent TrigInDetEvent TrigParticle TrigSteeringEvent TrigInterfacesLib
                   PRIVATE_LINK_LIBRARIES FourMom )

atlas_add_component( TrigT2Tau
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigCaloEvent TrigInDetEvent TrigParticle TrigSteeringEvent TrigInterfacesLib FourMom TrigT2TauLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

