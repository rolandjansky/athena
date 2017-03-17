# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies egammaForwardBuilder "standard"
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
import traceback

from RecExConfig.Configured import Configured

from egammaRec import egammaKeys
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory, FcnWrapper, FullNameWrapper
from egammaRec import egammaRecFlags as egRecFlags
egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags
from egammaTools.egammaToolsFactories import egammaOQFlagsBuilder, EMFourMomBuilder, LooseForwardElectronSelector, MediumForwardElectronSelector, TightForwardElectronSelector
from ElectronPhotonSelectorTools.ForwardElectronIsEMSelectorMapping import forwardelectronPIDmenu

def forwardSelectorTools():
   "Return a list with the selectors for forward electrons"
   return [ LooseForwardElectronSelector(), MediumForwardElectronSelector(), TightForwardElectronSelector() ]

egammaForwardBuilder = AlgFactory( egammaAlgsConf.egammaForwardBuilder,
                                   name = 'egammaForward',
                                   
                                   # Keys
                                   ElectronOutputName=egammaKeys.outputFwdElectronKey(),
                                   TopoClusterName="CaloCalTopoClusters",
                                   ClusterContainerName=egammaKeys.outputFwdClusterKey(),
                                   
                                   # Tools
                                   ObjectQualityToolName = FullNameWrapper( egammaOQFlagsBuilder ),
                                   FourMomBuilderTool = EMFourMomBuilder,
                                  
                                   # Flags and other properties
                                   EtCut=5*GeV,
                                   EtaCut=2.5,
                                   
                                   forwardelectronIsEMselectors = FcnWrapper(forwardSelectorTools),
                                   forwardelectronIsEMselectorResultNames = ["Loose", "Medium", "Tight"]
                                   )

class egammaForwardGetter ( Configured ) :
   
   def configure(self):
      mlog = logging.getLogger ('egammaForwardGetter.py::configure:')
      mlog.info('entering')
      
      # configure egammaForward here:
      try:
         self._egammaFwdBuilderHandle = egammaForwardBuilder()
      except Exception:
         mlog.error("could not get handle to egammaForward")
         print traceback.format_exc()
         return False
      return True
   
   def egammaFwdBuilderHandle(self):
      return self._egammaFwdBuilderHandle
   
   
