################################################################################
# Package: TrigKernel
################################################################################

# Declare the package name:
atlas_subdir( TrigKernel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( tdaq-common COMPONENTS eformat hltinterface )

# Component(s) in the package:
atlas_add_library( TrigKernel
                   src/*.cxx
                   PUBLIC_HEADERS TrigKernel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} GaudiKernel )

