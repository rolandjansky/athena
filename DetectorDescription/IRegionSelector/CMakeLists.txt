################################################################################
# Package: IRegionSelector
################################################################################

# Declare the package name:
atlas_subdir( IRegionSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( IRegionSelector
                   PUBLIC_HEADERS IRegionSelector
                   LINK_LIBRARIES AthenaKernel Identifier GaudiKernel )

