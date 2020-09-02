################################################################################
# Package: InDetAlignmentMonitoring
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignmentMonitoring )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO MathMore Gpad Matrix )

# Component(s) in the package:
atlas_add_component( InDetAlignmentMonitoring
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib
   GaudiKernel CommissionEvent AthenaBaseComps AthenaMonitoringLib AthContainers
   AtlasDetDescr Identifier EventPrimitives xAODEventInfo xAODTracking
   TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry
   InDetPrepRawData InDetRIO_OnTrack Particle LWHists TrkGeometry TrkSurfaces
   TrkEventPrimitives TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack
   TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkTruthData VxVertex TrkExInterfaces
   TrkToolInterfaces TrkVertexFitterInterfaces InDetTrackSelectionToolLib
   BeamSpotConditionsData InDetAlignGenToolsLib InDetTrackSplitterToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
