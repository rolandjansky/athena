# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
These minimal job options are for testing only!!!
"""
import AthenaPoolCnvSvc.ReadAthenaPool

# lxplus
ServiceMgr.EventSelector.InputCollections = \
    ['/scratchdisk2/end/data/valid1.105200.T1_McAtNlo_Jimmy.recon.AOD.e603_s932_r1588_tid172295_00/AOD.172295._000154.pool.root.1']

# SFU T3
#ServiceMgr.EventSelector.InputCollections = \
#    ['/cluster/data03/endw/ntuples/AOD/valid1.105200.T1_McAtNlo_Jimmy.recon.AOD.e603_s932_r1588_tid172295_00/AOD.172295._000001.pool.root.1']

from AthenaCommon.AppMgr import theApp
from TauDiscriminant.TauDiscriGetter import TauDiscriGetter

#TauDiscriGetter(do_upstream_algs = True, msglevel = VERBOSE)
#TauDiscriGetter(do_upstream_algs = False, do_only_fakebits = True, msglevel = VERBOSE)

TauDiscriGetter(msglevel=VERBOSE, do_Pi0=True)

theApp.EvtMax = 10
