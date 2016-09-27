################################################################################
# Package: TRT_SeededTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( TRT_SeededTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          PRIVATE
                          Control/CxxUtils
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack )

# Component(s) in the package:
atlas_add_component( TRT_SeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces TrkSegment TrkTrack TrkExInterfaces TrkFitterInterfaces CxxUtils InDetRIO_OnTrack TrkEventPrimitives TrkPseudoMeasurementOnTrack )

# Install files from the package:
atlas_install_headers( TRT_SeededTrackFinder )

