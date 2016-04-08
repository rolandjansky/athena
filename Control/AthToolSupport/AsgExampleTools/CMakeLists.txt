################################################################################
# Package: AsgExampleTools
################################################################################

# Declare the package name:
atlas_subdir( AsgExampleTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( AsgExampleTools
                     src/*.cxx
                     Root/AsgHelloTool.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools AthenaBaseComps GaudiKernel )

# Install files from the package:
atlas_install_headers( AsgExampleTools )

