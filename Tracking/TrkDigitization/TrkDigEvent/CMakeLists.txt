################################################################################
# Package: TrkDigEvent
################################################################################

# Declare the package name:
atlas_subdir( TrkDigEvent )
# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils )

# Component(s) in the package:
atlas_add_library( TrkDigEvent
                     src/*.cxx
                     LINK_LIBRARIES GeoPrimitives TrkDetDescrUtils TrkSurfaces )
# Install files from the package:
atlas_install_headers( TrkDigEvent )

