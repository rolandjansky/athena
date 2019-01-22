# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        VHFlags.py
## Authors:     Amber Roepe
## Email:       amber.rebecca.roepe@cern.ch
## Created:     July 2018
##
## Description: Here, all necessary job flags for the RPV/LL ESD-based DPD
##              are defined.
##
##=============================================================================

__doc__ = """job flags for the RPV/LL VH DESD"""
__version__ = "0.0.1"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class VHLowTrackJet_containerFlags(JobProperty):
    pass
primRPVLLDESDM.add_JobProperty(VHLowTrackJet_containerFlags)

class VHLowTrackJet_FilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    triggerNames = [
        'HLT_e26_lhtight_nod0_ivarloose',
        'HLT_e60_lhmedium_nod0',
        'HLT_e140_lhloose_nod0',
        'HLT_mu26_ivarmedium'
        ]
    debug=False
    cutJet=20000#MeV
    cutJetEta=2.1
    nJets=1
    cutEl=27000 #MeV
    cutMu=25000 #MeV
    pass
primRPVLLDESDM.add_JobProperty(VHLowTrackJet_FilterFlags)



