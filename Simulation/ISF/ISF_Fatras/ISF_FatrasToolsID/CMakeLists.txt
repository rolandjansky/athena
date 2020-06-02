################################################################################
# Package: ISF_FatrasToolsID
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasToolsID )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDictDetDescr
                          DetectorDescription/Identifier
			  DetectorDescription/GeoPrimitives
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/SiClusterizationTool
                          InnerDetector/InDetSimEvent
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrModel
                          Simulation/ISF/ISF_Fatras/ISF_FatrasInterfaces
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ISF_FatrasToolsID
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthContainers Identifier GaudiKernel InDetSimEvent ISF_FatrasInterfaces TrkParameters TrkExInterfaces StoreGateLib SGtests AtlasDetDescr IdDictDetDescr TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetPrepRawData InDetRIO_OnTrack SiClusterizationToolLib ISF_Event ISF_InterfacesLib ISF_FatrasDetDescrModel TrkDetElementBase TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkRIO_OnTrack TrkTrack TrkExUtils TrkToolInterfaces )

