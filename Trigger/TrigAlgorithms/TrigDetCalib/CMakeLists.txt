################################################################################
# Package: TrigDetCalib
################################################################################

# Declare the package name:
atlas_subdir( TrigDetCalib )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/IRegionSelector
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigSteer/TrigSteering
                          Trigger/TrigT1/TrigT1Result
                          Trigger/TrigTools/TrigTimeAlgs 
                          Trigger/TrigEvent/TrigMuonEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigDetCalib
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} IRegionSelector GaudiKernel TrigInDetEvent TrigSteeringEvent AthenaBaseComps AthenaKernel EventInfo TrkTrack TrigInterfacesLib TrigSteeringLib TrigT1Result TrigTimeAlgsLib TrigMuonEvent)

# Install files from the package:
atlas_install_headers( TrigDetCalib )
atlas_install_python_modules( python/*.py )

