# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# @file TrigBJetD3PDMaker/python/TrigBJetD3PDProdFlags.py
# @author scott snyder
# @date Feb, 2013
# @brief Define flags and configuration settings used when making 
#        bjet trigger D3PDs from production transforms.

from AthenaCommon.JobProperties import JobProperty


class WriteTrigBJetD3PD (JobProperty):
    """Produce the TrigBJet D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_TRIGBJET'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TrigBJetD3PDMaker/TrigBJetD3PD_prodJobOFragment.py"
    TreeNames = ['TrigBjet']
    SubSteps = ['a2d', 'e2d']
