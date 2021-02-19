################################################################################
# Package: MuonTGMeasAssocAlg
################################################################################

# Declare the package name:
atlas_subdir( MuonTGMeasAssocAlg )

# Component(s) in the package:
atlas_add_component( MuonTGMeasAssocAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GeoPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData TrkGeometry TrkSurfaces TrkPrepRawData TrkSegment TrkTrack TrkExInterfaces EventPrimitives MuonSegment TrkEventPrimitives TrkMeasurementBase TrkParameters MuonTGRecToolsLib )

# Install files from the package:
atlas_install_headers( MuonTGMeasAssocAlg )
atlas_install_joboptions( share/*.py )

