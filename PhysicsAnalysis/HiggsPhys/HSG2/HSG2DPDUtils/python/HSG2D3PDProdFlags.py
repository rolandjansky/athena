# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty

class WriteHSG2D3PD (JobProperty):
    """Produce the Higgs SG2 D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_HSG2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG2DPDUtils/hsg2_d3pdmaker_plain.prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']

class Write2LHSG2D3PD (JobProperty):
    """Produce the Higgs SG2 D3PD for the 2-lepton filter."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_2LHSG2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG2DPDUtils/hsg2_d3pdmaker_2L.prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']

class Write4LHSG2D3PD (JobProperty):
    """Produce the Higgs SG2 D3PD for the 4-lepton filter."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_4LHSG2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG2DPDUtils/hsg2_d3pdmaker_4L.prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']

class Write2L2QHSG2D3PD (JobProperty):
    """Produce the Higgs SG2 D3PD for the 2L2Q filter."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_2L2QHSG2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG2DPDUtils/hsg2_d3pdmaker_2L2Q.prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']

class WriteTPHSG2D3PD (JobProperty):
    """Produce the Higgs SG2 D3PD for the 2L filter dedicated to the Z tag-and-probe. """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_TPHSG2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG2DPDUtils/hsg2_d3pdmaker_TP.prodJobOFragment.py" 
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']

