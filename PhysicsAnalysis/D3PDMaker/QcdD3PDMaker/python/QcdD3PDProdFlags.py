# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: QcdD3PDProdFlags.py 513818 2012-08-15 16:10:43Z ssnyder $
#
# @file QcdD3PDMaker/python/QcdD3PDProdFlags.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2012
# @brief Define flags and configuration settings used when making 
#        Top D3PDs from production transforms.
#
# Any objects here with a name of the form Write*D3PD that derive
# from JobProperty are added to D3PDProdFlags.
#


from AthenaCommon.JobProperties import JobProperty


class WriteJetMetD3PD (JobProperty):
    """Produce the jet/met D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_JETMET'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "QcdD3PDMaker/JetMetD3PD_prodJobOFragment.py"
    TreeNames = ['qcd']
    SubSteps = ['a2d', 'e2d']


class WriteJetMetFullD3PD (JobProperty):
    """Produce the jet/met D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_JETMETFULL'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "QcdD3PDMaker/JetMetFullD3PD_prodJobOFragment.py"
    TreeNames = ['qcd']
    SubSteps = ['a2d', 'e2d']


class WriteJetMetWZD3PD (JobProperty):
    """Produce the jet/met D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_JETMETWZ'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "QcdD3PDMaker/JetMetWZD3PD_prodJobOFragment.py"
    TreeNames = ['qcd']
    SubSteps = ['a2d', 'e2d']


class WriteJetMetEMCLD3PD (JobProperty):
    """Produce the jet/met D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_JETMETEMCL'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "QcdD3PDMaker/JetMetEMCLD3PD_prodJobOFragment.py"
    TreeNames = ['qcd']
    SubSteps = ['a2d', 'e2d']


class WriteQcdD3PD (JobProperty):
    """Produce the physics D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_SMQCD'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "QcdD3PDMaker/QcdD3PD_prodJobOFragment.py"
    TreeNames = ['qcd']
    SubSteps = ['a2d', 'e2d']
