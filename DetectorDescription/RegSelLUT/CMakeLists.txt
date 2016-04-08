################################################################################
# Package: RegSelLUT
################################################################################

# Declare the package name:
atlas_subdir( RegSelLUT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/Identifier
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( RegSelLUT
                   src/*.cxx
                   PUBLIC_HEADERS RegSelLUT
                   LINK_LIBRARIES SGTools Identifier GaudiKernel )

