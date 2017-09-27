# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate all egammaTools with default configuration"
__author__ = "Bruno Lenzi"

import egammaToolsConf
from egammaRec.Factories import FcnWrapper, ToolFactory, PublicToolFactory, FullNameWrapper
from egammaRec import egammaKeys
from egammaRec.egammaRecFlags import jobproperties # to set jobproperties.egammaRecFlags
from RecExConfig.RecFlags import rec

def configureClusterCorrections(swTool):
  "Add attributes ClusterCorrectionToolsXX to egammaSwTool object"
  from CaloClusterCorrection.CaloSwCorrections import  make_CaloSwCorrections, rfac, etaoff_b1, etaoff_e1, \
      etaoff_b2,etaoff_e2,phioff_b2,phioff_e2,update,time,listBadChannel
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

  #Super cluster position only corrections
  if jobproperties.egammaRecFlags.doSuperclusters():
    for attrName, clName in clusterTypes.iteritems():
      n = 'ClusterCorrectionToolsSuperCluster' + attrName
      if not hasattr(swTool, n) or getattr(swTool, n):
        continue
  
      setattr(swTool, n ,_process_tools(swTool, 
                                        make_CaloSwCorrections(clName, suffix ='EGSuperCluster',
                                                               corrlist=[[rfac,'v5'],[etaoff_b1,'v5'],[etaoff_e1,'v5'],
                                                                         [etaoff_b2,'v5'],[etaoff_e2,'v5'], [phioff_b2, 'v5data'], 
                                                                         [phioff_e2,  'v5data'], [update], [time], [listBadChannel]],
                                                               cells_name=egammaKeys.caloCellKey() )))
    #End of super cluster position only corrections
#-------------------------

egammaSwTool = ToolFactory(egammaToolsConf.egammaSwTool,
                           postInit=[configureClusterCorrections])

from egammaMVACalib import egammaMVACalibConf 
egammaMVATool =  PublicToolFactory(egammaMVACalibConf.egammaMVATool,
                              folder=jobproperties.egammaRecFlags.calibMVAVersion())

EMClusterTool = ToolFactory(egammaToolsConf.EMClusterTool,
                            OutputClusterContainerName = egammaKeys.outputClusterKey(),
                            OutputTopoSeededClusterContainerName = egammaKeys.outputTopoSeededClusterKey(),
                            ClusterCorrectionTool = egammaSwTool,
                            doSuperCluster = jobproperties.egammaRecFlags.doSuperclusters(),
                            MVACalibTool= egammaMVATool
                            )


egammaCheckEnergyDepositTool = ToolFactory(egammaToolsConf.egammaCheckEnergyDepositTool,
                                           UseThrFmax=True, 
                                           ThrF0max=0.9,
                                           ThrF1max=0.8,
                                           ThrF2max=0.98,
                                           ThrF3max=0.8)


from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools


EMConversionBuilder = ToolFactory( egammaToolsConf.EMConversionBuilder,
                                   ConversionContainerName = egammaKeys.outputConversionKey(),
                                   ExtrapolationTool = EMExtrapolationTools)   

from ElectronPhotonSelectorTools import ElectronPhotonSelectorToolsConf
EGammaAmbiguityTool = ToolFactory( ElectronPhotonSelectorToolsConf.EGammaAmbiguityTool )

EMFourMomBuilder = ToolFactory( egammaToolsConf.EMFourMomBuilder)

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
