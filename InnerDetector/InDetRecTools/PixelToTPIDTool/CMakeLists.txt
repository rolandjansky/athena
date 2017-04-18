################################################################################
# Package: PixelToTPIDTool
################################################################################

# Declare the package name:
atlas_subdir( PixelToTPIDTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/StoreGate
   GaudiKernel
   Tracking/TrkEvent/TrkEventPrimitives
   PRIVATE
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/RegistrationServices
   DetectorDescription/Identifier
   Event/EventInfo
   InnerDetector/InDetConditions/PixelConditionsServices
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/PixelGeoModel
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   Reconstruction/Particle
   Tools/PathResolver
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkEvent/TrkTrackSummary
   Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS MathCore Core Tree Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( PixelToTPIDTool
  PixelToTPIDTool/*.h  src/*.cxx src/components/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES}
  AthenaBaseComps AthenaKernel StoreGateLib GaudiKernel TrkEventPrimitives
  AthenaPoolUtilities Identifier EventInfo InDetIdentifier InDetRIO_OnTrack
  Particle PathResolver TrkSurfaces TrkMeasurementBase TrkParameters
  TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkToolInterfaces PixelGeoModelLib )

# Install files from the package:
atlas_install_runtime( share/*.txt )
