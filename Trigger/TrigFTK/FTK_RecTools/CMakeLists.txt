################################################################################
# Package: FTK_RecTools
################################################################################

# Declare the package name:
atlas_subdir( FTK_RecTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasPolicy
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tools/PyJobTransforms
                          DetectorDescription/GeoPrimitives
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVxEdmCnv
                          Trigger/TrigFTK/FTK_DataProviderInterfaces
                          Trigger/TrigFTK/FTK_RecToolInterfaces
                          Trigger/TrigFTK/TrigFTK_RawData
			  Trigger/TrigFTK/TrigFTKSim
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetDetDescr/SCT_ModuleDistortions
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkTools/TrkAmbiguityProcessor
                          PRIVATE
                          Event/EventPrimitives
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          InnerDetector/InDetRecTools/SiClusterizationTool
                          Tracking/TrkDetDescr/TrkSurfaces )


# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Eigen )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( FTK_RecToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS FTK_RecTools
		   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES}  ${CLHEP_LIBRARIES} AthenaBaseComps GeoPrimitives xAODTracking TrkTrack VxVertex FTK_DataProviderInterfaces TrigFTK_RawData TrkVxEdmCnvLib TrigFTKSimLib InDetCondTools InDetPrepRawData InDetRIO_OnTrack TrkParameters TrkToolInterfaces TrkAmbiguityProcessorLib StoreGateLib SGtests EventPrimitives InDetIdentifier InDetReadoutGeometry SiClusterizationToolLib TrkSurfaces PixelConditionsData)

atlas_add_component( FTK_RecTools
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		     PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES}  ${CLHEP_LIBRARIES} AthenaBaseComps GeoPrimitives xAODTracking TrkTrack VxVertex TrkVxEdmCnvLib FTK_DataProviderInterfaces TrigFTK_RawData TrigFTKSimLib TrkParameters FTK_RecToolsLib InDetCondTools InDetPrepRawData InDetRIO_OnTrack TrkParameters TrkToolInterfaces TrkAmbiguityProcessorLib StoreGateLib SGtests EventPrimitives InDetIdentifier InDetReadoutGeometry SiClusterizationToolLib TrkSurfaces PixelConditionsData)


