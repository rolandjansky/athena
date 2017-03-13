################################################################################
# Package: InDetTrackValidation
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthenaKernel
   GaudiKernel
   InnerDetector/InDetRawEvent/InDetRawData
   InnerDetector/InDetRecEvent/InDetPrepRawData
   InnerDetector/InDetSimEvent
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkValidation/TrkValInterfaces
   PRIVATE
   Control/DataModel
   DetectorDescription/AtlasDetDescr
   DetectorDescription/GeoModel/GeoModelInterfaces
   Event/EventPrimitives
   Event/xAOD/xAODEventInfo
   Generators/GeneratorObjects
   InnerDetector/InDetConditions/InDetBeamSpotService
   InnerDetector/InDetConditions/InDetConditionsSummaryService
   InnerDetector/InDetConditions/PixelConditionsServices
   InnerDetector/InDetConditions/SCT_ConditionsServices
   InnerDetector/InDetConditions/TRT_ConditionsData
   InnerDetector/InDetConditions/TRT_ConditionsServices
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/PixelGeoModel
   InnerDetector/InDetDetDescr/SCT_Cabling
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   InnerDetector/InDetValidation/InDetTrackSplitterTool
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkPrepRawData
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkSpacePoint
   Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore )

# Component(s) in the package:
atlas_add_component( InDetTrackValidation
   InDetTrackValidation/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES}
   AthenaBaseComps AthenaKernel GaudiKernel InDetRawData InDetPrepRawData
   InDetSimEvent TrkParameters TrkTrack TrkValInterfaces DataModel AtlasDetDescr
   EventPrimitives xAODEventInfo GeneratorObjects TRT_ConditionsData
   TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry SCT_CablingLib
   InDetRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkMeasurementBase
   TrkPrepRawData TrkRIO_OnTrack TrkSpacePoint TrkToolInterfaces
   PixelGeoModelLib )
