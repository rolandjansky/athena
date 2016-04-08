################################################################################
# Package: TRT_DriftCircleOnTrackTool
################################################################################

# Declare the package name:
atlas_subdir( TRT_DriftCircleOnTrackTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          Tracking/TrkEvent/TrkEventPrimitives )

# Component(s) in the package:
atlas_add_component( TRT_DriftCircleOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRIO_OnTrack TrkToolInterfaces InDetReadoutGeometry TrkEventPrimitives )

# Install files from the package:
atlas_install_headers( TRT_DriftCircleOnTrackTool )

