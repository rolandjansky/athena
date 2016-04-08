################################################################################
# Package: PerfMonKernel
################################################################################

# Declare the package name:
atlas_subdir( PerfMonKernel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( PerfMonKernel
                   src/*.cxx
                   PUBLIC_HEADERS PerfMonKernel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel )

