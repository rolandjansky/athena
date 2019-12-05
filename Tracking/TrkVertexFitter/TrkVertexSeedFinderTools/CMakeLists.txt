################################################################################
# Package: TrkVertexSeedFinderTools
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexSeedFinderTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexSeedFinderUtils
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          Generators/GenAnalysisTools/TruthHelper
                          Generators/GeneratorObjects
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Eigen )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkVertexSeedFinderTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel TrkVertexFitterInterfaces GeoPrimitives EventPrimitives xAODEventInfo TruthHelper GeneratorObjects TrkEventPrimitives TrkParameters TrkTrack VxVertex )

# Install files from the package:
atlas_install_headers( TrkVertexSeedFinderTools )


atlas_install_joboptions( share/*.py )


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
                  EXTRA_PATTERNS " INFO |WARNING |found service|Adding private|^ +[+]|HepPDT Version|class DataHeader|class PoolToken" )
endfunction (run_seed_test)


run_seed_test( DummySeedFinder )
run_seed_test( ZScanSeedFinder )
run_seed_test( CrossDistancesSeedFinder )
run_seed_test( IndexedCrossDistancesSeedFinder )
run_seed_test( TrackDensitySeedFinder )
run_seed_test( ImagingSeedFinder )
run_seed_test( MCTrueSeedFinder )
