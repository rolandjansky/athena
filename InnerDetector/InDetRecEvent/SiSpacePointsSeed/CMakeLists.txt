################################################################################
# Package: SiSpacePointsSeed
################################################################################

# Declare the package name:
atlas_subdir( SiSpacePointsSeed )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkEvent/TrkSpacePoint
                          PRIVATE
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( SiSpacePointsSeed
                   src/*.cxx
                   PUBLIC_HEADERS SiSpacePointsSeed
                   LINK_LIBRARIES TrkSpacePoint
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

