################################################################################
# Package: GeneratorFortranCommon
################################################################################

# Declare the package name:
atlas_subdir( GeneratorFortranCommon )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/CLIDSvc
   Control/SGTools
   GaudiKernel )

# Component(s) in the package:
atlas_add_library( GeneratorFortranCommonLib
   src/inituser.F src/useuser.F src/getaddr.cxx
   PUBLIC_HEADERS GeneratorFortranCommon
   LINK_LIBRARIES SGTools GaudiKernel )
