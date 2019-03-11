################################################################################
# Package: TrigHLTMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigHLTMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Event/xAOD/xAODLuminosity
                          GaudiKernel
                          LumiBlock/LumiCalc
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfigSvc
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthenaKernel
                          Event/EventInfo
                          Event/xAOD/xAODTrigger )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( cx_Oracle )

# Component(s) in the package:
atlas_add_library( TrigHLTMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigHLTMonitoring
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES xAODLuminosity GaudiKernel TrigSteeringEvent AthenaMonitoringLib StoreGateLib SGtests LumiBlockCoolQuery TrigDecisionToolLib TrigConfigSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel EventInfo xAODTrigger )

atlas_add_component( TrigHLTMonitoring
                     src/components/*.cxx
                     LINK_LIBRARIES TrigHLTMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( python/scripts/*.py python/scripts/*.sh java/TrigMaMGUI_TRIGGERDBREPR.sh )
atlas_install_runtime(run/TrigHLTMon_tf.py)
atlas_install_generic( java/GUI/TrigMaMGUI.java
                       DESTINATION java/
                       TYPEANAME JavaGUI )
