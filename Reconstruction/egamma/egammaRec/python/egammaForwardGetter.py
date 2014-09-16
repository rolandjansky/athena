# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies egammaForwardBuilder "standard"
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
import traceback

from RecExConfig.Configured import Configured

from egammaRec import egammaKeys
from egammaRec import egammaRecConf
from egammaRec.Factories import AlgFactory, FcnWrapper, FullNameWrapper
from egammaRec import egammaRecFlags as egRecFlags
egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags

from egammaTools.egammaToolsFactories import egammaOQFlagsBuilder, EMFourMomBuilder

egammaForwardBuilder = AlgFactory( egammaRecConf.egammaForwardBuilder,
    name = 'egammaForward',
    
    # Keys
    ElectronOutputName=egammaKeys.outputFwdElectronKey(),
    TopoClusterName="CaloCalTopoCluster",
    ClusterContainerName=egammaKeys.outputFwdClusterKey(),
    
    # Tools
    ObjectQualityToolName = FullNameWrapper( egammaOQFlagsBuilder ),
    FourMomBuilderTool = EMFourMomBuilder,
    
    # Flags and other properties
    EtCut=5*GeV,
    EtaCut=2.5,
    EtaFCalCut=3.2,
    FirstENGdensCuts=[0.069,0.56],
    FracMaxCuts     =[0.39,0.45],
    LongMomentCuts  =[0.22,0.3],
    SecondLambdaCuts=[6000,7300],
    LatMomentCuts   =[0.57,0.3],
    SecondRCuts     =[2600,1000],
    CenterLambdaCuts=[280,280],
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
