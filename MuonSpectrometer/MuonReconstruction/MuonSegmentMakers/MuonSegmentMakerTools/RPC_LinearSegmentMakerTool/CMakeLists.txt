################################################################################
# Package: RPC_LinearSegmentMakerTool
################################################################################

# Declare the package name:
atlas_subdir( RPC_LinearSegmentMakerTool )

# Component(s) in the package:
atlas_add_component( RPC_LinearSegmentMakerTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonRecToolInterfaces StoreGateLib SGtests GeoPrimitives MuonReadoutGeometry MuonRIO_OnTrack MuonSegment MuonLinearSegmentMakerUtilities TrkSurfaces TrkEventPrimitives TrkRoad )

# Install files from the package:
atlas_install_headers( RPC_LinearSegmentMakerTool )

