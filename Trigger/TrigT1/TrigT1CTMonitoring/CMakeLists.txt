################################################################################
# Package: TrigT1CTMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CTMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Control/AthenaBaseComps
                          Trigger/TrigEvent/TrigDecisionInterface
                          Trigger/TrigT1/TrigT1Interfaces
                          Control/AthenaMonitoring
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/CoraCool
                          Event/EventInfo
                          Tools/LWHists
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigConfiguration/TrigConfStorage
                          Trigger/TrigT1/TrigT1CTP
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1Result 
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData)

# External dependencies:
find_package( Boost COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT1CTMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CTMonitoring
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} AthenaBaseComps GaudiKernel TrigT1Interfaces AthenaMonitoringLib StoreGateLib SGtests TrigT1CaloEventLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaPoolUtilities CoraCool EventInfo LWHists TrigConfL1Data TrigConfStorage TrigT1Result )

atlas_add_component( TrigT1CTMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaMonitoringLib StoreGateLib SGtests GaudiKernel TrigT1Interfaces MuonTrigCoinData AthenaPoolUtilities CoraCool EventInfo LWHists TrigConfL1Data TrigConfStorage TrigT1CaloEventLib TrigT1Result MuonDigitContainer MuonRDO TrigT1CTMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_xmls( data/*.xml )
atlas_install_xmls( data/*.xml )
atlas_install_generic( scripts/*.py
                       DESTINATION share/bin
                       EXECUTABLE )

