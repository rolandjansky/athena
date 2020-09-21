# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexSeedFinderUtils )

# External dependencies:
find_package( FFTW )
find_package( ROOT COMPONENTS Core Tree Hist )

# Component(s) in the package:
atlas_add_library( TrkVertexSeedFinderUtilsLib
   TrkVertexSeedFinderUtils/*.h
   INTERFACE
   PUBLIC_HEADERS TrkVertexSeedFinderUtils
   INCLUDE_DIRS ${FFTW_INCLUDE_DIRS}
   LINK_LIBRARIES ${FFTW_LIBRARIES} GaudiKernel AthenaBaseComps
   TrkVertexFitterInterfaces TrkParameters GeoPrimitives xAODTracking
   MagFieldConditions )

atlas_add_component( TrkVertexSeedFinderUtils
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${FFTW_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${FFTW_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel
   AthenaBaseComps TrkParticleBase TrkParameters TrkLinks TrkTrack
   TrkVertexFitterInterfaces TrkEventPrimitives xAODTracking xAODTruth
   InDetTrackSelectionToolLib BeamSpotConditionsData EventPrimitives
   GeoPrimitives MagFieldElements TrkVertexSeedFinderUtilsLib )

# Test(s) in the package.
atlas_add_test( NewtonTrkDistanceFinder_test
   SOURCES test/NewtonTrkDistanceFinder_test.cxx
           src/NewtonTrkDistanceFinder.cxx
           src/PointOnTrack.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools MagFieldConditions
   MagFieldElements CxxUtils GaudiKernel PathResolver SGTools StoreGateLib
   TrkEventPrimitives TrkVertexSeedFinderUtilsLib
   LOG_IGNORE_PATTERN "reading the map|field map" )

atlas_add_test( Trk2dDistanceSeeder_test
   SOURCES test/Trk2dDistanceSeeder_test.cxx
           src/Trk2dDistanceSeeder.cxx
           src/PointOnTrack.cxx
           src/TwoTracks.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils GaudiKernel PathResolver
   SGTools StoreGateLib MagFieldConditions MagFieldElements TrkParameters
   TrkVertexSeedFinderUtilsLib
   LOG_IGNORE_PATTERN "reading the map|field map" )


atlas_add_test( SeedNewtonTrkDistanceFinder_test
   SOURCES test/SeedNewtonTrkDistanceFinder_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils GaudiKernel PathResolver
   SGTools StoreGateLib MagFieldConditions MagFieldElements
   TrkVertexSeedFinderUtilsLib
   LOG_IGNORE_PATTERN "reading the map|field map" )

atlas_add_test( Trk2DDistanceFinder_test
   SOURCES test/Trk2DDistanceFinder_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils GaudiKernel PathResolver
   SGTools StoreGateLib MagFieldConditions MagFieldElements
   TrkVertexSeedFinderUtilsLib
   LOG_IGNORE_PATTERN "reading the map|field map" )

atlas_add_test( GaussianTrackDensity_test
   SOURCES test/GaussianTrackDensity_test.cxx
           src/GaussianTrackDensity.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils GaudiKernel TrkTrack
   TrkEventPrimitives TrkVertexSeedFinderUtilsLib )
# Needed to avoid spurious ubsan warnings.
set_target_properties( TrkVertexSeedFinderUtils_GaussianTrackDensity_test PROPERTIES ENABLE_EXPORTS True )

atlas_add_test( SimpleVertexClusterFinder_test
   SOURCES test/SimpleVertexClusterFinder_test.cxx
           src/SimpleVertexClusterFinder.cxx
           src/VertexImageMaker.cxx
           src/VertexImage.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils GaudiKernel
   EventPrimitives TrkVertexSeedFinderUtilsLib )

atlas_add_test( Mode3dFromFsmw1dFinder_test
   SOURCES test/Mode3dFromFsmw1dFinder_test.cxx
           src/Mode3dFromFsmw1dFinder.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils GaudiKernel TrkParameters
   TrkVertexSeedFinderUtilsLib )
# Needed to avoid spurious ubsan warnings.
set_target_properties( TrkVertexSeedFinderUtils_Mode3dFromFsmw1dFinder_test PROPERTIES ENABLE_EXPORTS True )

atlas_add_test( Mode3dTo1dFinder_test
   SOURCES test/Mode3dTo1dFinder_test.cxx
           src/Mode3dTo1dFinder.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils GaudiKernel TrkParameters
   TrkVertexSeedFinderUtilsLib )

# Install files from the package.
atlas_install_joboptions( share/*.py share/*.txt )
