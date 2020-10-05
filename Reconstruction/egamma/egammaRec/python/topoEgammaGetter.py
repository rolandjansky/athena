# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from egammaTools.egammaToolsFactories import \
    EMClusterTool, EMFourMomBuilder, EMShowerBuilder, egammaOQFlagsBuilder, \
    ElectronPIDBuilder, PhotonPIDBuilder

# specifies standard (supercluster) egamma.
from AthenaCommon.Logging import logging
from AthenaCommon.DetFlags import DetFlags
from RecExConfig.Configured import Configured
from egammaRec.Factories import FcnWrapper
from egammaRec import egammaRecFlags as egRecFlags
egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags


def doConversions():
    return DetFlags.detdescr.ID_on() and egammaRecFlags.doConversions()

# The tools used to add properties


def egammaDecorationTools():
    "Return a list with the tools that decorate both electrons and photons"
    return [EMFourMomBuilder()]


def electronDecorationTools():
    "Return a list with the tools that decorate only electrons"
    return [ElectronPIDBuilder()]


def photonDecorationTools():
    "Return a list with the tools that decorate only photons"
    return [PhotonPIDBuilder()]
#


class topoEgammaGetter (Configured):

    def configure(self):
        mlog = logging.getLogger('topoEgammaGetter.py::configure:')
        mlog.info('entering')

        # the egammaRecBuilder
        from egammaAlgs.egammaRecBuilder import egammaRecBuilder
        try:
            self._egammaRecBuilder = egammaRecBuilder(
                doTrackMatching=DetFlags.detdescr.ID_on(),
                doConversions=doConversions())
        except Exception:
            mlog.error("could not get handle to egammaRecBuilder")
            import traceback
            traceback.print_exc()
            return False

        # the supercluster builders
        from egammaAlgs.egammaSuperClusterBuilder import \
            electronSuperClusterBuilder, photonSuperClusterBuilder
        try:
            self._electronSuperClusterBuilder = electronSuperClusterBuilder(
                doTrackMatching=DetFlags.detdescr.ID_on())
            self._photonSuperClusterBuilder = photonSuperClusterBuilder(
                doConversions=doConversions())
        except Exception:
            mlog.error("could not get handle to supcluster builders")
            import traceback
            traceback.print_exc()
            return False

        # the topoEgammaBuilder (the part that puts everything together
        from egammaAlgs.topoEgammaBuilder import topoEgammaBuilder
        try:
            self._topoEgammaBuilder = topoEgammaBuilder(
                EMClusterTool=EMClusterTool,
                EMShowerTool=EMShowerBuilder,
                ObjectQualityTool=egammaOQFlagsBuilder,
                # Decoration tools
                egammaTools=FcnWrapper(egammaDecorationTools),
                ElectronTools=FcnWrapper(electronDecorationTools),
                PhotonTools=FcnWrapper(photonDecorationTools)
            )
        except Exception:
            mlog.error("could not get handle to topoEgammaBuilder")
            import traceback
            traceback.print_exc()
            return False

        # the egammaLargeClusterMaker
        # (Which chooses the cells to store in the AOD)
        from egammaAlgs.egammaLargeClusterMakerAlg import (
            egammaLargeClusterMakerAlg)
        try:
            self._egammaLargeClusterMaker = egammaLargeClusterMakerAlg()
        except Exception:
            mlog.error("could not get handle to egammaLargeClusterMaker")
            import traceback
            traceback.print_exc()
            return False

        return True
