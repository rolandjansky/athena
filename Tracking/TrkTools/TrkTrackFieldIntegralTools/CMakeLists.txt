################################################################################
# Package: TrkTrackFieldIntegralTools
################################################################################

# Declare the package name:
atlas_subdir( TrkTrackFieldIntegralTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrkTrackFieldIntegralTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkDetDescrInterfaces TrkGeometry TrkMaterialOnTrack TrkMeasurementBase TrkPseudoMeasurementOnTrack TrkTrack TrkToolInterfaces )

