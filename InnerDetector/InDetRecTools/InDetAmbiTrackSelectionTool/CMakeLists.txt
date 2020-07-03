################################################################################
# Package: InDetAmbiTrackSelectionTool
################################################################################

# Declare the package name:
atlas_subdir( InDetAmbiTrackSelectionTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthenaKernel
   GaudiKernel
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkTools/TrkToolInterfaces
   Tracking/TrkValidation/TrkValInterfaces
   PRIVATE
   Control/AthContainers
   Control/CxxUtils
   Control/StoreGate
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/PixelGeoModel
   InnerDetector/InDetRecEvent/InDetPrepRawData
   InnerDetector/InDetRecTools/InDetRecToolInterfaces
   Tracking/TrkDetDescr/TrkDetElementBase
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkCaloClusterROI
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkPrepRawData
   Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
   Tracking/TrkEvent/TrkTrackSummary 
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkFitter/TrkFitterInterfaces)

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( InDetAmbiTrackSelectionTool
   InDetAmbiTrackSelectionTool/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrkRIO_OnTrack
   TrkTrack TrkToolInterfaces TrkValInterfaces AthContainers CxxUtils StoreGateLib
   InDetIdentifier InDetPrepRawData InDetRecToolInterfaces TrkDetElementBase
   TrkSurfaces TrkCaloClusterROI TrkMeasurementBase TrkPrepRawData
   TrkPseudoMeasurementOnTrack TrkTrackSummary TrkParameters PixelGeoModelLib TrkFitterInterfaces)
