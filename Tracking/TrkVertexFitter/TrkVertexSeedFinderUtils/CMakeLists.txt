# $Id: CMakeLists.txt 769975 2016-08-25 07:41:26Z krasznaa $
################################################################################
# Package: TrkVertexSeedFinderUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexSeedFinderUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   DetectorDescription/GeoPrimitives
   Event/xAOD/xAODTracking
   GaudiKernel
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkParticleBase
   Tracking/TrkEvent/VxVertex
   InnerDetector/InDetConditions/BeamSpotConditionsData
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
find_package( FFTW )
find_package( ROOT COMPONENTS Core Tree Hist )

# Component(s) in the package:
atlas_add_library( TrkVertexSeedFinderUtilsLib
   TrkVertexSeedFinderUtils/*.h
   INTERFACE
   PUBLIC_HEADERS TrkVertexSeedFinderUtils
   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkParameters GeoPrimitives
   VxVertex TrkParticleBase xAODTracking )

atlas_add_component( TrkVertexSeedFinderUtils
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FFTW_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} EventPrimitives
   xAODTruth InDetTrackSelectionToolLib MagFieldInterfaces TrkEventPrimitives
   TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib )

atlas_install_joboptions( share/*.py )
