# $Id: CMakeLists.txt 782136 2016-11-03 14:26:48Z krasznaa $
################################################################################
# Package: JetTileCorrection
################################################################################

# Declare the package name:
atlas_subdir( JetTileCorrection )

# Extra dependencies, based on the environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthAnalysisBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Reconstruction/Jet/JetCPInterfaces
   Event/xAOD/xAODJet
   PRIVATE
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   Tools/PathResolver
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODCore
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )
find_package( Boost )

# Libraries in the package:
atlas_add_root_dictionary( JetTileCorrectionLib JetTileCorrectionDictSource
   ROOT_HEADERS JetTileCorrection/JetTileCorrectionTool.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( JetTileCorrectionLib
   JetTileCorrection/*.h Root/*.cxx ${JetTileCorrectionDictSource}
   PUBLIC_HEADERS JetTileCorrection
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AsgTools JetCPInterfaces
   xAODJet
   PRIVATE_LINK_LIBRARIES PATInterfaces PathResolver xAODEventInfo )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetTileCorrection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthAnalysisBaseCompsLib AsgTools JetCPInterfaces xAODJet
      PathResolver GaudiKernel JetTileCorrectionLib )
endif()

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( JetTileCorrectionTester
      util/JetTileCorrectionTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODJet
      xAODCore AsgTools PATInterfaces JetTileCorrectionLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   # FIX ME: temporarily disabled as part of the migration of AnalysisBase
   #         to master

   # atlas_add_test( ut_JetTileCorrectionTool_data
   #    SOURCES test/ut_JetTileCorrectionTool_data.cxx )
   # atlas_add_test( ut_JetTileCorrectionTool_mc
   #    SOURCES test/ut_JetTileCorrectionTool_mc.cxx )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( data/* )
