################################################################################
# Package: TrkDriftCircleMath
################################################################################

# Declare the package name:
atlas_subdir( TrkDriftCircleMath )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CxxUtils
                          GaudiKernel
 )			  

# Component(s) in the package:
atlas_add_library( TrkDriftCircleMath
                   src/*.cxx
                   PUBLIC_HEADERS TrkDriftCircleMath
                   LINK_LIBRARIES CxxUtils GaudiKernel)

