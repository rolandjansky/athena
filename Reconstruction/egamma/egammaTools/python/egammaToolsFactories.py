# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate all egammaTools with default configuration"
__author__ = "Bruno Lenzi"

import egammaToolsConf
from egammaRec.Factories import FcnWrapper, ToolFactory, FullNameWrapper
from egammaRec import egammaKeys
from egammaRec.egammaRecFlags import jobproperties # to set jobproperties.egammaRecFlags
from RecExConfig.RecFlags import rec

def configureClusterCorrections(swTool):
  "Add attributes ClusterCorrectionToolsXX to egammaSwTool object"
  from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections
  from CaloRec.CaloRecMakers import _process_tools
  
  clusterTypes = dict(
    Ele35='ele35', Ele55='ele55', Ele37='ele37',
    Gam35='gam35_unconv', Gam55='gam55_unconv',Gam37='gam37_unconv',
    Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
  )

  for attrName, clName in clusterTypes.iteritems():
    x = 'ClusterCorrectionTools' + attrName
    if not hasattr(swTool, x) or getattr(swTool, x):
      continue
    y = make_CaloSwCorrections(clName, suffix='EG',
      version = jobproperties.egammaRecFlags.clusterCorrectionVersion(),
      cells_name=egammaKeys.caloCellKey() )
    setattr(swTool, x, _process_tools (swTool, y) )

#-------------------------

egammaSwTool = ToolFactory(egammaToolsConf.egammaSwTool,
  postInit=[configureClusterCorrections])

from egammaMVACalib import egammaMVACalibConf 
egammaMVATool =  ToolFactory(egammaMVACalibConf.egammaMVATool,
                              folder="egammaMVACalib/offline/v3_E4crack_bis")

EMClusterTool = ToolFactory(egammaToolsConf.EMClusterTool,
                            OutputClusterContainerName = egammaKeys.outputClusterKey(),
                            OutputTopoSeededClusterContainerName = egammaKeys.outputTopoSeededClusterKey(),
                            ElectronContainerName = egammaKeys.outputElectronKey(),
                            PhotonContainerName = egammaKeys.outputPhotonKey(),
                            ClusterCorrectionToolName = FullNameWrapper(egammaSwTool),
                            doSuperCluster = jobproperties.egammaRecFlags.doSuperclusters(),
                            MVACalibTool= egammaMVATool
)


egammaCheckEnergyDepositTool = ToolFactory(egammaToolsConf.egammaCheckEnergyDepositTool,
                                           UseThrFmax=True, 
                                           ThrF0max=0.9,
                                           ThrF1max=0.8,
                                           ThrF2max=0.98,
                                           ThrF3max=0.8)


from EMBremCollectionBuilder import egammaBremCollectionBuilder
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
EMBremCollectionBuilder = ToolFactory( egammaBremCollectionBuilder,
                                       name = 'EMBremCollectionBuilder',
                                       ExtrapolationTool = EMExtrapolationTools,
                                       OutputTrackContainerName=egammaKeys.outputTrackKey(),
                                       ClusterContainerName=egammaKeys.inputClusterKey(),
                                       UseBremFinder=jobproperties.egammaRecFlags.doBremFinding(),
                                       DoTruth=rec.doTruth()
)


EMConversionBuilder = ToolFactory( egammaToolsConf.EMConversionBuilder,
                                   egammaRecContainerName = egammaKeys.outputEgammaRecKey(),
                                   ConversionContainerName = egammaKeys.outputConversionKey(),
                                   ExtrapolationTool = EMExtrapolationTools)   

from ElectronPhotonSelectorTools import ElectronPhotonSelectorToolsConf
EGammaAmbiguityTool = ToolFactory( ElectronPhotonSelectorToolsConf.EGammaAmbiguityTool )

EMFourMomBuilder = ToolFactory( egammaToolsConf.EMFourMomBuilder)

#Tools for doing superclustering.

egammaTopoClusterCopier = ToolFactory( egammaToolsConf.egammaTopoClusterCopier,
                                     name = 'egammaTopoClusterCopier' ,
                                     InputTopoCollection=jobproperties.egammaRecFlags.inputTopoClusterCollection(),
                                     OutputTopoCollection=jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
                                     IsHadronic = True
                                     )


electronSuperClusterBuilder = ToolFactory( egammaToolsConf.electronSuperClusterBuilder,
                                           name = 'electronSuperClusterBuilder',
                                           ExtrapolationTool=EMExtrapolationTools,
                                           UseBremFinder=jobproperties.egammaRecFlags.doBremFinding(),
                                           MVACalibTool= egammaMVATool
                                         )


photonSuperClusterBuilder = ToolFactory( egammaToolsConf.photonSuperClusterBuilder,
                                         name = 'photonSuperClusterBuilder',
                                         MVACalibTool= egammaMVATool
                                         )

egammaTopoClusterMap = ToolFactory( egammaToolsConf.egammaTopoClusterMap,
                                    name = 'egammaTopoClusterMap' )

#End of super clustering

# Electron Selectors
from EMPIDBuilderBase import EMPIDBuilderElectronBase
ElectronPIDBuilder = ToolFactory( EMPIDBuilderElectronBase, name = "ElectronPIDBuilder")

# Photon Selectors
from EMPIDBuilderBase import EMPIDBuilderPhotonBase
PhotonPIDBuilder = ToolFactory( EMPIDBuilderPhotonBase, name = "PhotonPIDBuilder")

# FarwardElectron Selectors
from ElectronPhotonSelectorTools.ConfiguredAsgForwardElectronIsEMSelectors import ConfiguredAsgForwardElectronIsEMSelector

import cppyy
cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import egammaPID

LooseForwardElectronSelector = ToolFactory( ConfiguredAsgForwardElectronIsEMSelector, name="LooseForwardElectronSelector", quality = egammaPID.ForwardElectronIDLoose )
MediumForwardElectronSelector = ToolFactory( ConfiguredAsgForwardElectronIsEMSelector, name="MediumForwardElectronSelector", quality = egammaPID.ForwardElectronIDMedium )
TightForwardElectronSelector = ToolFactory( ConfiguredAsgForwardElectronIsEMSelector, name="TightForwardElectronSelector", quality = egammaPID.ForwardElectronIDTight )

#-------------------------

# Import the factories that are not defined here
from EMShowerBuilder import EMShowerBuilder
from egammaOQFlagsBuilder import egammaOQFlagsBuilder
from EMTrackMatchBuilder import EMTrackMatchBuilder
from EMVertexBuilder import EMVertexBuilder
