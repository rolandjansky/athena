################################################################################
# Package: TrkResidualPullCalculator
################################################################################

# Declare the package name:
atlas_subdir( TrkResidualPullCalculator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkTools/TrkToolInterfaces )

find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrkResidualPullCalculator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasDetDescr EventPrimitives GaudiKernel TrkEventPrimitives TrkEventUtils TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkToolInterfaces )

