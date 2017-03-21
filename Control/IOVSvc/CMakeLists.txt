################################################################################
# Package: IOVSvc
################################################################################

# Declare the package name:
atlas_subdir( IOVSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Event/EventInfo 
			  Event/xAOD/xAODEventInfo )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( IOVSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS IOVSvc
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES TestTools EventInfo xAODEventInfo)

atlas_add_component( IOVSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib )

atlas_add_test( IOVSvcTool_test
                SOURCES
                test/IOVSvcTool_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib
                EXTRA_PATTERNS "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG|^DetectorStore_Impl +DEBUG|^ConditionStore|^StoreGateSvc +DEBUG" 
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_joboptions( share/IOVSvc.txt share/IOVSvc.py )

