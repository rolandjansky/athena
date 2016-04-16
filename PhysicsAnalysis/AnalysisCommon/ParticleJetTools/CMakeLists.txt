# $Id: CMakeLists.txt 740788 2016-04-16 14:22:45Z krasznaa $
################################################################################
# Package: ParticleJetTools
################################################################################

# Declare the package name:
atlas_subdir( ParticleJetTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
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

# Component(s) in the package:
atlas_add_library( ParticleJetToolsLib
  ParticleJetTools/*.h Root/*.cxx src/*.cxx
  PUBLIC_HEADERS ParticleJetTools
  LINK_LIBRARIES AsgTools EventPrimitives xAODJet xAODTruth GaudiKernel
  JetInterface MCTruthClassifierLib JetRecLib
  PRIVATE_LINK_LIBRARIES AthAnalysisBaseComps AthContainers TruthUtils )

atlas_add_component( ParticleJetTools
  src/components/*.cxx
  LINK_LIBRARIES ParticleJetToolsLib )

atlas_add_dictionary( ParticleJetToolsDict
  ParticleJetTools/ParticleJetToolsDict.h
  ParticleJetTools/selection.xml
  LINK_LIBRARIES ParticleJetToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
