# $Id: CMakeLists.txt 783761 2016-11-11 16:50:45Z nkoehler $
################################################################################
# Package: MuonEfficiencyCorrections
################################################################################


# Declare the package name:
atlas_subdir( MuonEfficiencyCorrections )

# Extra dependencies, based on the environment:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess )
else()
   set( extra_dep Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODMuon
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PhysicsAnalysis/Interfaces/MuonAnalysisInterfaces
   PRIVATE
   Control/AthContainers
   Event/xAOD/xAODTrigger
   Tools/PathResolver
   ${extra_dep} )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree RIO Hist Physics )


# Libraries in the package:
atlas_add_library( MuonEfficiencyCorrectionsLib
   MuonEfficiencyCorrections/*.h Root/*.cxx
   PUBLIC_HEADERS MuonEfficiencyCorrections
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODMuon xAODEventInfo MuonAnalysisInterfacesLib PATInterfaces
   PRIVATE_LINK_LIBRARIES xAODTrigger PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( MuonEfficiencyCorrections
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODMuon PATInterfaces AthContainers MuonAnalysisInterfacesLib
      AthenaBaseComps GaudiKernel MuonEfficiencyCorrectionsLib )
endif()

atlas_add_dictionary( MuonEfficiencyCorrectionsDict
   MuonEfficiencyCorrections/MuonEfficiencyCorrectionsDict.h
   MuonEfficiencyCorrections/selection.xml
   LINK_LIBRARIES MuonEfficiencyCorrectionsLib )

# Executable(s) in the package:
macro( _add_exec name )
   atlas_add_executable( ${name}
      util/${name}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODMuon MuonAnalysisInterfacesLib
      PATInterfaces xAODCore AsgAnalysisInterfaces PileupReweightingLib MuonEfficiencyCorrectionsLib )
endmacro( _add_exec )

if( XAOD_STANDALONE )
   _add_exec( MuonEfficiencyCorrectionsRootCoreTest )
   _add_exec( MuonEfficiencyCorrectionsSFFilesTest )
   _add_exec( MuonTriggerSFRootCoreTest )
   _add_exec( MuonTriggerSFFilesTest )
   _add_exec( MuonTriggerSFConfGenerator )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
