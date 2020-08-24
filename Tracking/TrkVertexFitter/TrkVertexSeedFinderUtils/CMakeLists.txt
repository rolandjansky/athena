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
   MagneticField/MagFieldConditions   # Exposed by NewtonTrkDistanceFinder.h and Trk2dDistanceSeeder.h
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkParticleBase
   Tracking/TrkEvent/VxVertex
   InnerDetector/InDetConditions/BeamSpotConditionsData
   PRIVATE
   AtlasTest/TestTools
   Event/EventPrimitives
   Event/xAOD/xAODTruth
   InnerDetector/InDetConditions/InDetBeamSpotService
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   MagneticField/MagFieldElements
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkLinks
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkVertexFitter/TrkVertexFitterInterfaces 
   Tools/PathResolver )

# External dependencies:
find_package( FFTW )
find_package( ROOT COMPONENTS Core Tree Hist )

# Component(s) in the package:
atlas_add_library( TrkVertexSeedFinderUtilsLib
   TrkVertexSeedFinderUtils/*.h
   INTERFACE
   PUBLIC_HEADERS TrkVertexSeedFinderUtils
   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkParameters GeoPrimitives
   VxVertex TrkParticleBase xAODTracking BeamSpotConditionsData )

atlas_add_component( TrkVertexSeedFinderUtils
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FFTW_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} EventPrimitives
   xAODTruth InDetTrackSelectionToolLib MagFieldConditions MagFieldElements TrkEventPrimitives
   TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib )

atlas_install_joboptions( share/*.py )
atlas_install_joboptions( share/*.txt )


atlas_add_test( NewtonTrkDistanceFinder_test
                SOURCES test/NewtonTrkDistanceFinder_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} TestTools
                EventPrimitives
                xAODTruth InDetTrackSelectionToolLib MagFieldConditions MagFieldElements PathResolver TrkEventPrimitives
                TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib
                LOG_IGNORE_PATTERN "reading the map|field map" )


atlas_add_test( Trk2dDistanceSeeder_test
                SOURCES test/Trk2dDistanceSeeder_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} TestTools
                EventPrimitives
                xAODTruth InDetTrackSelectionToolLib MagFieldConditions MagFieldElements PathResolver TrkEventPrimitives
                TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib
                LOG_IGNORE_PATTERN "reading the map|field map" )


atlas_add_test( SeedNewtonTrkDistanceFinder_test
                SOURCES test/SeedNewtonTrkDistanceFinder_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} TestTools
                EventPrimitives
                xAODTruth InDetTrackSelectionToolLib MagFieldConditions MagFieldElements PathResolver TrkEventPrimitives
                TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib
                LOG_IGNORE_PATTERN "reading the map|field map" )


atlas_add_test( Trk2DDistanceFinder_test
                SOURCES test/Trk2DDistanceFinder_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} TestTools
                EventPrimitives
                xAODTruth InDetTrackSelectionToolLib MagFieldConditions MagFieldElements PathResolver TrkEventPrimitives
                TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib
                LOG_IGNORE_PATTERN "reading the map|field map" )


atlas_add_test( GaussianTrackDensity_test
                SOURCES test/GaussianTrackDensity_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} TestTools
                EventPrimitives
                xAODTruth InDetTrackSelectionToolLib MagFieldConditions MagFieldElements TrkEventPrimitives
                TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib )
# Needed to avoid spurious ubsan warnings.
set_target_properties( TrkVertexSeedFinderUtils_GaussianTrackDensity_test PROPERTIES ENABLE_EXPORTS True )


atlas_add_test( SimpleVertexClusterFinder_test
                SOURCES test/SimpleVertexClusterFinder_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} TestTools
                EventPrimitives
                xAODTruth InDetTrackSelectionToolLib MagFieldConditions MagFieldElements TrkEventPrimitives
                TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib )


atlas_add_test( Mode3dFromFsmw1dFinder_test
                SOURCES test/Mode3dFromFsmw1dFinder_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} TestTools
                EventPrimitives
                xAODTruth InDetTrackSelectionToolLib MagFieldConditions MagFieldElements TrkEventPrimitives
                TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib )
# Needed to avoid spurious ubsan warnings.
set_target_properties( TrkVertexSeedFinderUtils_Mode3dFromFsmw1dFinder_test PROPERTIES ENABLE_EXPORTS True )


atlas_add_test( Mode3dTo1dFinder_test
                SOURCES test/Mode3dTo1dFinder_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} TestTools
                EventPrimitives
                xAODTruth InDetTrackSelectionToolLib MagFieldConditions MagFieldElements TrkEventPrimitives
                TrkLinks TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib )
