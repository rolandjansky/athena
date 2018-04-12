# $Id: CMakeLists.txt 779562 2016-10-21 09:48:54Z krasznaa $
################################################################################
# Package: JetSubStructureMomentTools
################################################################################

# Declare the package name:
atlas_subdir( JetSubStructureMomentTools )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODJet
  Reconstruction/Jet/JetInterface
  Reconstruction/Jet/JetRec
  Reconstruction/Jet/JetSubStructureUtils
  PRIVATE
  Event/xAOD/xAODMuon
  Reconstruction/Jet/JetEDM
  Reconstruction/Jet/JetCalibTools
  ${extra_deps} )

# External dependencies:
find_package( FastJet )
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
set( extra_srcs )
if( NOT XAOD_STANDALONE )
   set( extra_srcs src/*.cxx )
endif()
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs GaudiKernel )
endif()
atlas_add_library( JetSubStructureMomentToolsLib
   JetSubStructureMomentTools/*.h Root/*.cxx ${extra_srcs}
   PUBLIC_HEADERS JetSubStructureMomentTools
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools xAODCaloEvent xAODJet JetInterface JetRecLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} xAODMuon
   JetEDM JetSubStructureUtils JetCalibToolsLib ${extra_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetSubStructureMomentTools
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel JetSubStructureMomentToolsLib )
endif()

atlas_add_dictionary( JetSubStructureMomentToolsDict
	JetSubStructureMomentTools/JetSubStructureMomentToolsDict.h
   JetSubStructureMomentTools/selection.xml
	LINK_LIBRARIES JetSubStructureMomentToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
