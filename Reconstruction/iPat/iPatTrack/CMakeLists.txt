################################################################################
# Package: iPatTrack
################################################################################

# Declare the package name:
atlas_subdir( iPatTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Reconstruction/iPat/iPatTrackParameters
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExUtils )

# Component(s) in the package:
atlas_add_library( iPatTrack
                   src/HitOnTrack.cxx
                   src/HitQuality.cxx
                   src/HitStatistics.cxx
                   src/HitStatus.cxx
                   src/Track.cxx
                   src/TrackEnvelope.cxx
                   src/TrackStatus.cxx
                   src/TruthAssociation.cxx
                   PUBLIC_HEADERS iPatTrack
                   LINK_LIBRARIES Identifier EventPrimitives iPatTrackParameters TrkParameters
                   PRIVATE_LINK_LIBRARIES TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkRIO_OnTrack TrkTrack TrkExUtils )

