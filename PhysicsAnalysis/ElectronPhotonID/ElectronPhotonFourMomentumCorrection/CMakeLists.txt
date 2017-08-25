# $Id: CMakeLists.txt 794027 2017-01-26 15:03:20Z tguillem $
###################################################################################
# Package: ElectronPhotonFourMomentumCorrection
###################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonFourMomentumCorrection )

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

# Component(s) in the package:
atlas_add_library( ElectronPhotonFourMomentumCorrectionLib
   ElectronPhotonFourMomentumCorrection/*.h Root/*.cxx
   PUBLIC_HEADERS ElectronPhotonFourMomentumCorrection
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma
   xAODEventInfo PATInterfaces EgammaAnalysisInterfaces PATCoreLib egammaMVACalibLib xAODTruth
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
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODEventInfo PATCoreLib PATInterfaces AthenaBaseComps xAODBase xAODCore xAODTracking GaudiKernel AthAnalysisBaseCompsLib egammaLayerRecalibTool egammaMVACalibLib PathResolver ElectronPhotonFourMomentumCorrectionLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_xmls( data/*.root )
atlas_install_data( data/*.root )

