################################################################################
# Package: TrigGenericAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrigGenericAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigL1Calo
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTrigger
                          Trigger/TrigAlgorithms/TrigL2MuonSA
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigMuonRoITools
                          PRIVATE
                          Control/AthenaKernel
                          DetectorDescription/IRegionSelector
                          Event/ByteStreamCnvSvcBase
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_component( TrigGenericAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} xAODTrigL1Calo xAODTrigMuon xAODTrigger TrigL2MuonSALib TrigCaloEvent TrigMuonEvent TrigInterfacesLib TrigT1Interfaces AthenaKernel IRegionSelector ByteStreamCnvSvcBaseLib GaudiKernel TrigConfL1Data TrigNavigationLib TrigSteeringEvent TrigT1Result )

# Install files from the package:
atlas_install_headers( TrigGenericAlgs )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

