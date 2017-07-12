# $Id: CMakeLists.txt 767189 2016-08-10 09:17:01Z krasznaa $
################################################################################
# Package: TrigT1CTMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CTMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   PRIVATE
   Trigger/TrigConfiguration/TrigConfInterfaces
   Control/AthenaBaseComps
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
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData )

# External dependencies:
find_package( Boost )
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigT1CTMonitoring
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES}
   ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib GaudiKernel
   TrigT1Interfaces MuonTrigCoinData AthenaPoolUtilities CoraCool EventInfo
   LWHists TrigConfL1Data TrigConfStorage TrigT1CaloEventLib TrigT1Result
   MuonDigitContainer MuonRDO )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_xmls( data/*.xml )
atlas_install_scripts( scripts/*.py )
