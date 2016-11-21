################################################################################
# Package: TrigConfigSvc
################################################################################

# Declare the package name:
atlas_subdir( TrigConfigSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthenaKernel
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
find_package( Boost COMPONENTS filesystem thread system )
find_package( COOL COMPONENTS CoolKernel )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread PyROOT )

# Component(s) in the package:
atlas_add_library( TrigConfigSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfigSvc
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigConfHLTData TrigConfL1Data TrigSteeringEvent StoreGateLib SGtests TrigMonitorBaseLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${COOL_LIBRARIES} AthenaKernel AthenaPoolUtilities IOVDbDataModel EventInfo PathResolver TrigConfBase TrigConfJobOptData TrigConfStorage L1TopoConfig )

atlas_add_component( TrigConfigSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${COOL_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel TrigConfHLTData TrigConfL1Data TrigSteeringEvent AthenaKernel AthenaPoolUtilities IOVDbDataModel EventInfo PathResolver TrigConfBase TrigConfJobOptData TrigConfStorage TrigMonitorBaseLib L1TopoConfig TrigConfigSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/checkTrigger.py share/checkTriggerConfig.py )
atlas_install_xmls( data/*.dtd )

# Aliases:
atlas_add_alias( checkTrigger "checkTrigger.py" )

