# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ElectronEfficiencyCorrection )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist MathCore)

if (XAOD_STANDALONE)
  set (extra_private_libs )
else ()
  set (extra_private_libs AthAnalysisBaseCompsLib)
endif ()

# Component(s) in the package:
atlas_add_library( ElectronEfficiencyCorrectionLib
   ElectronEfficiencyCorrection/*.h Root/*.cxx
   PUBLIC_HEADERS ElectronEfficiencyCorrection
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AsgTools
   xAODEgamma PATInterfaces PATCoreLib AsgAnalysisInterfaces EgammaAnalysisInterfacesLib
   PRIVATE_LINK_LIBRARIES ${extra_private_libs} xAODCaloEvent xAODCore xAODEventInfo xAODTracking
   xAODMetaData PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ElectronEfficiencyCorrection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES  AthenaBaseComps GaudiKernel ElectronEfficiencyCorrectionLib )
endif()

atlas_add_dictionary( ElectronEfficiencyCorrectionDict
   ElectronEfficiencyCorrection/ElectronEfficiencyCorrectionDict.h
   ElectronEfficiencyCorrection/selection.xml
   LINK_LIBRARIES ElectronEfficiencyCorrectionLib )

# Utilities provided by the package:
atlas_add_executable( EgEfficiencyCorr_mem_check
   util/EgEfficiencyCorr_mem_check.cxx
   LINK_LIBRARIES AsgTools ElectronEfficiencyCorrectionLib CxxUtils)

#Test
atlas_add_test(ut_SimpleInitTest
	       SCRIPT EgEfficiencyCorr_mem_check)
# AnalysisBase-only utilities:
if( XAOD_STANDALONE )

   atlas_add_executable( EgEfficiencyCorr_testEgEfficiencyCorr
      util/testEgEfficiencyCorr.cxx util/SFHelpers.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCore ElectronEfficiencyCorrectionLib )

   atlas_add_executable( EgEfficiencyCorr_testEgEfficiencyCorrFwd
      util/testEgEfficiencyCorrFwd.cxx util/SFHelpers.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCore ElectronEfficiencyCorrectionLib )
    
   atlas_add_executable( EgEfficiencyCorr_testEgEfficiencyCorrWithoutFile
      util/testEgEfficiencyCorrWithoutFile.cxx 
      util/CreateDummyEl.cxx util/SFHelpers.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCore xAODCaloEvent xAODTracking AsgTools PATInterfaces
      ElectronEfficiencyCorrectionLib )

   atlas_add_executable( EgEfficiencyCorr_testEgChargeCorr
      util/testEgChargeCorr.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCore ElectronPhotonSelectorToolsLib ElectronEfficiencyCorrectionLib )
   
   atlas_install_python_modules(util/*.py)
   #Tests
   atlas_add_test(ut_RunOnASGAOD
	          SCRIPT EgEfficiencyCorr_testEgEfficiencyCorr  $ASG_TEST_FILE_MC 40)
 
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py)
