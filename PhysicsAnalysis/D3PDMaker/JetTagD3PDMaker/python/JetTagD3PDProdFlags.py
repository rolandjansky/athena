# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: JetTagD3PDProdFlags.py 512470 2012-08-05 15:31:54Z aad $
#
# @file JetTagD3PDMaker/python/JetTagD3PDProdFlags.py
# @author scott snyder <snyder@bnl.gov>
# @date May, 2012
# @brief Define flags and configuration settings used when making 
#        Jet Tag D3PDs from production transforms.
#
# Any objects here with a name of the form Write*D3PD that derive
# from JobProperty are added to D3PDProdFlags.
#

from AthenaCommon.JobProperties import JobProperty
from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys


class WriteBTAGD3PD (JobProperty):
     """Produce the BTAG D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_BTAGD3PD'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "JetTagD3PDMaker/JetTagD3PD_prodJobOFragment.py"
     TreeNames = [JetTagD3PDKeys.D3PDTupleName()]
     SubSteps = ['a2d', 'e2d']


class WriteBTAGSLIMD3PD (JobProperty):
     """Produce the BTAG D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_BTAGSLIM'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "JetTagD3PDMaker/JetTagD3PDSLIM_prodJobOFragment.py"
     TreeNames = [JetTagD3PDKeys.D3PDTupleName()]
     SubSteps = ['a2d', 'e2d']


class WriteBTAGFULLD3PD (JobProperty):
     """Produce the BTAG D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_BTAGFULL'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "JetTagD3PDMaker/JetTagD3PDFULL_prodJobOFragment.py"
     TreeNames = [JetTagD3PDKeys.D3PDTupleName()]
     SubSteps = ['a2d', 'e2d']


class WriteBTAGEFFD3PD (JobProperty):
     """Produce the BTAG D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_BTAGEFF'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "JetTagD3PDMaker/JetTagD3PDEFF_prodJobOFragment.py"
     TreeNames = [JetTagD3PDKeys.D3PDTupleName()]
     SubSteps = ['a2d', 'e2d']

class WriteBTAGGHOSTD3PD (JobProperty):
     """Produce the BTAG D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_BTAGGHOST'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "JetTagD3PDMaker/JetTagD3PDGHOST_prodJobOFragment.py"
     TreeNames = [JetTagD3PDKeys.D3PDTupleName()]
     SubSteps = ['a2d', 'e2d']
