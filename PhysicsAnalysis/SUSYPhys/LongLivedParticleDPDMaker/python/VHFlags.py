# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
        'HLT_e24_lhmedium_L1EM20VH',
        'HLT_e24_lhtight_nod0_ivarloose',
        'HLT_e26_lhtight_nod0_ivarloose',
        'HLT_e60_lhmedium',
        'HLT_e60_lhmedium_nod0',
        'HLT_e120_lhloose',
        'HLT_e140_lhloose_nod0',
        'HLT_mu20_iloose_L1MU15',
        'HLT_mu24_iloose',
        'HLT_mu24_ivarloose',
        'HLT_mu24_imedium',
        'HLT_mu24_ivarmedium',
        'HLT_mu26_imedium',
        'HLT_mu26_ivarmedium',
        'HLT_mu40',
        'HLT_mu50'
        ]
    debug=False
    cutJet=20*Units.GeV
    cutJetEta=2.1
    nJets=1
    cutEl=27*Units.GeV
    cutMu=25*Units.GeV
primRPVLLDESDM.add_JobProperty(VHLowTrackJet_FilterFlags)



