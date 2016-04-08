################################################################################
# Package: TrigSteeringTest
################################################################################

# Declare the package name:
atlas_subdir( TrigSteeringTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Control/DataModel
                          Event/xAOD/xAODTrigger
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_component( TrigSteeringTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrigSteeringEvent TrigInterfacesLib AthenaKernel DataModel xAODTrigger TrigNavigationLib TrigT1Interfaces TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigSteeringTest )
atlas_install_python_modules( python/*.py )

