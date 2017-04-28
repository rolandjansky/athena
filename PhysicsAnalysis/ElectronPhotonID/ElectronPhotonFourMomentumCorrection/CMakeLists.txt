# $Id: CMakeLists.txt 794027 2017-01-26 15:03:20Z tguillem $
###################################################################################
# Package: ElectronPhotonFourMomentumCorrection
###################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonFourMomentumCorrection )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PRIVATE
                          Control/AthenaBaseComps
			  Control/AthAnalysisBaseComps
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking
			  Event/xAOD/xAODMetaData
                          GaudiKernel
                          Reconstruction/egamma/egammaLayerRecalibTool
                          Reconstruction/egamma/egammaMVACalib
                          TestPolicy
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ElectronPhotonFourMomentumCorrectionLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS ElectronPhotonFourMomentumCorrection
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODCaloEvent xAODEgamma xAODEventInfo PATInterfaces PATCoreLib egammaMVACalibLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODBase xAODCore xAODTracking xAODMetaData GaudiKernel egammaLayerRecalibTool PathResolver)

atlas_add_component( ElectronPhotonFourMomentumCorrection
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODEventInfo PATCoreLib PATInterfaces AthenaBaseComps xAODBase xAODCore xAODTracking GaudiKernel egammaLayerRecalibTool egammaMVACalibLib PathResolver ElectronPhotonFourMomentumCorrectionLib )

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

