################################################################################
# Package: TrkTruthToTrack
################################################################################

# Declare the package name:
atlas_subdir( TrkTruthToTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Event/xAOD/xAODTruth
                          Simulation/G4Sim/TrackRecord
                          Tracking/TrkExtrapolation/TrkExInterfaces 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TrkTruthToTrack
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel TrackRecordLib TrkParameters TrkToolInterfaces xAODTruth TrkExInterfaces )

# Install files from the package:
atlas_install_headers( TrkTruthToTrack )

