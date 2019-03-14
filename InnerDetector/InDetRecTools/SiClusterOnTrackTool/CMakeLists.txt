################################################################################
# Package: SiClusterOnTrackTool
################################################################################

# Declare the package name:
atlas_subdir( SiClusterOnTrackTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetDetDescr/SCT_ModuleDistortions
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkTools/TrkAmbiguityProcessor
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkUtilityPackages/TrkNeuralNetworkUtils
                          Control/StoreGate
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
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( SiClusterOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps CxxUtils AthenaPoolUtilities GeoPrimitives GaudiKernel InDetPrepRawData InDetRIO_OnTrack TrkParameters TrkToolInterfaces TrkAmbiguityProcessorLib  StoreGateLib SGtests EventPrimitives InDetIdentifier InDetReadoutGeometry SiClusterizationToolLib TrkSurfaces TrkRIO_OnTrack PixelConditionsData TrkNeuralNetworkUtils )

# Install files from the package:
atlas_install_headers( SiClusterOnTrackTool )

