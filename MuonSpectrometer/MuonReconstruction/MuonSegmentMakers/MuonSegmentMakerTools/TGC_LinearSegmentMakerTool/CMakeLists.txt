################################################################################
# Package: TGC_LinearSegmentMakerTool
################################################################################

# Declare the package name:
atlas_subdir( TGC_LinearSegmentMakerTool )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TGC_LinearSegmentMakerTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel MuonRecToolInterfaces StoreGateLib SGtests GeoPrimitives MuonReadoutGeometry MuonRIO_OnTrack MuonSegment MuonLinearSegmentMakerUtilities TrkSurfaces TrkEventPrimitives TrkRoad TrkExInterfaces TrkExUtils )

# Install files from the package:
atlas_install_headers( TGC_LinearSegmentMakerTool )

