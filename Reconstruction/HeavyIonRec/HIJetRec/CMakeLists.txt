################################################################################
# Package: HIJetRec
################################################################################

# Declare the package name:
atlas_subdir( HIJetRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Calorimeter/CaloRec
                          Calorimeter/CaloUtils
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Control/Navigation
                          Event/FourMomUtils
                          Event/NavFourMom
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODJet
                          GaudiKernel
                          PhysicsAnalysis/HeavyIonPhys/HIEventUtils
                          Reconstruction/Jet/JetInterface
                          Reconstruction/Jet/JetRec
                          PRIVATE
                          Control/CxxUtils
                          Event/xAOD/xAODCore
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( HIJetRec
                     src/*.cxx
                     src/components/*.cxx
                     Root/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloRecLib CaloUtilsLib AsgTools AthenaBaseComps Navigation FourMomUtils NavFourMom xAODBase xAODCaloEvent xAODHIEvent xAODJet GaudiKernel HIEventUtilsLib JetInterface JetRecLib CxxUtils xAODCore PathResolver )

# Install files from the package:
atlas_install_headers( HIJetRec )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( data/cluster.geo.root )

