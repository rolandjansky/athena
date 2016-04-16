# $Id: CMakeLists.txt 740784 2016-04-16 13:40:59Z krasznaa $
################################################################################
# Package: JetSubStructureMomentTools
################################################################################

# Declare the package name:
atlas_subdir( JetSubStructureMomentTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODJet
  Reconstruction/Jet/JetInterface
  Reconstruction/Jet/JetRec
  PRIVATE
  Event/xAOD/xAODMuon
  GaudiKernel
  Reconstruction/Jet/JetEDM
  Reconstruction/Jet/JetSubStructureUtils )

# External dependencies:
find_package( FastJet )
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_library( JetSubStructureMomentToolsLib
  JetSubStructureMomentTools/*.h Root/*.cxx src/*.cxx
  PUBLIC_HEADERS JetSubStructureMomentTools
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
  LINK_LIBRARIES AsgTools xAODCaloEvent xAODJet JetInterface JetRecLib
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} xAODMuon
  JetEDM JetSubStructureUtils GaudiKernel )

atlas_add_component( JetSubStructureMomentTools
  src/components/*.cxx
  LINK_LIBRARIES JetSubStructureMomentToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
