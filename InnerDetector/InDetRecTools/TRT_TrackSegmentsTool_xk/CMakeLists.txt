################################################################################
# Package: TRT_TrackSegmentsTool_xk
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrackSegmentsTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Event/EventInfo
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TRT_TrackSegmentsTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel InDetReadoutGeometry InDetPrepRawData InDetRIO_OnTrack InDetRecToolInterfaces TrkGeometry TrkEventPrimitives TrkParameters TrkToolInterfaces EventInfo TrkSurfaces TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkExInterfaces )

# Install files from the package:
atlas_install_headers( TRT_TrackSegmentsTool_xk )

