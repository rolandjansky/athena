# $Id: CMakeLists.txt 808750 2017-07-26 19:46:53Z jbossios $
################################################################################
# Package: JetCalibTools
################################################################################

# Declare the package name:
atlas_subdir( JetCalibTools )

# Extra dependencies, based on the environment:
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
   Event/xAOD/xAODEventShape
   Event/xAOD/xAODJet
   Event/xAOD/xAODTracking
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   Reconstruction/Jet/JetInterface
   PRIVATE
   Event/xAOD/xAODMuon
   Tools/PathResolver
   PhysicsAnalysis/POOLRootAccess
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO Graf Gpad )

atlas_install_generic( share/* DESTINATION data/JetCalibTools )

# Libraries in the package:
atlas_add_library( JetCalibToolsLib
   JetCalibTools/*.h Root/*.cxx
   PUBLIC_HEADERS JetCalibTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODEventShape
   xAODJet xAODTracking PATInterfaces JetInterface
   PRIVATE_LINK_LIBRARIES xAODMuon PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetCalibTools
      src/components/*.cxx
      LINK_LIBRARIES JetCalibToolsLib )
endif()

atlas_add_dictionary( JetCalibToolsDict
   JetCalibTools/JetCalibToolsDict.h
   JetCalibTools/selection.xml
   LINK_LIBRARIES JetCalibToolsLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( JetCalibTools_Example
      util/JetCalibTools_Example.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODEventShape xAODCore xAODRootAccess JetCalibToolsLib )
   
   atlas_add_executable( JetCalibTools_PlotJESFactors
      util/JetCalibTools_PlotJESFactors.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess JetCalibToolsLib )
   
   atlas_add_executable( JetCalibTools_PlotJMSFactors
      util/JetCalibTools_PlotJMSFactors.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess JetCalibToolsLib )

   atlas_add_executable( JetCalibTools_SmearingPlots
      util/JetCalibTools_SmearingPlots.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess JetCalibToolsLib )
   
else()
   atlas_add_executable( JetCalibTools_Example
      util/JetCalibTools_Example.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODEventShape xAODCore POOLRootAccess xAODRootAccess JetCalibToolsLib )

   atlas_add_executable( JetCalibTools_PlotJESFactors
      util/JetCalibTools_PlotJESFactors.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess POOLRootAccess JetCalibToolsLib )

   atlas_add_executable( JetCalibTools_PlotJMSFactors
      util/JetCalibTools_PlotJMSFactors.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess POOLRootAccess JetCalibToolsLib )

   atlas_add_executable( JetCalibTools_SmearingPlots
      util/JetCalibTools_SmearingPlots.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess POOLRootAccess JetCalibToolsLib )

endif()

atlas_install_python_modules( python/*.py )
