################################################################################
# Package: TrkGaussianSumFilter
################################################################################

# Declare the package name:
atlas_subdir( TrkGaussianSumFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMultiComponentStateOnSurface
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Event/xAOD/xAODEventInfo
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkCaloCluster_OnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkGaussianSumFilter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps DataModel GeoPrimitives EventPrimitives GaudiKernel TrkDetDescrInterfaces TrkGeometry TrkSurfaces TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkMultiComponentStateOnSurface TrkParameters TrkExInterfaces TrkExUtils TrkFitterInterfaces TrkFitterUtils TrkToolInterfaces CxxUtils xAODEventInfo PathResolver TrkDetElementBase TrkVolumes TrkCaloCluster_OnTrack TrkMeasurementBase TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( TrkGaussianSumFilter )
atlas_install_runtime( Data/*.par )

