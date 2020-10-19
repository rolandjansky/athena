# $Id: CMakeLists.txt 786868 2016-11-28 12:40:09Z krasznaa $
################################################################################
# Package: JetUncertainties
################################################################################

# Declare the package name:
atlas_subdir( JetUncertainties )

# Extra dependencies, based on the build environment:
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
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/AnalysisCommon/PATCore 
   Reconstruction/Jet/JetCPInterfaces
   Reconstruction/Jet/JetInterface
   PRIVATE
   Reconstruction/Jet/BoostedJetTaggers
   Reconstruction/Jet/JetCalibTools
   Event/xAOD/xAODCore
   Event/xAOD/xAODTracking
   Tools/PathResolver
   PhysicsAnalysis/AnalysisCommon/ParticleJetTools
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO Graf Gpad )

# Libraries in the package:
atlas_add_library( JetUncertaintiesLib
   JetUncertainties/*.h Root/*.cxx
   PUBLIC_HEADERS JetUncertainties
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet PATInterfaces
   JetCPInterfaces JetInterface PATCoreLib 
   PRIVATE_LINK_LIBRARIES xAODCore xAODTracking PathResolver)

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetUncertainties
      src/components/*.cxx
      LINK_LIBRARIES JetUncertaintiesLib )
endif()

atlas_add_dictionary( JetUncertaintiesDict
   JetUncertainties/JetUncertaintiesDict.h
   JetUncertainties/selection.xml
   LINK_LIBRARIES JetUncertaintiesLib )

# Executable(s) in the package:
atlas_add_executable( MakeCorrelationMatrixPlots
   util/MakeCorrelationMatrixPlots.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} JetUncertaintiesLib )

if( XAOD_STANDALONE )
   atlas_add_executable( MakeUncertaintyPlots
      util/MakeUncertaintyPlots.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODEventInfo
      xAODRootAccess JetUncertaintiesLib )

   atlas_add_executable( JetUncertainties_testTool
      util/testTool.cxx
      LINK_LIBRARIES xAODJet xAODEventInfo PATInterfaces xAODRootAccess
      JetUncertaintiesLib )
   
   atlas_add_executable( JetUncertainties_testResolution
      util/testResolution.cxx
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODEventInfo
      xAODRootAccess JetUncertaintiesLib )

   atlas_add_executable( FFJetSmearingTool_MyExample
      util/FFJetSmearingTool_MyExample.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODEventShape xAODCore xAODRootAccess BoostedJetTaggersLib JetUncertaintiesLib JetCalibToolsLib ParticleJetToolsLib )


endif()

# Install files from the package:
atlas_install_generic( share/*
   DESTINATION share PKGNAME_SUBDIR )
