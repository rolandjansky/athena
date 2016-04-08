################################################################################
# Package: TrkSegmentConverter
################################################################################

# Declare the package name:
atlas_subdir( TrkSegmentConverter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces )

# Component(s) in the package:
atlas_add_component( TrkSegmentConverter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkToolInterfaces EventPrimitives TrkSurfaces TrkEventPrimitives TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkExInterfaces TrkFitterInterfaces )

# Install files from the package:
atlas_install_headers( TrkSegmentConverter )

