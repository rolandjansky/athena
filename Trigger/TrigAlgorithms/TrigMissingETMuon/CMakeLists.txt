################################################################################
# Package: TrigMissingETMuon
################################################################################

# Declare the package name:
atlas_subdir( TrigMissingETMuon )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigMissingET
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
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
atlas_add_library( TrigMissingETMuonLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigMissingETMuon
                   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES xAODTrigMissingET TrigInterfacesLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils EventInfo xAODMuon GaudiKernel TrigMissingEtEvent TrigMuonEvent TrigSteeringEvent TrigT1Interfaces )

atlas_add_component( TrigMissingETMuon
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} xAODTrigMissingET TrigInterfacesLib CxxUtils EventInfo xAODMuon GaudiKernel TrigMissingEtEvent TrigMuonEvent TrigNavigationLib TrigSteeringEvent TrigT1Interfaces TrigMissingETMuonLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

