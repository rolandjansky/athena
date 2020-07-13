################################################################################
# Package: TrkTruthTrackAlgs
################################################################################
# Declare the package name:
atlas_subdir( TrkTruthTrackAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkTruthTracks/TrkTruthTrackInterfaces 
                          Generators/AtlasHepMC )

# External dependencies:

# Component(s) in the package:
atlas_add_component( TrkTruthTrackAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps GaudiKernel TrkSegment TrkTrack TrkToolInterfaces TrkTruthTrackInterfaces )

