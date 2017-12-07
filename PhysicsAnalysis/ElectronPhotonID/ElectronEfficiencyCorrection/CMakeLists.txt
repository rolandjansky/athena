# $Id: CMakeLists.txt 797023 2017-02-14 11:03:58Z christos $
################################################################################
# Package: ElectronEfficiencyCorrection
################################################################################

# Declare the package name:
atlas_subdir( ElectronEfficiencyCorrection )

# Extra dependencies when not in AnalysisBase:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
else()
   set( extra_deps Control/xAODRootAccess )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces	
   PRIVATE
   Control/AthAnalysisBaseComps
   Tools/PathResolver
   Control/CxxUtils
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTracking
   Event/xAOD/xAODMetaData
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools	
   ${extra_deps} )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core MathCore Hist RIO MathMore )

# Component(s) in the package:
atlas_add_library( ElectronEfficiencyCorrectionLib
   ElectronEfficiencyCorrection/*.h Root/*.cxx
   PUBLIC_HEADERS ElectronEfficiencyCorrection
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AsgTools
   xAODEgamma PATInterfaces PATCoreLib AsgAnalysisInterfaces EgammaAnalysisInterfacesLib
   PRIVATE_LINK_LIBRARIES xAODCaloEvent xAODCore xAODEventInfo xAODTracking
   xAODMetaData PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ElectronEfficiencyCorrection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES xAODEgamma PATInterfaces AthenaBaseComps xAODCore
      xAODEventInfo GaudiKernel ElectronEfficiencyCorrectionLib )
endif()

atlas_add_dictionary( ElectronEfficiencyCorrectionDict
   ElectronEfficiencyCorrection/ElectronEfficiencyCorrectionDict.h
   ElectronEfficiencyCorrection/selection.xml
   LINK_LIBRARIES ElectronEfficiencyCorrectionLib )

# Utilities provided by the package:
atlas_add_executable( EgEfficiencyCorr_mem_check
   util/EgEfficiencyCorr_mem_check.cxx
   LINK_LIBRARIES AsgTools ElectronEfficiencyCorrectionLib )

# AnalysisBase-only utilities:
if( XAOD_STANDALONE )

   atlas_add_executable( EgEfficiencyCorr_testCorrelationModels
      util/testCorrelationModels.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCore xAODCaloEvent xAODTracking AsgTools PATInterfaces
      ElectronEfficiencyCorrectionLib )

   atlas_add_executable( EgEfficiencyCorr_testEgEfficiencyCorr
      util/testEgEfficiencyCorr.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCore ElectronEfficiencyCorrectionLib )

   atlas_add_executable( EgEfficiencyCorr_testEgEfficiencyCorrWithoutFile
      util/testEgEfficiencyCorrWithoutFile.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCore xAODCaloEvent xAODTracking AsgTools PATInterfaces
      ElectronEfficiencyCorrectionLib )

   atlas_add_executable( EgEfficiencyCorr_testEgChargeCorr
      util/testEgChargeCorr.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCore ElectronPhotonSelectorToolsLib ElectronEfficiencyCorrectionLib )

endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( data/*.root data/*.txt )
