################################################################################
# Package: TrigMissingETMuon
################################################################################

# Declare the package name:
atlas_subdir( TrigMissingETMuon )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Event/xAOD/xAODTrigMissingET
                          Trigger/TrigSteer/TrigInterfaces
                          Control/CxxUtils
                          Event/EventInfo
                          Event/xAOD/xAODMuon
                          GaudiKernel
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigMissingETMuon
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} xAODTrigMissingET TrigInterfacesLib CxxUtils EventInfo xAODMuon GaudiKernel TrigMissingEtEvent TrigMuonEvent TrigNavigationLib TrigSteeringEvent TrigT1Interfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )

