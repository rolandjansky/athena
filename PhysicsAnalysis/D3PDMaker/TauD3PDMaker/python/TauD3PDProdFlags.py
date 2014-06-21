# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TauD3PDProdFlags.py 543601 2013-04-04 14:26:33Z mtm $
#
# @file TauD3PDMaker/python/TauD3PDProdFlags.py
# @author scott snyder <snyder@bnl.gov>
# @date Jun, 2012
# @brief Define flags and configuration settings used when making 
#        Tau D3PDs from production transforms.
#
# Any objects here with a name of the form Write*D3PD that derive
# from JobProperty are added to D3PDProdFlags.
#


from AthenaCommon.JobProperties import JobProperty


class WriteTauSmallD3PD (JobProperty):
     """Produce the Small Tau D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_TAUSMALL'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "TauD3PDMaker/TauSmallD3PD_prodJobOFragment.py"
     TreeNames = ['tau']
     SubSteps = ['a2d', 'e2d']


class WriteTauMediumD3PD (JobProperty):
     """Produce the Medium Tau D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_TAUMEDIUM'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "TauD3PDMaker/TauMediumD3PD_prodJobOFragment.py"
     TreeNames = ['tau']
     SubSteps = ['a2d', 'e2d']


class WriteTauFullD3PD (JobProperty):
    """Produce the TauFull D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_TAUFULL'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/TauFullD3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']


class WriteTauD3PD (JobProperty):
    """Produce the Tau D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_TAU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/TauD3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']


######################################
##       Embedding NTUP types       ##
######################################

class WriteEmbLLDND3PD (JobProperty):
    """Produce the Tau lepton-lepton isolation down sys. embedding D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EMBLLDN'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/EmbLLDND3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']

class WriteEmbLLIMD3PD (JobProperty):
    """Produce the Tau lepton-lepton nominal embedding D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EMBLLIM'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/EmbLLIMD3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']

class WriteEmbLLUPD3PD (JobProperty):
    """Produce the Tau lepton-lepton isolation up sys. embedding D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EMBLLUP'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/EmbLLUPD3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']


class WriteEmbLHDND3PD (JobProperty):
    """Produce the Tau lepton-hadron isolation down sys. embedding D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EMBLHDN'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/EmbLHDND3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']

class WriteEmbLHIMD3PD (JobProperty):
    """Produce the Tau lepton-hadron nominal embedding D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EMBLHIM'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/EmbLHIMD3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']

class WriteEmbLHUPD3PD (JobProperty):
    """Produce the Tau lepton-hadron isolation up sys. embedding D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EMBLHUP'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/EmbLHUPD3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']


class WriteEmbHHDND3PD (JobProperty):
    """Produce the Tau hadron-hadron isolation down sys. embedding D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EMBHHDN'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/EmbHHDND3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']

class WriteEmbHHIMD3PD (JobProperty):
    """Produce the Tau hadron-hadron nominal embedding D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EMBHHIM'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/EmbHHIMD3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']

class WriteEmbHHUPD3PD (JobProperty):
    """Produce the Tau hadron-hadron isolation up sys. embedding D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EMBHHUP'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TauD3PDMaker/EmbHHUPD3PD_prodJobOFragment.py"
    TreeNames = ['tau']
    SubSteps = ['a2d', 'e2d']
