 ################################################################################
 # Package: METReconstruction
 ################################################################################
 
 # Declare the package name:
 atlas_subdir( METReconstruction )
 
 # Declare the package's dependencies:
 atlas_depends_on_subdirs( PUBLIC
                           Calorimeter/CaloInterface
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
                           InnerDetector/InDetRecTools/InDetTrackSelectionTool
                           InnerDetector/InDetRecTools/TrackVertexAssociationTool
                           Reconstruction/MET/METRecoInterface
                           Reconstruction/PFlow/PFlowUtils
                           PRIVATE
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
 atlas_add_library( METReconstructionLib
                    src/*.cxx
                    Root/*.cxx
                    PUBLIC_HEADERS METReconstruction
                    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                    LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODPFlow xAODTracking xAODTruth GaudiKernel TrackVertexAssociationToolLib
                    PRIVATE_LINK_LIBRARIES CaloEvent AthContainers AthenaBaseComps EventPrimitives xAODTau TruthUtils )
 
 atlas_add_component( METReconstruction
                      src/components/*.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODPFlow xAODTracking xAODTruth GaudiKernel TrackVertexAssociationToolLib CaloEvent AthContainers AthenaBaseComps EventPrimitives xAODTau TruthUtils METReconstructionLib )
 
 atlas_add_dictionary( METReconstructionDict
                       METReconstruction/METReconstructionDict.h
                       METReconstruction/selection.xml
                       INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                       LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODPFlow xAODTracking xAODTruth GaudiKernel TrackVertexAssociationToolLib CaloEvent AthContainers AthenaBaseComps EventPrimitives xAODTau TruthUtils METReconstructionLib )
 
 # Install files from the package:
 atlas_install_python_modules( python/*.py )
 atlas_install_joboptions( share/*.py )

