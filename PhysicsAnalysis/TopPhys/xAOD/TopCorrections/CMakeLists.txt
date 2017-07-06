# Auto-generated on: 2017-03-08 14:47:35.652165

# Declare the name of this package:
atlas_subdir( TopCorrections None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODEgamma
                          xAODMuon
                          TopEvent
                          PileupReweighting
                          ElectronEfficiencyCorrection
                          MuonEfficiencyCorrections
                          xAODBTaggingEfficiency
                          TauAnalysisTools
                          JetJvtEfficiency
                          PhotonEfficiencyCorrection
                          PMGTools )

# This package uses LHAPDF:
find_package( Lhapdf )
# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Custom definitions needed for this package:
add_definitions( -g -std=c++14 )

# Build a library that other components can link against:
atlas_add_library( TopCorrections Root/*.cxx Root/*.h Root/*.icc
                   TopCorrections/*.h TopCorrections/*.icc TopCorrections/*/*.h
                   TopCorrections/*/*.icc 
                   PUBLIC_HEADERS TopCorrections
                   LINK_LIBRARIES xAODEgamma
                                  xAODMuon
                                  TopEvent
                                  PileupReweightingLib
                                  ElectronEfficiencyCorrectionLib
                                  MuonEfficiencyCorrectionsLib
                                  xAODBTaggingEfficiencyLib
                                  TauAnalysisToolsLib
                                  JetJvtEfficiencyLib
                                  PhotonEfficiencyCorrectionLib
                                  PMGToolsLib
				  ${LHAPDF_LIBRARIES}
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
		                ${LHAPDF_INCLUDE_DIRS})

# Install data files from the package:
atlas_install_data( share/* )

