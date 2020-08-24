################################################################################
# Package: InDetTrigTrackResidualMonitor
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigTrackResidualMonitor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthContainers
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkValidation/TrkValEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/CxxUtils
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetAlignTools/InDetAlignGenTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkTools/TrkTrackSummaryTool
                          Tracking/TrkValidation/TrkValTools
                          Trigger/TrigEvent/TrigParticle )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetTrigTrackResidualMonitor
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthContainers AtlasDetDescr Identifier GaudiKernel InDetReadoutGeometry InDetPrepRawData TrkEventPrimitives TrkParameters TrkTrack TrkValEvent TrigSteeringEvent TrigInterfacesLib IdDictDetDescr InDetIdentifier MagFieldInterfaces TrkGeometry TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkRIO_OnTrack TrkTrackSummary TrkExInterfaces TrkToolInterfaces TrigParticle TrkValToolsLib InDetAlignGenToolsLib )

# Install files from the package:
atlas_install_headers( InDetTrigTrackResidualMonitor )
atlas_install_python_modules( python/*.py )

