################################################################################
# Package: TRT_TrackSegmentsFinder
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrackSegmentsFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkEvent/TrkCaloClusterROI
                          Tracking/TrkEvent/TrkSegment )

# Component(s) in the package:
atlas_add_component( TRT_TrackSegmentsFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces InDetReadoutGeometry TrkCaloClusterROI TrkSegment )

# Install files from the package:
atlas_install_headers( TRT_TrackSegmentsFinder )

