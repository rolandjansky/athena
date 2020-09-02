################################################################################
# Package: TRT_SeededTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( TRT_SeededTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          PRIVATE
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecEvent/SiSPSeededTrackFinderData
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack 
                          Tracking/TrkEvent/TrkCaloClusterROI
                          DetectorDescription/RoiDescriptor )

# Component(s) in the package:
atlas_add_component( TRT_SeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces TrkSegment TrkTrack TrkExInterfaces TrkFitterInterfaces InDetRIO_OnTrack SiSPSeededTrackFinderData TrkEventPrimitives TrkPseudoMeasurementOnTrack TrkEventUtils BeamSpotConditionsData RoiDescriptor TrkCaloClusterROI)

# Install files from the package:
atlas_install_headers( TRT_SeededTrackFinder )

