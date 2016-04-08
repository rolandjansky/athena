################################################################################
# Package: ToyConversion
################################################################################

# Declare the package name:
atlas_subdir( ToyConversion )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( ToyConversionLib
                   src/*.cxx
                   PUBLIC_HEADERS ToyConversion
                   LINK_LIBRARIES SGTools GaudiKernel )

atlas_add_component( ToyConversion
                     src/components/*.cxx
                     LINK_LIBRARIES SGTools GaudiKernel ToyConversionLib )

# Install files from the package:
atlas_install_joboptions( share/ToyConversionOpts.txt share/ToyConversionOpts.py )

