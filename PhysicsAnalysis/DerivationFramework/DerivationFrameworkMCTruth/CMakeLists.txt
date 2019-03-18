################################################################################
# Package: DerivationFrameworkMCTruth
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkMCTruth )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODJet
                          Generators/GeneratorObjects
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
                          PhysicsAnalysis/MCTruthClassifier )

# External dependencies:
find_package( FastJet )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkMCTruthLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkMCTruth
                   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${FASTJET_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps xAODEventInfo xAODTruth GaudiKernel SGtests ExpressionEvaluationLib MCTruthClassifierLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel xAODBase xAODJet GeneratorObjects )

atlas_add_component( DerivationFrameworkMCTruth
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${FASTJET_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps xAODEventInfo xAODTruth GaudiKernel AthenaKernel SGtests EventInfo xAODBase xAODJet GeneratorObjects ExpressionEvaluationLib MCTruthClassifierLib DerivationFrameworkMCTruthLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

