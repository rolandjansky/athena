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
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          LumiBlock/LumiBlockData
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          Tracking/TrkEvent/TrkEventPrimitives )

# Component(s) in the package:
atlas_add_component( TRT_DriftCircleOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRIO_OnTrack TrkRIO_OnTrack TrkToolInterfaces InDetReadoutGeometry TRT_ReadoutGeometry TrkEventPrimitives TrkRIO_OnTrack LumiBlockData TRT_DriftFunctionToolLib )

# Install files from the package:
atlas_install_headers( TRT_DriftCircleOnTrackTool )

