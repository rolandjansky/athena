# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file L1CaloD3PDMaker/python/L1CaloD3PDProdFlags.py
# @author scott snyder
# @date Oct, 2012
# @brief Define flags and configuration settings used when making 
#        L1Calo D3PDs from production transforms.

from AthenaCommon.JobProperties import JobProperty


class WriteL1CALOD3PD (JobProperty):
     """Produce the L1Calo D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_L1CALO'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "L1CaloD3PDMaker/L1CaloD3PD_prodJobOFragment.py"
     TreeNames = ['physics']
     SubSteps = ['e2d']


class WriteL1CALOPROBD3PD (JobProperty):
     """Produce the L1Calo Problem Channel D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_L1CALOPROB'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "L1CaloD3PDMaker/L1CaloProbChanD3PD_prodJobOFragment.py"
     TreeNames = ['L1CaloTree']
     SubSteps = ['e2d']

