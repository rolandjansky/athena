
################################################################################
# Package: TRTMonitoringRun3
################################################################################

# Declare the package name:
atlas_subdir( TRTMonitoringRun3 )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Tree Hist RIO pthread MathCore Core Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11)

atlas_add_library( TRTMonitoringRun3Lib
                   src/*.cxx
                   PUBLIC_HEADERS TRTMonitoringRun3
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps xAODEventInfo InDetPrepRawData GaudiKernel TRT_ConditionsServicesLib InDetReadoutGeometry TRT_ReadoutGeometry InDetRawData TrkTrack TrkToolInterfaces AthenaMonitoringLib CommissionEvent TrkSpacePoint MagFieldInterfaces InDetByteStreamErrors
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaKernel SGTools SGAudCore AthenaPoolUtilities LumiBlockCompsLib EventInfo LWHists TrkTrackSummary InDetRIO_OnTrack InDetIdentifier TrkRIO_OnTrack TrkParameters TrkMeasurementBase TrkEventUtils TrkSurfaces PathResolver EventPrimitives Identifier AtlasDetDescr AthContainers TRT_DriftFunctionToolLib )
                   
atlas_add_component( TRTMonitoringRun3
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} TRTMonitoringRun3Lib AthenaMonitoringLib SGAudCore SGTools AthenaPoolUtilities GaudiKernel EventInfo InDetRawData InDetPrepRawData LumiBlockCompsLib CommissionEvent AthContainers AtlasDetDescr Identifier xAODEventInfo EventPrimitives TRT_ConditionsServicesLib InDetReadoutGeometry TRT_ReadoutGeometry InDetRIO_OnTrack LWHists TrkTrack TrkTrackSummary TrkToolInterfaces AthenaKernel InDetIdentifier MagFieldInterfaces PathResolver TrkSurfaces TrkEventUtils TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkSpacePoint TRT_DriftFunctionToolLib )
                   
# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

