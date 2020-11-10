# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexSeedFinderTools )

# External dependencies:
find_package( HepPDT )

atlas_add_library( TrkVertexSeedFinderToolsLib
                   TrkVertexSeedFinderTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkVertexSeedFinderTools
                   LINK_LIBRARIES AthenaKernel GaudiKernel AthenaBaseComps TrkVertexFitterInterfaces
                   TrkVertexSeedFinderUtilsLib xAODEventInfo GeneratorObjects AtlasHepMCLib CxxUtils )

# Component(s) in the package:
atlas_add_component( TrkVertexSeedFinderTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} AtlasHepMCLib EventPrimitives GaudiKernel GeneratorObjects
                     GeoPrimitives StoreGateLib TestTools TrkEventPrimitives TrkParameters TrkTrack
                     TrkVertexFitterInterfaces TrkVertexSeedFinderToolsLib TrkVertexSeedFinderUtilsLib TruthHelper )


# Install files from the package:
atlas_install_joboptions( share/*.py )

# Tests in the package:
function( run_seed_test testName )
  cmake_parse_arguments( ARG "" "COMMAND;ARG" "" ${ARGN} )

  if( ARG_COMMAND )
    set( _command ${ARG_COMMAND} )
  else()
     set( _command athena.py )
  endif()

  if( ARG_ARG )
    set( _arg ${ARG_ARG} )
  else()
    set( _arg TrkVertexSeedFinderTools/${testName}_test.py )
  endif()

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/seed_test.sh.in
                  ${CMAKE_CURRENT_BINARY_DIR}/seed_${testName}.sh
                  @ONLY )
  atlas_add_test( ${testName}
                  SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/seed_${testName}.sh
                  PROPERTIES TIMEOUT 300
                  LOG_IGNORE_PATTERN " INFO |WARNING |found service|Adding private|^ +[+]|HepPDT Version|class DataHeader|class PoolToken" )
endfunction (run_seed_test)

run_seed_test( DummySeedFinder )
run_seed_test( ZScanSeedFinder )
run_seed_test( CrossDistancesSeedFinder )
run_seed_test( IndexedCrossDistancesSeedFinder )
run_seed_test( TrackDensitySeedFinder )
run_seed_test( MCTrueSeedFinder )
