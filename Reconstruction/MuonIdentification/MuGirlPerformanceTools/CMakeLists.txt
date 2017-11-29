################################################################################
# Package: MuGirlPerformanceTools
################################################################################

# Declare the package name:
atlas_subdir( MuGirlPerformanceTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuGirlInterfaces
                          Reconstruction/iPat/iPatInterfaces
                          Trigger/TrigEvent/TrigMuonEvent
                          PRIVATE
                          Generators/GenInterfaces
                          Generators/GeneratorObjects
                          Reconstruction/MuonIdentification/MuGirlCandidate
                          Reconstruction/iPat/iPatTrackParameters
                          Reconstruction/iPat/iPatTruthTrajectory
                          Simulation/G4Sim/TrackRecord )

# External dependencies:
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( MuGirlPerformanceTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} AthenaBaseComps GaudiKernel MuGirlInterfaces iPatInterfaces TrigMuonEvent GeneratorObjects iPatTrackParameters )

# Install files from the package:
atlas_install_headers( MuGirlPerformanceTools )

