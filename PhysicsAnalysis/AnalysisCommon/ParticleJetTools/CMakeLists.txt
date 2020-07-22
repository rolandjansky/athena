################################################################################
# Package: ParticleJetTools
################################################################################

# Declare the package name:
atlas_subdir( ParticleJetTools )


# Declare the package's dependencies:
if( XAOD_STANDALONE )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgDataHandles
      Control/AthToolSupport/AsgTools
      Event/EventPrimitives
      Event/xAOD/xAODJet
      Event/xAOD/xAODTruth
      PhysicsAnalysis/MCTruthClassifier
      Reconstruction/Jet/JetInterface
      Reconstruction/Jet/JetRec
      PRIVATE
      Control/AthContainers
      Generators/TruthUtils )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgDataHandles
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
endif()

# Component(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_library( ParticleJetToolsLib
      ParticleJetTools/*.h Root/*.cxx src/*.cxx
      PUBLIC_HEADERS ParticleJetTools
      LINK_LIBRARIES AsgTools EventPrimitives xAODJet xAODTruth
      JetInterface MCTruthClassifierLib JetRecLib AsgDataHandlesLib
      PRIVATE_LINK_LIBRARIES AthContainers TruthUtils )
else()
   atlas_add_library( ParticleJetToolsLib
      ParticleJetTools/*.h Root/*.cxx src/*.cxx
      PUBLIC_HEADERS ParticleJetTools
      LINK_LIBRARIES AsgTools EventPrimitives xAODJet xAODTruth GaudiKernel
      JetInterface MCTruthClassifierLib JetRecLib AsgDataHandlesLib
      PRIVATE_LINK_LIBRARIES AthAnalysisBaseCompsLib AthContainers TruthUtils )
endif()

if( NOT XAOD_STANDALONE )
   atlas_add_component( ParticleJetTools
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel ParticleJetToolsLib )
endif()

atlas_add_dictionary( ParticleJetToolsDict
  ParticleJetTools/ParticleJetToolsDict.h
  ParticleJetTools/selection.xml
  LINK_LIBRARIES ParticleJetToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
