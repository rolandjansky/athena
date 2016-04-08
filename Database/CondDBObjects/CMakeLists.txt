################################################################################
# Package: CondDBObjects
################################################################################

# Declare the package name:
atlas_subdir( CondDBObjects )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( CondDBObjects
                   src/*.cxx
                   PUBLIC_HEADERS CondDBObjects
                   LINK_LIBRARIES SGTools GaudiKernel )

