################################################################################
# Package: TRT_Rec
################################################################################

# Declare the package name:
atlas_subdir( TRT_Rec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          GaudiKernel
                          Reconstruction/iPat/iPatInterfaces
                          Event/EventContainers
                          PRIVATE
                          DetectorDescription/RoiDescriptor
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/iPat/iPatTrack
                          Reconstruction/iPat/iPatTrackParameters
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TRT_Rec
                     src/TRT_Recon.cxx
                     src/TRT_DataSelector.cxx
                     src/TRT_Histogrammer.cxx
                     src/TRT_Fit.cxx
                     src/TRT_Prediction.cxx
                     src/TRT_Predictor.cxx
                     src/TRT_RoadData.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives IRegionSelector Identifier GaudiKernel iPatInterfaces RoiDescriptor InDetIdentifier InDetReadoutGeometry InDetPrepRawData iPatTrack iPatTrackParameters TrkGeometry TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkExInterfaces TrkExUtils TrkToolInterfaces EventContainers )

# Install files from the package:
atlas_install_headers( TRT_Rec )

