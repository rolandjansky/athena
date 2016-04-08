################################################################################
# Package: ISF_FatrasToolsID
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasToolsID )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModel
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetSimEvent
                          Simulation/ISF/ISF_Fatras/ISF_FatrasInterfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/SiClusterizationTool
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrModel
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
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
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel DataModel Identifier GaudiKernel InDetSimEvent ISF_FatrasInterfaces TrkParameters TrkExInterfaces StoreGateLib SGtests AtlasDetDescr IdDictDetDescr TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry InDetPrepRawData InDetRIO_OnTrack SiClusterizationToolLib ISF_Event ISF_Interfaces ISF_FatrasDetDescrModel TrkDetElementBase TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkRIO_OnTrack TrkTrack TrkExUtils TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( ISF_FatrasToolsID )

