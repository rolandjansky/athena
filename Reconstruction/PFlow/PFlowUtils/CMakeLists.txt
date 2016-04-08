################################################################################
# Package: PFlowUtils
################################################################################

# Declare the package name:
atlas_subdir( PFlowUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODPFlow
                          PRIVATE
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( PFlowUtils
                     Root/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools xAODPFlow GaudiKernel )

# Install files from the package:
atlas_install_headers( PFlowUtils )

