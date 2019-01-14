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

#set(extra_depsp )
#if( NOT GENERATIONBASE )
#  set( extra_deps Event/xAOD/xAODMuon )
#  set( extra_libs xAODMuon )
#endif()

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
#  ${extra_depsp}
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
#if ( GENERATIONBASE )
#   atlas_install_python_modules( python/DefaultTools.py python/FastJetInterfaceConfig.py 
#python/JetAlgorithm.py 
#   python/JetFlavorAlgs.py python/JetRecCalibrationFinder.py python/JetRecFlags.py 
#python/JetRecStandard.py 
#   python/JetRecStandardToolManager.py python/JetRecStandardTools.py python/JetRecUtils.py 
#python/JetToolSupport.py 
#   python/BasicTruthJetsFinding.py  )
#else() 
   atlas_install_python_modules( python/*.py )
#endif() 
atlas_install_joboptions( share/JetRec_jobOptions.py share/myjets.py )

