# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies standard (supercluster) egamma.
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

#Some handy flags

def doConversions() :
  return DetFlags.detdescr.ID_on() and egammaRecFlags.doConversions()
################

# Import the tool factories
from egammaTools.egammaToolsFactories import \
    EMClusterTool, EMFourMomBuilder, EMShowerBuilder, egammaOQFlagsBuilder, \
    ElectronPIDBuilder, PhotonPIDBuilder

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

class topoEgammaGetter ( Configured ) :

    def configure(self):
        mlog = logging.getLogger ('topoEgammaGetter.py::configure:')
        mlog.info('entering')

        # the egammaTopoClusterCopier
        from egammaAlgs.egammaTopoClusterCopier import egammaTopoClusterCopier
        try:
            self._egammaTopoClusterCopierHandle = egammaTopoClusterCopier()
        except Exception:
            mlog.error("could not get handle to egammaTopoClusterCopier")
            import traceback
            print traceback.format_exc()
            return False
        print self._egammaTopoClusterCopierHandle


        # the egammaRecBuilder
        from egammaAlgs.egammaRecBuilder import egammaRecBuilder
        try:
            self._egammaRecBuilder = egammaRecBuilder(doTrackMatching = DetFlags.detdescr.ID_on(),
                                                      doConversions = doConversions())
        except Exception:
            mlog.error("could not get handle to egammaRecBuilder")
            import traceback
            print traceback.format_exc()
            return False
        print self._egammaRecBuilder

        # the supercluster builders
        from egammaAlgs.egammaSuperClusterBuilder import \
            electronSuperClusterBuilder, photonSuperClusterBuilder
        try:
          self._electronSuperClusterBuilder = electronSuperClusterBuilder(
            doTrackMatching = DetFlags.detdescr.ID_on())
          self._photonSuperClusterBuilder = photonSuperClusterBuilder(
            doConversions = doConversions())
        except Exception:
            mlog.error("could not get handle to supcluster builders")
            import traceback
            print traceback.format_exc()
            return False
        print self._electronSuperClusterBuilder
        print self._photonSuperClusterBuilder

        # the topoEgammaBuilder (the part that puts everything together
        from egammaAlgs.topoEgammaBuilder import topoEgammaBuilder
        try:
            self._topoEgammaBuilder = topoEgammaBuilder(                                   
                               # Decoration tools
                               egammaTools = FcnWrapper(egammaDecorationTools),
                               ElectronTools = FcnWrapper(electronDecorationTools),
                               PhotonTools = FcnWrapper(photonDecorationTools)
                               )
        except Exception:
            mlog.error("could not get handle to topoEgammaBuilder")
            import traceback
            print traceback.format_exc()
            return False
        print self._topoEgammaBuilder

        return True

