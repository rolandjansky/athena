################################################################################
# Package: iPatRecAlgs
################################################################################

# Declare the package name:
atlas_subdir( iPatRecAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/iPat/iPatRecEvent
                          Reconstruction/iPat/iPatTrack
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODEventInfo
                          Generators/GenInterfaces
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/iPat/iPatInterfaces
                          Reconstruction/iPat/iPatTrackParameters
                          Reconstruction/iPat/iPatUtility
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkFitter/TrkFitterInterfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( iPatRecAlgs
                     src/iPatRec.cxx
                     src/iPatShortTracks.cxx
                     src/iPatStatistics.cxx
                     src/iPatTrackTruthAssociator.cxx
                     src/IntersectorTest.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel iPatRecEvent iPatTrack GeoPrimitives xAODEventInfo InDetPrepRawData iPatInterfaces iPatTrackParameters iPatUtility TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkSpacePoint TrkTrack TrkTruthData TrkExInterfaces TrkExUtils TrkFitterInterfaces )

# Install files from the package:
atlas_install_headers( iPatRecAlgs )

