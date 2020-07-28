################################################################################
# Package: TilePulseSimulator
################################################################################

# Declare the package name:
atlas_subdir( TilePulseSimulator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core Tree MathCore RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( TilePulseSimulator
                   src/*.cxx
                   PUBLIC_HEADERS TilePulseSimulator
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES GaudiKernel )
