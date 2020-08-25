################################################################################
# Package: METReconstruction
################################################################################

# Declare the package name:
atlas_subdir( METReconstruction )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          Reconstruction/MET/METRecoInterface
                          PRIVATE
                          Calorimeter/CaloInterface
                          Calorimeter/CaloConditions
                          Event/FourMomUtils
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
                          Reconstruction/PFlow/PFlowUtils
                          Reconstruction/RecoTools/CaloClusterMatching
                          Reconstruction/RecoTools/RecoToolInterfaces
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          Calorimeter/CaloEvent
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Event/EventPrimitives
                          Event/xAOD/xAODTau
                          Generators/TruthUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

include_directories(src)

# Component(s) in the package:

atlas_add_component( METReconstruction
                     Root/*.cxx src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools FourMomUtils xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODPFlow xAODTracking xAODTruth GaudiKernel RecoToolInterfaces CaloEvent AthContainers AthenaBaseComps EventPrimitives xAODTau TruthUtils PATCoreLib METRecoInterface PFlowUtilsLib InDetTrackSelectionToolLib TrackVertexAssociationToolLib )

#atlas_add_dictionary( METReconstructionDict
#	              METReconstruction/selection-athena.xml
#	              INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
#	              LINK_LIBRARIES ${ROOT_LIBRARIES} )

# Install files from the package:
atlas_install_python_modules( python/*.py  python/LegacyRunII/*.py)
atlas_install_joboptions( share/*.py )
