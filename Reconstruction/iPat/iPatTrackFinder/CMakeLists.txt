################################################################################
# Package: iPatTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( iPatTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/iPat/iPatInterfaces
                          Reconstruction/iPat/iPatTrack
                          Event/EventContainers
                          PRIVATE
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          MagneticField/MagFieldInterfaces
                          Reconstruction/iPat/iPatGeometry
                          Reconstruction/iPat/iPatTrackParameters
                          Reconstruction/iPat/iPatUtility
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/VxVertex )

# Component(s) in the package:
atlas_add_component( iPatTrackFinder
                     src/AssignedHits.cxx
                     src/CandidateBuilder.cxx
                     src/CombinationMaker.cxx
                     src/FinderTolerances.cxx
                     src/Point.cxx
                     src/PointGroup.cxx
                     src/PointManager.cxx
                     src/PrimaryCandidate.cxx
                     src/SecondaryCandidate.cxx
                     src/TrackFinder.cxx
                     src/TrackManager.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier EventPrimitives GaudiKernel InDetPrepRawData iPatInterfaces iPatTrack InDetReadoutGeometry MagFieldInterfaces iPatGeometry iPatTrackParameters iPatUtility TrkPrepRawData TrkSpacePoint VxVertex EventContainers)

# Install files from the package:
atlas_install_headers( iPatTrackFinder )

