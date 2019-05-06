################################################################################
# Package: TrigT1CTP
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CTP )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Control/AthenaMonitoring
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          Event/xAOD/xAODTrigCalo
                          Tools/PathResolver
                          Trigger/TrigConfiguration/TrigConfStorage
                          Trigger/TrigT1/L1Topo/L1TopoConfig
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result
			  Trigger/TrigSteer/TrigSteering )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_component( TrigT1CTP
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel TrigConfL1Data TrigInterfacesLib AthenaKernel AthenaMonitoringLib AthenaPoolUtilities EventInfo PathResolver TrigConfStorage L1TopoConfig TrigT1Interfaces TrigT1Result )

# Install files from the package:
atlas_install_headers( TrigT1CTP )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_generic( data/*.cfg
                       DESTINATION share/TrigT1CTP
                       EXECUTABLE )

