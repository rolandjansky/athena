################################################################################
# Package: MuonSegmentMatchingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMatchingTools )

# Component(s) in the package:
atlas_add_component( MuonSegmentMatchingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GeoPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonRIO_OnTrack MuonCompetingRIOsOnTrack MuonSegment MuonRecHelperToolsLib TrkDetDescrUtils TrkGeometry TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces MuonSegmentMakerToolInterfaces )

