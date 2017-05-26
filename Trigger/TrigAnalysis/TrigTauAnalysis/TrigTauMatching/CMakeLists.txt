# $Id: CMakeLists.txt 781031 2016-10-28 15:10:31Z krasznaa $

# The name of the package:
atlas_subdir( TrigTauMatching )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODTau
   Event/xAOD/xAODTrigger
   Trigger/TrigAnalysis/TrigDecisionTool
   PRIVATE
   Event/xAOD/xAODCore
   Trigger/TrigConfiguration/TrigConfxAOD
   ${extra_deps} )

# External(s) the package uses:
find_package( ROOT COMPONENTS Core Hist Tree RIO )

# Libraries in the package:
atlas_add_library( TrigTauMatchingLib
   TrigTauMatching/*.h Root/*.cxx
   PUBLIC_HEADERS TrigTauMatching
   LINK_LIBRARIES AsgTools xAODTau xAODTrigger TrigDecisionToolLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrigTauMatching
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps TrigDecisionToolLib GaudiKernel
      xAODTau TrigTauMatchingLib )
endif()

atlas_add_dictionary( TrigTauMatchingDict
   TrigTauMatching/TrigTauMatchingDict.h
   TrigTauMatching/selection.xml
   LINK_LIBRARIES TrigTauMatchingLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( TrigTauMatching_example
      util/TrigTauMatching_example.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODCore xAODTau AsgTools
      TrigConfxAODLib TrigDecisionToolLib TrigTauMatchingLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
