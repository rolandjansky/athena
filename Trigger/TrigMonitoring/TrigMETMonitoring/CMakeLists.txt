################################################################################
# Package: TrigMETMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigMETMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTrigMissingET
                          Event/xAOD/xAODTrigger
                          Reconstruction/MissingETEvent
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PRIVATE
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfL1Data )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( TrigMETMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigMETMonitoring
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES xAODEgamma xAODMissingET xAODMuon xAODTrigMissingET xAODTrigger MissingETEvent TrigMissingEtEvent TrigSteeringEvent TrigHLTMonitoringLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel TrigConfL1Data )

atlas_add_component( TrigMETMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} xAODEgamma xAODMissingET xAODMuon xAODTrigMissingET xAODTrigger MissingETEvent TrigMissingEtEvent TrigSteeringEvent TrigHLTMonitoringLib StoreGateLib SGtests GaudiKernel TrigConfL1Data TrigMETMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

