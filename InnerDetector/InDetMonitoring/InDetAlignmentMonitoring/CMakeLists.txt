################################################################################
# Package: InDetAlignmentMonitoring
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignmentMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   PRIVATE
   Commission/CommissionEvent
   Control/AthenaBaseComps
   Control/AthenaMonitoring
   Control/DataModel
   DetectorDescription/AtlasDetDescr
   DetectorDescription/Identifier
   Event/EventPrimitives
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTracking
   InnerDetector/InDetAlignTools/InDetAlignGenTools
   InnerDetector/InDetConditions/InDetBeamSpotService
   InnerDetector/InDetConditions/TRT_ConditionsServices
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetRecEvent/InDetPrepRawData
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   InnerDetector/InDetValidation/InDetTrackSplitterTool
   Reconstruction/Particle
   Tools/LWHists
   Tracking/TrkDetDescr/TrkGeometry
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkParticleBase
   Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkEvent/TrkTrackSummary
   Tracking/TrkEvent/TrkTruthData
   Tracking/TrkEvent/VxVertex
   Tracking/TrkExtrapolation/TrkExInterfaces
   Tracking/TrkTools/TrkToolInterfaces
   Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO MathMore Gpad Matrix )

# Component(s) in the package:
atlas_add_component( InDetAlignmentMonitoring
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES}
   GaudiKernel CommissionEvent AthenaBaseComps AthenaMonitoringLib DataModel
   AtlasDetDescr Identifier EventPrimitives xAODEventInfo xAODTracking
   TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry
   InDetPrepRawData InDetRIO_OnTrack Particle LWHists TrkGeometry TrkSurfaces
   TrkEventPrimitives TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack
   TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkTruthData VxVertex TrkExInterfaces
   TrkToolInterfaces TrkVertexFitterInterfaces InDetTrackSelectionToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
