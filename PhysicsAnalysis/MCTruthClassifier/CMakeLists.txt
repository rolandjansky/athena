################################################################################
# Package: MCTruthClassifier
################################################################################

# Declare the package name:
atlas_subdir( MCTruthClassifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          Generators/GeneratorObjects
                          Reconstruction/RecoTools/ParticlesInConeTools
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers )

# External dependencies:
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MCTruthClassifierLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS MCTruthClassifier
                   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${HEPMC_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTracking xAODTruth GaudiKernel GeneratorObjects RecoToolInterfaces TrkEventPrimitives TrkParametersIdentificationHelpers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_component( MCTruthClassifier
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTracking xAODTruth GaudiKernel GeneratorObjects RecoToolInterfaces TrkEventPrimitives TrkParametersIdentificationHelpers MCTruthClassifierLib )

atlas_add_dictionary( MCTruthClassifierDict
                      MCTruthClassifier/MCTruthClassifierDict.h
                      MCTruthClassifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTracking xAODTruth GaudiKernel GeneratorObjects RecoToolInterfaces TrkEventPrimitives TrkParametersIdentificationHelpers MCTruthClassifierLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

