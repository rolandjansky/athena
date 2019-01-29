################################################################################
# Package: VP1AlgsBatch
################################################################################

# Declare the package name:
atlas_subdir( VP1AlgsBatch )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   GaudiKernel
   PRIVATE
   Database/APR/StorageSvc
   Event/EventInfo
   Tools/PathResolver
   graphics/VP1/VP1UtilsBase )

# Component(s) in the package:
atlas_add_component( VP1AlgsBatch
   VP1AlgsBatch/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel StorageSvc EventInfo PathResolver
   VP1UtilsBase )

# Install files from the package:
atlas_install_runtime( share/*.vp1 )
