# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
  atlas_add_test( ${testName}
                  SCRIPT athena.py TrkVertexSeedFinderTools/${testName}_test.py
                  PROPERTIES TIMEOUT 300
                  LOG_IGNORE_PATTERN " INFO |WARNING |^\\|-|found service|Adding private|^ +[+]|HepPDT Version|class DataHeader|class PoolToken" )
endfunction (run_seed_test)

run_seed_test( DummySeedFinder )
run_seed_test( ZScanSeedFinder )
run_seed_test( CrossDistancesSeedFinder )
run_seed_test( IndexedCrossDistancesSeedFinder )
run_seed_test( TrackDensitySeedFinder )
run_seed_test( MCTrueSeedFinder )
