################################################################################
# Package: MuonSegmentTaggerTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentTaggerTools )

# Component(s) in the package:
atlas_add_library( MuonSegmentTaggerToolsLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES GaudiKernel StoreGateLib SGtests MuonIdHelpersLib MuonRecHelperToolsLib
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps Identifier EventPrimitives MuonReadoutGeometry MuonRIO_OnTrack MuonSegment MuonSegmentMakerUtils MuonCombinedEvent TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters TrkSegment TrkTrack TrkExInterfaces TrkToolInterfaces MuonSegmentTaggerToolInterfaces )

atlas_add_component( MuonSegmentTaggerTools
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps StoreGateLib SGtests Identifier EventPrimitives MuonReadoutGeometry MuonIdHelpersLib MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonSegmentMakerUtils MuonCombinedEvent TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters TrkSegment TrkTrack TrkExInterfaces TrkToolInterfaces MuonSegmentTaggerToolsLib MuonSegmentTaggerToolInterfaces )

# Install files from the package:
atlas_install_joboptions( share/*.py )

