# $Id: CMakeLists.txt 780663 2016-10-27 09:43:48Z krasznaa $
################################################################################
# Package: JetSelectorTools
################################################################################

# Declare the package name:
atlas_subdir( JetSelectorTools )

# Extra dependencies based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODJet
   PhysicsAnalysis/AnalysisCommon/PATCore
   Reconstruction/Jet/JetInterface
   PRIVATE
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO )

# Component(s) in the package:
atlas_add_library( JetSelectorToolsLib
   JetSelectorTools/*.h Root/*.cxx
   PUBLIC_HEADERS JetSelectorTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODJet JetInterface PATCoreLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetSelectorTools
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel JetSelectorToolsLib )
endif()

atlas_add_dictionary( JetSelectorToolsDict
   JetSelectorTools/JetSelectorToolsDict.h
   JetSelectorTools/selection.xml
   LINK_LIBRARIES JetSelectorToolsLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( TestJetCleaningTool
      util/TestJetCleaningTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODJet
      JetSelectorToolsLib )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_data( share/*.config )
