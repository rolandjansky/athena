################################################################################
# Package: MuonOverlayBase
################################################################################

# Declare the package name:
atlas_subdir( MuonOverlayBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventOverlay/IDC_OverlayBase
                          Event/EventOverlay/OverlayAlgBase
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( MuonOverlayBase
                   PUBLIC_HEADERS MuonOverlayBase
                   LINK_LIBRARIES AthenaBaseComps DataModel Identifier IDC_OverlayBase OverlayAlgBase GaudiKernel StoreGateLib SGtests )

