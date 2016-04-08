################################################################################
# Package: ImpactParameterTuningUtils
################################################################################

# Declare the package name:
atlas_subdir( ImpactParameterTuningUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( ImpactParameterTuningUtils
                   src/*.cxx
                   PUBLIC_HEADERS ImpactParameterTuningUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

