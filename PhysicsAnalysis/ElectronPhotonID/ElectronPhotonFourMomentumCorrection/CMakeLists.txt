# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ElectronPhotonFourMomentumCorrection )

# Environment specific extra dependencies:
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs AthAnalysisBaseCompsLib )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Boost )

# Component(s) in the package:
atlas_add_library( ElectronPhotonFourMomentumCorrectionLib
   ElectronPhotonFourMomentumCorrection/*.h Root/*.cxx
   PUBLIC_HEADERS ElectronPhotonFourMomentumCorrection
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AsgMessagingLib xAODCaloEvent xAODEgamma
   xAODEventInfo PATInterfaces EgammaAnalysisInterfacesLib egammaUtils PATCoreAcceptLib
   xAODRootAccess xAODTracking
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} AthLinks xAODMetaData egammaLayerRecalibTool
   egammaMVACalibAnalysisLib PathResolver ${extra_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ElectronPhotonFourMomentumCorrection
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps
      EgammaAnalysisInterfacesLib ElectronPhotonFourMomentumCorrectionLib GaudiKernel
      PATInterfaces xAODBase xAODCaloEvent xAODCore xAODEgamma xAODEventInfo xAODTruth )
endif()

atlas_add_dictionary( ElectronPhotonFourMomentumCorrectionDict
   ElectronPhotonFourMomentumCorrection/ElectronPhotonFourMomentumCorrectionDict.h
   ElectronPhotonFourMomentumCorrection/selection.xml
   LINK_LIBRARIES ElectronPhotonFourMomentumCorrectionLib )

atlas_add_executable( ElectronPhotonFourMomentumCorrection_test_memory
   util/ElectronPhotonFourMomentumCorrection_test_memory.cxx
   LINK_LIBRARIES ElectronPhotonFourMomentumCorrectionLib )

atlas_add_executable( ElectronPhotonFourMomemtumCorrection_testMomentumSystematics
   util/testMomentumSystematics.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ElectronPhotonFourMomentumCorrectionLib )

atlas_add_executable( ElectronPhotonFourMomemtumCorrection_testResolutionParam
   util/testResolutionParam.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ElectronPhotonFourMomentumCorrectionLib )

atlas_add_executable( ElectronPhotonFourMomentumCorrection_testUniformityCorrections
   util/testUniformityCorrections.cxx
   LINK_LIBRARIES ElectronPhotonFourMomentumCorrectionLib )

if( XAOD_STANDALONE )
   atlas_add_executable( ElectronPhotonFourMomentumCorrection_testEgammaCalibTool
      util/testEgammaCalibTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEventInfo xAODEgamma
      xAODCaloEvent xAODCore PATInterfaces ElectronPhotonFourMomentumCorrectionLib )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} ) 
atlas_install_joboptions( share/*.py )
