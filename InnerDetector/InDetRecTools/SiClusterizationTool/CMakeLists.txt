################################################################################
# Package: SiClusterizationTool
################################################################################

# Declare the package name:
atlas_subdir( SiClusterizationTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/DetDescrCond/DetDescrCondTools
                          Event/EventInfo
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkUtilityPackages/TrkNeuralNetworkUtils )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( SiClusterizationToolLib
                   src/*.cxx
                   PUBLIC_HEADERS SiClusterizationTool
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier EventPrimitives GaudiKernel InDetSimData InDetIdentifier InDetReadoutGeometry InDetRawData InDetPrepRawData InDetRecToolInterfaces TrkParameters TrkNeuralNetworkUtilsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaPoolUtilities AtlasDetDescr EventInfo TrkSurfaces TrkEventPrimitives VxVertex )

atlas_add_component( SiClusterizationTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier EventPrimitives GaudiKernel InDetSimData InDetIdentifier InDetReadoutGeometry InDetRawData InDetPrepRawData InDetRecToolInterfaces TrkParameters AthenaPoolUtilities AtlasDetDescr EventInfo TrkSurfaces TrkEventPrimitives VxVertex TrkNeuralNetworkUtilsLib SiClusterizationToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.root )

