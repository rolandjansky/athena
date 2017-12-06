# $Id: CMakeLists.txt 729159 2016-03-11 12:58:15Z krasznaa $
################################################################################
# Package: RngComps
################################################################################

# Declare the package name:
atlas_subdir( RngComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   AtlasTest/TestTools
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/StoreGate
   Event/EventInfo
   GaudiKernel
   Simulation/Tools/AtlasCLHEP_RandomGenerators )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( RngComps src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES}
   AthenaBaseComps AthenaKernel StoreGateLib EventInfo GaudiKernel
   AtlasCLHEP_RandomGenerators )

# Test(s) in the package:
set( RNGCOMPS_REFERENCE_TAG RngComps/RngCompsReference-01-00-02 ) 
 
atlas_add_test( AtRndmGen_test
   SOURCES test/AtRndmGen_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} TestTools AthenaKernel StoreGateLib
   GaudiKernel
   EXTRA_PATTERNS "//GP:|^IncidentSvc *DEBUG Adding .* listener|^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AtRanlux_test
   SOURCES test/AtRanlux_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} TestTools AthenaKernel StoreGateLib
   GaudiKernel
   EXTRA_PATTERNS "^IncidentSvc *DEBUG Adding .* listener|^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AtDSFMT_test
   SOURCES test/AtDSFMT_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} TestTools AthenaKernel StoreGateLib
   GaudiKernel
   EXTRA_PATTERNS "^IncidentSvc *DEBUG Adding .* listener|^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"  )
set_property( TEST RngComps_AtDSFMT_test_ctest APPEND PROPERTY
              ENVIRONMENT "ATLAS_REFERENCE_TAG=${RNGCOMPS_REFERENCE_TAG}" )

atlas_add_test( TestSeedRunEvent
   SCRIPT test/TestSeedRunEvent.sh
   EXTRA_PATTERNS "SGAudSvc +INFO Finalizing|Py:Athena +INFO executing ROOT6Setup|SGAudSvc +INFO Initializing|No duplicates have been found|duplicate entry.*ignored|^Py:ConfigurableDb WARNING|Read module info for|^ToolSvc.* INFO( finalize:)? Removing all tools|^CoreDumpSvc *INFO Handling signals|types added|including file|^(Mon|Tue|Wed|Thu|Fri|Sat|Sun) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)|local .* libGaudiKernelDict.so|^Number of My|^Py:Athena +INFO using release|^StoreGateSvc +INFO Start|^Py:AthenaDsoDb +INFO could not install alias|Bindings.py:660newobj = object.__new__|Updating ROOT::Reflex::PluginService::SetDebug|DEBUG Calling destructor|DEBUG Property update for OutputLevel"
   ENVIRONMENT "ATLAS_REFERENCE_TAG=${RNGCOMPS_REFERENCE_TAG}"
   PROPERTIES TIMEOUT 500 )

atlas_add_test( RNGWrapper_test
   SOURCES test/RNGWrapper_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} TestTools AthenaKernel StoreGateLib
   GaudiKernel AtlasCLHEP_RandomGenerators )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
