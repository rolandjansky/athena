################################################################################
# Package: InDetAlignmentMonitoringRun3
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignmentMonitoringRun3 )

# Component(s) in the package:
atlas_add_component( InDetAlignmentMonitoringRun3
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel CommissionEvent AthenaBaseComps AthenaMonitoringLib AthContainers AtlasDetDescr Identifier EventPrimitives xAODEventInfo xAODTracking TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry PixelReadoutGeometryLib SCT_ReadoutGeometry InDetPrepRawData InDetRIO_OnTrack Particle TrkGeometry TrkSurfaces TrkEventPrimitives TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkTruthData VxVertex TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces InDetTrackSelectionToolLib BeamSpotConditionsData InDetAlignGenToolsLib InDetTrackSplitterToolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
