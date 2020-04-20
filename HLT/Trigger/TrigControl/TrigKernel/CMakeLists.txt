################################################################################
# Package: TrigKernel
################################################################################

# Declare the package name:
atlas_subdir( TrigKernel )

# Header-only boost dependency
find_package( Boost )

# Header-only library
atlas_add_library( TrigKernel
                   INTERFACE
                   PUBLIC_HEADERS TrigKernel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel CxxUtils )
