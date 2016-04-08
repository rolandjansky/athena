# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: PhysicsD3PDProdFlags.py 531461 2012-12-20 21:43:15Z nedwards $
#
# @file PhysicsD3PDMaker/python/PhysicsD3PDProdFlags.py
# @author scott snyder <snyder@bnl.gov>
# @date Jun, 2012
# @brief Define flags and configuration settings used when making 
#        Physics D3PDs from production transforms.
#
# Any objects here with a name of the form Write*D3PD that derive
# from JobProperty are added to D3PDProdFlags.
#


from AthenaCommon.JobProperties import JobProperty


class WriteSMWZD3PD (JobProperty):
    """Produce the SMWZ D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMWZ'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMWZD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMWZSOFTD3PD (JobProperty):
    """Produce the SMWZ soft D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMWZSOFT'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMWZsoftD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMWENUD3PD (JobProperty):
    """Produce the SMWENU D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMWENU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMWENUD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMWENUJJD3PD (JobProperty):
    """Produce the SMWENUJJ D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMWENUJJ'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMWENUJJD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMZEED3PD (JobProperty):
    """Produce the SMZEE D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMZEE'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMZEED3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMDYEED3PD (JobProperty):
    """Produce the SMDYEE D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMDYEE'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMDYEED3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMBKGED3PD (JobProperty):
    """Produce the SMBKGE D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMBKGE'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMBKGED3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMWMUNUD3PD (JobProperty):
    """Produce the SMWMUNU D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMWMUNU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMWMUNUD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMWMUNUJJD3PD (JobProperty):
    """Produce the SMWMUNUJJ D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMWMUNUJJ'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMWMUNUJJD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMZMUMUD3PD (JobProperty):
    """Produce the SMZMUMU D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMZMUMU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMZMUMUD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMDYMUMUD3PD (JobProperty):
    """Produce the SMDYMUMU D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMDYMUMU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMDYMUMUD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMBKGMUD3PD (JobProperty):
    """Produce the SMBKGMU D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMBKGMU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMBKGMUD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMDILEPD3PD (JobProperty):
    """Produce the SMDILEP D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMDILEP'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMDILEPD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMTRILEPD3PD (JobProperty):
    """Produce the SMTRILEP D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMTRILEP'
    FileName = ''
    isVirtual = False
    #DPDMakerScript = "SMEWTrilepSkim/SMTRILEPD3PD_prodJobOFragment.py"
    DPDMakerScript = "PhysicsD3PDMaker/SMTRILEPD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']


class WriteSMLIGHTD3PD (JobProperty):
    """Produce the SMLIGHT D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMLIGHT'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "PhysicsD3PDMaker/SMLIGHTD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
