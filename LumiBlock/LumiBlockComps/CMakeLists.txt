# $Id: CMakeLists.txt 789876 2016-12-15 13:57:55Z krasznaa $
################################################################################
# Package: LumiBlockComps
################################################################################

# Declare the package name:
atlas_subdir( LumiBlockComps )

# Declare the package's dependencies:
if( XAOD_ANALYSIS )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthenaBaseComps
      Control/AthenaKernel
      Control/StoreGate
      Control/xAODRootAccess
      Event/xAOD/xAODLuminosity
      GaudiKernel
      PRIVATE
      DataQuality/GoodRunsLists
      Database/AthenaPOOL/AthenaPoolUtilities
      Database/AthenaPOOL/DBDataModel
      Database/CoraCool )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthenaBaseComps
      Control/AthenaKernel
      Control/StoreGate
      Control/xAODRootAccess
      Database/CoolLumiUtilities
      Event/xAOD/xAODLuminosity
      GaudiKernel
      LumiBlock/LumiCalc
      PRIVATE
      DataQuality/GoodRunsLists
      Database/AthenaPOOL/AthenaPoolUtilities
      Database/AthenaPOOL/DBDataModel
      Database/CoraCool )
endif()

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core Tree RIO )

# Component(s) in the package:
set( extra_libs )
set( extra_srcs )
if( NOT XAOD_ANALYSIS )
   set( extra_libs LumiBlockCoolQuery CoolLumiUtilitiesLib )
   set( extra_srcs src/*.cxx )
endif()
atlas_add_library( LumiBlockCompsLib
   LumiBlockComps/*.h src/*.h Root/*.cxx
   src/CreateLumiBlockCollectionFromFile.cxx src/ILumiCalcSvc.cxx src/xAOD2NtupLumiSvc.cxx
   ${extra_srcs}
   PUBLIC_HEADERS LumiBlockComps
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps
   AthenaKernel xAODLuminosity GaudiKernel StoreGateLib ${extra_libs}
   PRIVATE_LINK_LIBRARIES ${COOL_LIBRARIES} AthenaPoolUtilities DBDataModel
   GoodRunsListsLib xAODRootAccess )

atlas_add_component( LumiBlockComps
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel LumiBlockCompsLib )

atlas_add_dictionary( LumiBlockCompsDict
   LumiBlockComps/LumiBlockCompsDict.h
   LumiBlockComps/selection.xml
   LINK_LIBRARIES LumiBlockCompsLib )

atlas_add_executable( getLumi
   util/getLumi.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODLuminosity GoodRunsListsLib
   xAODRootAccess LumiBlockCompsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
