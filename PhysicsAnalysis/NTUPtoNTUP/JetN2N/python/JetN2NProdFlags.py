# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty

class WriteSlimSMQCDNTUP (JobProperty):
    """slimming NTUP"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SLIMSMQCD'
    FileName = ''
    isVirtual = False
    SkimNTUPScript = "JetN2N/SlimSMQCDNTUP_prodJobOFragment.py" 
    TreeNames = ['qcd']
    SubSteps = ['n2n']

