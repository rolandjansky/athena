################################################################################
# Package: iPatTrackFitter
################################################################################

# Declare the package name:
atlas_subdir( iPatTrackFitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/iPat/iPatInterfaces
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkFitter/TrkiPatFitterUtils )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( iPatTrackFitter
                     src/TrackFitter.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel iPatInterfaces GeoPrimitives EventPrimitives TrkSurfaces TrkVolumes TrkDetDescrInterfaces TrkEventPrimitives TrkMeasurementBase VxVertex TrkExInterfaces TrkExUtils TrkiPatFitterUtils )

# Install files from the package:
atlas_install_headers( iPatTrackFitter )

