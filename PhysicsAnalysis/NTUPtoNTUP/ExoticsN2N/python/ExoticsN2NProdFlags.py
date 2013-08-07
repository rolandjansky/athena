# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty

class WriteEXMJNTUP (JobProperty):
    """NTUP for exotics multijet"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EXMJ'
    FileName = ''
    isVirtual = False
    SkimNTUPScript = "ExoticsN2N/EXMJNTUP_prodJobOFragment.py" 
    TreeNames = ['susy']
    SubSteps = ['n2n']

