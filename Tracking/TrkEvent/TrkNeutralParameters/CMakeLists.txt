################################################################################
# Package: TrkNeutralParameters
################################################################################

# Declare the package name:
atlas_subdir( TrkNeutralParameters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParametersBase )

# Component(s) in the package:
atlas_add_library( TrkNeutralParameters
                   PUBLIC_HEADERS TrkNeutralParameters
                   LINK_LIBRARIES TrkSurfaces TrkParametersBase )

