################################################################################
# Package: EventContainers
################################################################################

# Declare the package name:
atlas_subdir( EventContainers )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          Control/AthenaKernel )

# Install files from the package:

atlas_add_library( EventContainers 
   src/*.cxx
   PUBLIC_HEADERS EventContainers
   LINK_LIBRARIES AthenaKernel Identifier GaudiKernel )


atlas_add_dictionary( EventContainersDict
                      EventContainers/EventContainersDict.h
                      EventContainers/selection.xml
                      LINK_LIBRARIES EventContainers )


atlas_add_test( IdCont SOURCES test/ID_ContainerTest.cxx
                INCLUDE_DIRS src test EventContainers
                LINK_LIBRARIES Identifier AthenaKernel GaudiKernel EventContainers
                LOG_IGNORE_PATTERN "elapsed"
               )
atlas_add_test( IdMTCont SOURCES test/IDMT_ContainerTest.cxx
                INCLUDE_DIRS src test EventContainers
                LINK_LIBRARIES Identifier AthenaKernel GaudiKernel EventContainers
                LOG_IGNORE_PATTERN "elapsed"
               )
atlas_add_test( IDStressTest SOURCES test/IDC_Realistic_Test.cxx
                INCLUDE_DIRS src test EventContainers
                LINK_LIBRARIES Identifier AthenaKernel GaudiKernel EventContainers
                LOG_IGNORE_PATTERN "elapsed|^no lock time|^deleted|^countHit|^lock time"
              )
atlas_add_test( IDCValueTest SOURCES test/IDCValueTest.cxx
                INCLUDE_DIRS src test EventContainers
                LINK_LIBRARIES Identifier AthenaKernel GaudiKernel EventContainers
              )
atlas_add_test( IDBenchTest SOURCES test/IDC_Benchmark.cxx
                INCLUDE_DIRS src test EventContainers
                LINK_LIBRARIES Identifier AthenaKernel GaudiKernel EventContainers
                LOG_IGNORE_PATTERN "time"
              )
