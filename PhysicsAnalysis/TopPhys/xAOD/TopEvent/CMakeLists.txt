# Auto-generated on: 2017-03-08 14:47:36.020351

# Declare the name of this package:
atlas_subdir( TopEvent None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          CxxUtils
                          xAODCore
                          AthContainers
                          AthLinks
                          AsgTools
                          xAODEventInfo
                          xAODTruth
                          xAODEgamma
                          xAODMuon
                          xAODJet
                          xAODTau
                          xAODMissingET
                          xAODTracking
                          FourMomUtils
                          TopConfiguration
                          TopPartons
                          JetAnalysisInterfaces
                          JetSubStructureMomentTools
                          JetSubStructureUtils
                          JetReclustering )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Need fast jet for the RC jet substructure code
find_package( FastJet COMPONENTS fastjetplugins fastjettools )
find_package( FastJetContrib COMPONENTS EnergyCorrelator Nsubjettiness )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopEvent _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopEvent Root/*.cxx Root/*.h Root/*.icc
                   TopEvent/*.h TopEvent/*.icc TopEvent/*/*.h
                   TopEvent/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopEvent
                   LINK_LIBRARIES CxxUtils
                                  xAODCore
                                  AthContainers
                                  AthLinks
                                  AsgTools
                                  xAODEventInfo
                                  xAODTruth
                                  xAODEgamma
                                  xAODMuon
                                  xAODJet
                                  xAODTau
                                  xAODMissingET
                                  xAODTracking
                                  FourMomUtils
                                  TopConfiguration
                                  TopPartons
                                  JetAnalysisInterfacesLib
                                  JetSubStructureMomentToolsLib
                                  JetSubStructureUtils
                                  JetReclusteringLib
                                  ${FASTJET_LIBRARIES}
                                  ${FASTJETCONTRIB_LIBRARIES}
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

