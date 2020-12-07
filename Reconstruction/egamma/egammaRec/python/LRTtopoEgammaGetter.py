# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

# specifies standard (supercluster) egamma.
from AthenaCommon.Logging import logging
from AthenaCommon.DetFlags import DetFlags
from RecExConfig.Configured import Configured
from egammaRec import egammaRecFlags as egRecFlags
egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags


def doConversions():
    return DetFlags.detdescr.ID_on() and egammaRecFlags.doConversions()

# The tools used to add properties


class LRTtopoEgammaGetter (Configured):

    def configure(self):
        mlog = logging.getLogger('LRTtopoEgammaGetter.py::configure:')
        mlog.info('entering')

        # the egammaRecBuilder
        from egammaRec.LRTEgammaConfig import LRTegammaRecBuilder
        try:
            self._LRTegammaRecBuilder = LRTegammaRecBuilder(
                doTrackMatching=DetFlags.detdescr.ID_on(),
                doConversions=False)
        except Exception:
            mlog.error("could not get handle to LRTegammaRecBuilder")
            import traceback
            traceback.print_exc()
            return False

        # the supercluster builders
        from egammaRec.LRTEgammaConfig import (
            LRTelectronSuperClusterBuilder, LRTphotonSuperClusterBuilder)
        try:
            self._LRTelectronSuperClusterBuilder = LRTelectronSuperClusterBuilder(
                doTrackMatching=DetFlags.detdescr.ID_on())
            self._LRTphotonSuperClusterBuilder = LRTphotonSuperClusterBuilder(
                doConversions=False)
        except Exception:
            mlog.error("could not get handle to supcluster builders")
            import traceback
            traceback.print_exc()
            return False

        # the topoEgammaBuilder (the part that puts everything together
        from egammaRec.LRTEgammaConfig import LRTtopoEgammaBuilder
        try:
            self._LRTtopoEgammaBuilder = LRTtopoEgammaBuilder(
                doPhotons=False
            )
        except Exception:
            mlog.error("could not get handle to LRTtopoEgammaBuilder")
            import traceback
            traceback.print_exc()
            return False

        return True
