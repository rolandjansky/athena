################################################################################
# Package: MuPatPrimitives
################################################################################

# Declare the package name:
atlas_subdir( MuPatPrimitives)

# Component(s) in the package:
# Component(s) in the package:
atlas_add_library( MuPatPrimitives
                   src/*.cxx
                   PUBLIC_HEADERS MuPatPrimitives
                   LINK_LIBRARIES CxxUtils EventPrimitives TrkMeasurementBase TrkParameters MuonStationIndexLib 
			          MuonSegment TrkSegment TrkTrack TrkTrackSummary)

# Install files from the package:
#atlas_install_joboptions( share/*.py )

