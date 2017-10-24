# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This file is for the "old" non-topocluster version of egamma
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

from AthenaCommon.DetFlags import DetFlags
from RecExConfig.Configured import Configured
#
from egammaRec import egammaKeys
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory, FcnWrapper
from egammaRec import egammaRecFlags as egRecFlags
egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags

def doConversions() :
  return DetFlags.detdescr.ID_on() and egammaRecFlags.doConversions()
################

# Import the tool factories
from egammaTools.egammaToolsFactories import \
    EMTrackMatchBuilder,\
    EMConversionBuilder, EGammaAmbiguityTool,\
    EMClusterTool, EMFourMomBuilder, EMShowerBuilder, egammaOQFlagsBuilder, \
    ElectronPIDBuilder, PhotonPIDBuilder

#
#Topo seeded fixed size clusters, related configurations
#Create and return the TopoSeededCollection
def getTopoSeededCollectionName():
  if egammaRecFlags.doTopoCaloSeeded():
    from CaloRec import CaloRecFlags
    from CaloRec.CaloRecTopoEM35Flags import jobproperties
    jobproperties.CaloRecTopoEM35Flags.EtSeedCut =0.8 * GeV # The cut on the Fixed size is 1.5 GeV later on 
    from CaloRec.CaloClusterTopoEMFixedSizeGetter import CaloClusterTopoEMFixedSizeGetter
    theCaloClusterTopoEMFixedSizeGetter = CaloClusterTopoEMFixedSizeGetter()
    return theCaloClusterTopoEMFixedSizeGetter.outputKey()
  else:
    return ""

#The tools used to add properties 
def egammaDecorationTools():
  "Return a list with the tools that decorate both electrons and photons"
  return [EMClusterTool(), EMFourMomBuilder(), EMShowerBuilder(), egammaOQFlagsBuilder()]

def electronDecorationTools():
  "Return a list with the tools that decorate only electrons"
  return [ ElectronPIDBuilder() ]

def photonDecorationTools():
  "Return a list with the tools that decorate only photons"
  return [ PhotonPIDBuilder() ]
#

class egammaGetter ( Configured ) :

    def configure(self):
        mlog = logging.getLogger ('egammaGetter.py::configure:')
        mlog.info('entering')        

        egammaBuilder = AlgFactory(egammaAlgsConf.egammaBuilder, name = 'egamma',
                                   # Keys
                                   ElectronOutputName = egammaKeys.outputElectronKey(),
                                   PhotonOutputName = egammaKeys.outputPhotonKey(),
                                   TopoSeededClusterContainerName = getTopoSeededCollectionName(),                                     
                                   # Builder tools
                                   TrackMatchBuilderTool = EMTrackMatchBuilder,
                                   ConversionBuilderTool = EMConversionBuilder if doConversions() else None,
                                   AmbiguityTool = EGammaAmbiguityTool,                                     
                                   # Decoration tools
                                   egammaTools = FcnWrapper(egammaDecorationTools),
                                   ElectronTools = FcnWrapper(electronDecorationTools),
                                   PhotonTools = FcnWrapper(photonDecorationTools),                                     
                                   # Flags and other properties
                                   # Track matching depending if ID is on/off
                                   doTrackMatching = DetFlags.detdescr.ID_on(),
                                   # conversions matching depending if ID is on/off
                                   doConversions = doConversions(),
                                   #
                                   clusterEnergyCut = 10*MeV,
                                   doTopoSeededPhotons = egammaRecFlags.doTopoCaloSeeded()
                                   )          
        
        # configure egamma here:
        try:
            self._egammaBuilderHandle = egammaBuilder()
        except Exception:
            mlog.error("could not get handle to egamma")
            import traceback
            print traceback.format_exc()
            return False
        
        print self._egammaBuilderHandle
        return True

    def egammaBuilderHandle(self):
        return self._egammaBuilderHandle
