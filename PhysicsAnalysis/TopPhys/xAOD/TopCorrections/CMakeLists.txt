# Declare the name of this package:
atlas_subdir( TopCorrections )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODEgamma
                          xAODMuon
                          TopEvent
                          PileupReweighting
                          ElectronEfficiencyCorrection
                          TauAnalysisTools
                          JetJvtEfficiency
                          PhotonEfficiencyCorrection
                          PMGTools
			  PATInterfaces 
			  FTagAnalysisInterfaces 
			  MuonAnalysisInterfaces 
			  TriggerAnalysisInterfaces
			  PMGAnalysisInterfaces
			  EgammaAnalysisInterfaces)

# This package uses LHAPDF:
find_package( Lhapdf )
# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Add a ROOT dictionary
atlas_add_root_dictionary( TopCorrections _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopCorrections Root/*.cxx Root/*.h Root/*.icc
                   TopCorrections/*.h TopCorrections/*.icc TopCorrections/*/*.h
                   TopCorrections/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopCorrections
                   LINK_LIBRARIES xAODEgamma
                                  xAODMuon
                                  TopEvent
                                  PileupReweightingLib
                                  ElectronEfficiencyCorrectionLib
                                  TauAnalysisToolsLib
                                  JetJvtEfficiencyLib
                                  PhotonEfficiencyCorrectionLib
                                  PMGToolsLib
				  PATInterfaces
				  FTagAnalysisInterfacesLib
				  MuonAnalysisInterfacesLib
				  #TriggerAnalysisInterfacesLib
				  PMGAnalysisInterfacesLib
				  EgammaAnalysisInterfacesLib
				  ${LHAPDF_LIBRARIES}
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
		                ${LHAPDF_INCLUDE_DIRS})

# Install data files from the package:
atlas_install_data( share/* )

