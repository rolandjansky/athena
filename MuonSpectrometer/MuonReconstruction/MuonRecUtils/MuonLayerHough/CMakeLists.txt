################################################################################
# Package: MuonLayerHough
################################################################################

# Declare the package name:
atlas_subdir( MuonLayerHough )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Gpad Graf Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf3d Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( MuonLayerHough
                   src/*.cxx
                   PUBLIC_HEADERS MuonLayerHough
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES CxxUtils MuonStationIndexLib GaudiKernel AthenaKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives )

atlas_add_executable( MainLayerAnalysis
                      exe/MainLayerAnalysis.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CxxUtils GaudiKernel AthenaKernel MuonStationIndexLib GeoPrimitives MuonLayerHough )

