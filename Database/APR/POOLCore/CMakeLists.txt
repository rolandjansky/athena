################################################################################
# Package: POOLCore
################################################################################

# Declare the package name:
atlas_subdir( POOLCore )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
      GaudiKernel
      Control/AthenaBaseComps
   PRIVATE
   )

# Component(s) in the package:
atlas_add_library( POOLCore
                   src/*.cpp
                   PUBLIC_HEADERS POOLCore
                   LINK_LIBRARIES GaudiKernel AthenaBaseComps
                   )

