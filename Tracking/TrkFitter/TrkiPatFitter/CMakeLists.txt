################################################################################
# Package: TrkiPatFitter
################################################################################

# Declare the package name:
atlas_subdir( TrkiPatFitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkiPatFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkEvent/TrkTrackSummary
                          PRIVATE
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          DetectorDescription/Identifier
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrkiPatFitter
                     src/iPatFitter.cxx
                     src/iPatGlobalFitter.cxx
                     src/MaterialAllocator.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel TrkParameters TrkTrack TrkFitterInterfaces TrkiPatFitterUtils EventPrimitives TrkDetDescrInterfaces TrkDetElementBase TrkGeometry TrkSurfaces Identifier TrkMaterialOnTrack TrkMeasurementBase TrkRIO_OnTrack MuonCompetingRIOsOnTrack TrkExInterfaces TrkExUtils TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TrkiPatFitter )
