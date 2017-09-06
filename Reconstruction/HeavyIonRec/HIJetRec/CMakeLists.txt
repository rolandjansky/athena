# $Id: CMakeLists.txt 740786 2016-04-16 13:57:11Z krasznaa $
################################################################################
# Package: HIJetRec
################################################################################

# Declare the package name:
atlas_subdir( HIJetRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/FourMomUtils
  Event/xAOD/xAODBase
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODHIEvent
  Event/xAOD/xAODJet
  PhysicsAnalysis/HeavyIonPhys/HIEventUtils
  Reconstruction/Jet/JetInterface
  Reconstruction/Jet/JetRec
  PRIVATE
  Control/CxxUtils
  Event/xAOD/xAODCore
  Tools/PathResolver
  Control/Navigation
  Control/AthenaBaseComps
  Event/NavFourMom
  Calorimeter/CaloEvent
  Calorimeter/CaloInterface
  Calorimeter/CaloRec
  Calorimeter/CaloUtils
  GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist Physics )

# Component(s) in the package:
atlas_add_component( HIJetRec
  HIJetRec/*.h Root/*.cxx src/*.h src/*.cxx src/components/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloRecLib CaloUtilsLib
  AsgTools AthenaBaseComps Navigation FourMomUtils NavFourMom xAODBase
  xAODCaloEvent xAODHIEvent xAODJet GaudiKernel HIEventUtilsLib
  JetInterface JetRecLib CxxUtils xAODCore PathResolver )

# Install files from the package:
atlas_install_headers( HIJetRec )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
