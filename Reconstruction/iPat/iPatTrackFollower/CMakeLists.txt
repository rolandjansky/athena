################################################################################
# Package: iPatTrackFollower
################################################################################

# Declare the package name:
atlas_subdir( iPatTrackFollower )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/iPat/iPatGeometry
                          Reconstruction/iPat/iPatInterfaces
                          Reconstruction/iPat/iPatTrack
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( iPatTrackFollower
                     src/LayerAssociation.cxx
                     src/LayerPrediction.cxx
                     src/LayerPredictor.cxx
                     src/SiClusterProperties.cxx
                     src/SiliconClusterMap.cxx
                     src/SiliconLayerAssociator.cxx
                     src/TrackBuilder.cxx
                     src/TrackFollower.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData iPatGeometry iPatInterfaces iPatTrack TrkParameters StoreGateLib SGtests GeoPrimitives InDetIdentifier InDetReadoutGeometry InDetRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkExInterfaces TrkExUtils TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( iPatTrackFollower )

