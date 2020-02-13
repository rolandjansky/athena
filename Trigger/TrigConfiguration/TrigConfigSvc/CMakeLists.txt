################################################################################
# Package: TrigConfigSvc
################################################################################

# Declare the package name:
atlas_subdir( TrigConfigSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfIO
                          Trigger/TrigConfiguration/TrigConfData
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigSteeringEvent
                          Control/AthenaKernel
                          Control/AthAnalysisBaseComps
                          Control/AthenaMonitoring
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/IOVDbDataModel
                          Event/EventInfo
                          Tools/PathResolver
                          Trigger/TrigConfiguration/TrigConfBase
                          Trigger/TrigConfiguration/TrigConfJobOptData
                          Trigger/TrigConfiguration/TrigConfStorage
                          Trigger/TrigMonitoring/TrigMonitorBase
                          Trigger/TrigT1/L1Topo/L1TopoConfig )

# External dependencies:
find_package( Boost )
find_package( COOL COMPONENTS CoolKernel )
find_package( ROOT COMPONENTS Hist )
find_package( cx_Oracle )
find_package( nlohmann_json )

atlas_add_component( TrigConfigSvc
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES AthAnalysisBaseCompsLib AthenaBaseComps TrigConfIO TrigConfData TrigConfStorage TrigConfL1Data L1TopoConfig EventInfo AthenaMonitoringLib nlohmann_json::nlohmann_json )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/jobOptions_commonSetup.py
  share/jobOptions_setupHLTSvc.py
  share/jobOptions_setupLVL1Svc.py
  share/testTriggerFrontierQuery.py )
atlas_install_scripts( share/checkTrigger.py share/checkTriggerConfigOld.py share/trigconf_property.py )
atlas_install_xmls( data/*.dtd )

# Aliases:
atlas_add_alias( checkTrigger "checkTrigger.py" )

atlas_add_test( AccumulatorTest
   SCRIPT python -m TrigConfigSvc.TrigConfigSvcConfig
   POST_EXEC_SCRIPT nopost.sh )
