################################################################################
# Package: CscSegmentPerformance
################################################################################

# Declare the package name:
atlas_subdir( CscSegmentPerformance )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Hist Tree Core MathCore RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_executable( csc_layer_efficiency
                      src/csc_layer_efficiency.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} )

atlas_add_executable( csc_segment_performance
                      src/csc_segment_performance.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} )

atlas_add_executable( csc_4d_segment_performance
                      src/csc_4d_segment_performance.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} )

# Install files from the package:
atlas_install_headers( CscSegmentPerformance )

