# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# hook for the AthenaRootComps py-module

"""
This module allows people to read a chain of ttrees from inside Athena, merging ROOT's usual event loop over trees and Gaudi event loop.

ex:

from AthenaCommon.AppMgr import (theApp,
                                 ServiceMgr as svcMgr)

# install the ROOT event loop
import AthenaRootComps.ReadAthenaRoot

import AthenaRootComps.ReadAthenaxAOD

svcMgr.THistSvc.OutputLevel = DEBUG
svcMgr.EventSelector.OutputLevel = DEBUG

# cbnt files obtained with:
# athena.py CBNT_AOD/test_CBNT_TruthParticle_jobOptions.py
svcMgr.EventSelector.InputCollections = [
    'cbnt_aod.aan.1.root',
    'cbnt_aod.aan.2.root',
    ]
svcMgr.EventSelector.TupleName = 'CollectionTree'
svcMgr.EventSelector.SkipEvents = 10
theApp.EvtMax = -1

"""
