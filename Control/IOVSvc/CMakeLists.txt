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
                          Control/AthContainersInterfaces
                          Database/PersistentDataModel
                          Event/xAOD/xAODEventInfo )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( IOVSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS IOVSvc
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools GaudiKernel StoreGateLib SGtests xAODEventInfo PersistentDataModel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools )

atlas_add_component( IOVSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} IOVSvcLib )

atlas_add_test( IOVSvcTool_test
                SOURCES
                test/IOVSvcTool_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES IOVSvcLib
                LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG" 
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( IOVSvc_test
                SOURCES
                test/IOVSvc_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES IOVSvcLib
                LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG" 
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_joboptions( share/IOVSvc.txt share/IOVSvc.py )

