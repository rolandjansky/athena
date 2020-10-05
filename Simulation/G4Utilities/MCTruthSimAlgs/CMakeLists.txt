################################################################################
# Package: MCTruthSimAlgs
################################################################################

# Declare the package name:
atlas_subdir( MCTruthSimAlgs )

# Component(s) in the package:
atlas_add_component( MCTruthSimAlgs
                     src/*.cxx
                     src/components/MCTruthSimAlgs_entries.cxx
                     LINK_LIBRARIES CaloSimEvent AthenaBaseComps AthenaKernel PileUpToolsLib StoreGateLib SGtests xAODJet GaudiKernel GeneratorObjects TruthUtils MuonSimEvent RecEvent EventInfo TrackRecordLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

