# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate all egammaTools with default configuration"
__author__ = "Bruno Lenzi"

import egammaToolsConf
from egammaRec.Factories import FcnWrapper, ToolFactory, FullNameWrapper
from egammaRec import egammaKeys
from RecExConfig.RecFlags import rec

def configureClusterCorrections(swTool):
  "Add attributes ClusterCorrectionToolsXX to egammaSwTool object"
  from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections
  from CaloRec.CaloRecMakers import _process_tools
  
  clusterTypes = dict(
    Ele35='ele35', Ele55='ele55', Ele37='ele37',
    Gam35='gam35_unconv', Gam55='gam55_unconv',
    Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
  )

  for attrName, clName in clusterTypes.iteritems():
    x = 'ClusterCorrectionTools' + attrName
    if not hasattr(swTool, x) or getattr(swTool, x):
      continue
    y = make_CaloSwCorrections(clName, suffix='EG', cells_name=egammaKeys.caloCellKey() )
    setattr(swTool, x, _process_tools (swTool, y) )

#-------------------------

egammaSwTool = ToolFactory(egammaToolsConf.egammaSwTool,
  postInit=[configureClusterCorrections])


EMClusterTool = ToolFactory(egammaToolsConf.EMClusterTool,
  OutputClusterContainerName = egammaKeys.outputClusterKey(),
  ElectronContainerName = egammaKeys.outputElectronKey(),
  PhotonContainerName = egammaKeys.outputPhotonKey(),
  ClusterCorrectionToolName = FullNameWrapper(egammaSwTool)
)


egammaCheckEnergyDepositTool = ToolFactory(egammaToolsConf.egammaCheckEnergyDepositTool,
                                           UseThrFmax=True, 
                                           ThrF0max=0.9,
                                           ThrF1max=0.8,
                                           ThrF2max=0.98,
                                           ThrF3max=0.8)


from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
EMTrackIsolationTool = ToolFactory( egammaToolsConf.EMTrackIsolationTool,
  TrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool),
  useBremAssoc = True)


from EMBremCollectionBuilder import egammaBremCollectionBuilder
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
EMBremCollectionBuilder = ToolFactory( egammaBremCollectionBuilder,
                                       name = 'EMBremCollectionBuilder',
                                       ExtrapolationTool = EMExtrapolationTools,
                                       OutputTrackContainerName=egammaKeys.outputTrackKey(),
                                       ClusterContainerName=egammaKeys.inputClusterKey(),
                                       DoTruth=rec.doTruth()
)



EMConversionBuilder = ToolFactory( egammaToolsConf.EMConversionBuilder,
                                   egammaRecContainerName = egammaKeys.outputEgammaRecKey(),
                                   ConversionContainerName = egammaKeys.outputConversionKey(),
                                   ExtrapolationTool = EMExtrapolationTools)   


EMAmbiguityTool = ToolFactory( egammaToolsConf.EMAmbiguityTool )



from egammaTrackTools.egammaTrackToolsFactories import convUtils
import EventKernel.ParticleDataType
EMFourMomBuilder = ToolFactory( egammaToolsConf.EMFourMomBuilder,
                                ConversionUtils          = convUtils,
                                dataType=EventKernel.ParticleDataType.Full )

# Electron Selectors
from EMPIDBuilderBase import EMPIDBuilderElectronBase
ElectronPIDBuilder = ToolFactory( EMPIDBuilderElectronBase, name = "ElectronPIDBuilder")

# Photon Selectors
from EMPIDBuilderBase import EMPIDBuilderPhotonBase
PhotonPIDBuilder = ToolFactory( EMPIDBuilderPhotonBase, name = "PhotonPIDBuilder")

#-------------------------

# Import the factories that are not defined here
from EMShowerBuilder import EMShowerBuilder
from egammaOQFlagsBuilder import egammaOQFlagsBuilder
from EMTrackMatchBuilder import EMTrackMatchBuilder
from EMVertexBuilder import EMVertexBuilder
