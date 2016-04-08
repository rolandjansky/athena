################################################################################
# Package: TrkVertexSeedFinderUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexSeedFinderUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          Event/EventPrimitives
                          Event/xAOD/xAODTruth
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( Eigen )
find_package( FFTW )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkVertexSeedFinderUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FFTW_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives xAODTracking GaudiKernel TrkParameters TrkParticleBase VxVertex EventPrimitives xAODTruth MagFieldInterfaces TrkEventPrimitives TrkLinks TrkTrack TrkVertexFitterInterfaces )

# Install files from the package:
atlas_install_headers( TrkVertexSeedFinderUtils )

