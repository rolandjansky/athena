################################################################################
# Package: ParticleJetTools
################################################################################

# Declare the package name:
atlas_subdir( ParticleJetTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/EventPrimitives
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/MCTruthClassifier
                          Reconstruction/Jet/JetInterface
                          Reconstruction/Jet/JetRec
                          PRIVATE
                          Control/AthAnalysisBaseComps
                          Control/AthContainers
                          Generators/TruthUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ParticleJetToolsLib
                   src/HadronUtils.cxx
                   Root/JetQuarkLabel.cxx
                   Root/JetConeLabeling.cxx
                   Root/ParticleToJetAssociator.cxx
                   Root/JetPartonTruthLabel.cxx
                   Root/CopyTruthParticles.cxx
                   Root/CopyFlavorLabelTruthParticles.cxx
                   Root/CopyBosonTopLabelTruthParticles.cxx
                   Root/CopyTruthPartons.cxx
                   Root/CopyTruthJetParticles.cxx
                   Root/JetFlavourInfo.cxx
                   Root/ParticleJetDeltaRLabelTool.cxx
                   PUBLIC_HEADERS ParticleJetTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools EventPrimitives xAODJet xAODTruth GaudiKernel JetInterface MCTruthClassifierLib JetRecLib
                   PRIVATE_LINK_LIBRARIES AthAnalysisBaseComps AthContainers TruthUtils )

atlas_add_component( ParticleJetTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools EventPrimitives xAODJet xAODTruth GaudiKernel MCTruthClassifierLib JetInterface JetRecLib AthAnalysisBaseComps AthContainers TruthUtils ParticleJetToolsLib )

atlas_add_dictionary( ParticleJetToolsDict
                      ParticleJetTools/ParticleJetToolsDict.h
                      ParticleJetTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools EventPrimitives xAODJet xAODTruth GaudiKernel MCTruthClassifierLib JetInterface JetRecLib AthAnalysisBaseComps AthContainers TruthUtils ParticleJetToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

