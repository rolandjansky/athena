################################################################################
# Package: AthenaRootKernel
################################################################################

# Declare the package name:
atlas_subdir( AthenaRootKernel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelRoot
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( AthenaRootKernel
                   src/*.cxx
                   PUBLIC_HEADERS AthenaRootKernel
                   LINK_LIBRARIES DataModelRoot GaudiKernel )

