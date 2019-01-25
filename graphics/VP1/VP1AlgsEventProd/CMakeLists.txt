################################################################################
# Package: VP1AlgsEventProd
################################################################################

# Declare the package name:
atlas_subdir( VP1AlgsEventProd )

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
atlas_add_component( VP1AlgsEventProd
   VP1AlgsEventProd/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel StorageSvc EventInfo PathResolver
   VP1UtilsBase )
