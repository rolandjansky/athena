################################################################################
# Package: TrigmuRoI
################################################################################

# Declare the package name:
atlas_subdir( TrigmuRoI )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigMuonRoITools
                          PRIVATE
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_component( TrigmuRoI
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrigSteeringEvent TrigInterfacesLib TrigT1Interfaces TrigNavigationLib TrigT1Result )

# Install files from the package:
atlas_install_headers( TrigmuRoI )
atlas_install_python_modules( python/*.py )

