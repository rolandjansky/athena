# $Id: CMakeLists.txt 778359 2016-10-13 19:24:10Z krasznaa $
################################################################################
# Package: JetRec
################################################################################

# Declare the package name:
atlas_subdir( JetRec )

# Extra dependencies, based on the environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthLinks
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODJet
  Event/xAOD/xAODMuon
  Reconstruction/EventShapes/EventShapeInterface
  Reconstruction/Jet/JetEDM
  Reconstruction/Jet/JetInterface
  PRIVATE
  Event/xAOD/xAODBase
  Event/xAOD/xAODCore
  Event/xAOD/xAODEventInfo
  Event/xAOD/xAODTracking
  ${extra_deps} )

# External dependencies:
find_package( FastJet COMPONENTS fastjetplugins fastjettools siscone
  siscone_spherical )
find_package( FastJetContrib COMPONENTS VariableR RecursiveTools )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( JetRecLib
  JetRec/*.h Root/*.cxx
  PUBLIC_HEADERS JetRec
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} AthLinks AsgTools
  xAODCaloEvent xAODJet xAODMuon EventShapeInterface JetEDM
  JetInterface
  PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} xAODBase xAODCore
  xAODEventInfo xAODTracking )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetRec
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps JetInterface JetRecLib )
endif()

atlas_add_dictionary( JetRecDict
   JetRec/JetRecDict.h
   JetRec/selection.xml
   LINK_LIBRARIES JetRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
