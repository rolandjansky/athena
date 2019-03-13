################################################################################
# Package: TrigmuIso
################################################################################

# Declare the package name:
atlas_subdir( TrigmuIso )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          Event/xAOD/xAODTrigMuon
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigmuIso
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests xAODTrigMuon GaudiKernel TrigInDetEvent TrigMuonEvent TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib AthenaKernel xAODEventInfo xAODTracking TrigConfHLTData TrigT1Interfaces )

# Install files from the package:
atlas_install_headers( TrigmuIso )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

