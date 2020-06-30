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
   Control/CxxUtils
   Control/StoreGate
   Event/xAOD/xAODEventInfo
   GaudiKernel
   InnerDetector/InDetRawEvent/InDetRawData
   InnerDetector/InDetRecEvent/InDetPrepRawData
   InnerDetector/InDetSimEvent
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkSpacePoint
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkValidation/TrkValInterfaces
   PRIVATE
   Control/AthContainers
   DetectorDescription/AtlasDetDescr
   DetectorDescription/GeoModel/GeoModelInterfaces
   Event/EventPrimitives
   Generators/GeneratorObjects
   Generators/AtlasHepMC
   InnerDetector/InDetConditions/InDetBeamSpotService
   InnerDetector/InDetConditions/InDetConditionsSummaryService
   InnerDetector/InDetConditions/PixelConditionsTools
   InnerDetector/InDetConditions/SCT_ConditionsTools
   InnerDetector/InDetConditions/TRT_ConditionsData
   InnerDetector/InDetConditions/TRT_ConditionsServices
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/SCT_Cabling
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   InnerDetector/InDetValidation/InDetTrackSplitterTool
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkPrepRawData
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore )

# Component(s) in the package:
atlas_add_component( InDetTrackValidation
   InDetTrackValidation/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib
   AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests xAODEventInfo GaudiKernel InDetRawData InDetPrepRawData
   InDetSimEvent TrkParameters TrkSpacePoint TrkTrack TrkValInterfaces AthContainers AtlasDetDescr
   EventPrimitives GeneratorObjects TRT_ConditionsData
   TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry SCT_CablingLib
   InDetRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkMeasurementBase
   TrkPrepRawData TrkRIO_OnTrack TrkToolInterfaces SCT_ConditionsToolsLib
   InDetTrackSplitterToolLib )
