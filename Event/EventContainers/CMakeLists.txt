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
                          Control/CLIDSvc
                          Control/SGTools )

# Install files from the package:

atlas_add_library( EventContainers 
   src/IdentifiableCacheBase.cxx
   PUBLIC_HEADERS EventContainers
   LINK_LIBRARIES GaudiKernel SGTools )

atlas_add_test( IdCont SOURCES test/ID_ContainerTest.cxx
                INCLUDE_DIRS src test EventContainers
                LINK_LIBRARIES Identifier GaudiKernel SGTools EventContainers
                EXTRA_PATTERNS "elapsed"
               )
atlas_add_test( IdMTCont SOURCES test/IDMT_ContainerTest.cxx
                INCLUDE_DIRS src test EventContainers
                LINK_LIBRARIES Identifier GaudiKernel SGTools EventContainers
                EXTRA_PATTERNS "elapsed"
               )
