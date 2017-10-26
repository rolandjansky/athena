###################################################################################
# Package: ElectronPhotonFourMomentumCorrection
###################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonFourMomentumCorrection )

# Environment specific extra dependencies:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess )
else()
   set( extra_dep Control/AthenaBaseComps
      Control/AthAnalysisBaseComps
      GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces	
   PRIVATE
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODTracking
   Event/xAOD/xAODMetaData
   Reconstruction/egamma/egammaLayerRecalibTool
   Reconstruction/egamma/egammaMVACalib
   Event/xAOD/xAODTruth
   Tools/PathResolver
   ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Boost )

# Component(s) in the package:
atlas_add_library( ElectronPhotonFourMomentumCorrectionLib
   ElectronPhotonFourMomentumCorrection/*.h Root/*.cxx
   PUBLIC_HEADERS ElectronPhotonFourMomentumCorrection
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma
   xAODEventInfo PATInterfaces EgammaAnalysisInterfacesLib PATCoreLib egammaMVACalibLib xAODTruth
   PRIVATE_LINK_LIBRARIES xAODTracking xAODMetaData egammaLayerRecalibTool
   PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ElectronPhotonFourMomentumCorrection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgTools xAODCaloEvent xAODEgamma xAODEventInfo
      PATInterfaces AthenaBaseComps xAODBase xAODCore GaudiKernel
      ElectronPhotonFourMomentumCorrectionLib )
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
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCaloEvent xAODCore PATInterfaces
      ElectronPhotonFourMomentumCorrectionLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_test( ut_test SCRIPT test/ut_test.py )
   atlas_add_test( ut_test_factory SCRIPT test/ut_test_factory.py )
   atlas_add_test( ut_test_resolution SCRIPT test/ut_test_resolution.py )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_xmls( data/*.root )
atlas_install_data( data/*.root )
