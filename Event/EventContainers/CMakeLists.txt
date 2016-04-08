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

# Component(s) in the package:
atlas_add_executable( testIdCont
                      src/ID_ContainerTest.cxx
                      LINK_LIBRARIES Identifier GaudiKernel SGTools )

# Install files from the package:
atlas_install_headers( EventContainers )

