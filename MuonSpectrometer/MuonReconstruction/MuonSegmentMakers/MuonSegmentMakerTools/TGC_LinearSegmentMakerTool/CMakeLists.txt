################################################################################
# Package: TGC_LinearSegmentMakerTool
################################################################################

# Declare the package name:
atlas_subdir( TGC_LinearSegmentMakerTool )

# Component(s) in the package:
atlas_add_component( TGC_LinearSegmentMakerTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonRecToolInterfaces StoreGateLib SGtests GeoPrimitives MuonReadoutGeometry MuonRIO_OnTrack MuonSegment MuonLinearSegmentMakerUtilities TrkSurfaces TrkEventPrimitives TrkRoad TrkExInterfaces TrkExUtils )

# Install files from the package:
atlas_install_headers( TGC_LinearSegmentMakerTool )

