# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file HSG5DPDUtils/python/HSG5D3PDProdFlags.py
# @author Robert Harrington <roberth@cern.ch>
# @date March 16, 2012
# @brief Define flags and configuration settings used when making 
#        HSG5 D3PDs from production transforms.
#
# Any objects here with a name of the form Write*D3PD that derive
# from JobProperty are added to D3PDProdFlags.
#

from AthenaCommon.JobProperties import JobProperty

allowedD3PDList = []

# WH D3PD
class WriteHSG5WHD3PD (JobProperty):
    """Produce the HSG5 BOOSTED WH D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_HSG5WH'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG5DPDUtils/HSG5_WHlnubb_boosted_D3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5WHFile')

class WriteHSG5WHUD3PD (JobProperty):
    """Produce the HSG5 UNBOOSTED WH D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_HSG5WHU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG5DPDUtils/HSG5_WHlnubb_unboosted_D3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5WHUFile')

class WriteHSG5WHQD3PD (JobProperty):
    """Produce the HSG5 UNBOOSTED WH QCD D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_HSG5WHQ'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG5DPDUtils/HSG5_WHlnubb_unboosted_QCDbkg_D3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5WHQFile')

# ZHLL D3PD
class WriteHSG5ZHLLD3PD (JobProperty):
    """Produce the HSG5 BOOSTED ZH->llbb D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_HSG5ZHLL'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG5DPDUtils/HSG5_ZHllbb_boosted_D3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5ZHLLFile')

class WriteHSG5ZHLLUD3PD (JobProperty):
    """Produce the HSG5 UNBOOSTED ZH->llbb D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_HSG5ZHLLU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG5DPDUtils/HSG5_ZHllbb_unboosted_D3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5ZHLLUFile')

# ZHMET D3PD
class WriteHSG5ZHMETD3PD (JobProperty):
    """Produce the HSG5 BOOSTED ZH->nunubb D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_HSG5ZHMET'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG5DPDUtils/HSG5_ZHnunubb_boosted_D3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5ZHMETFile')

class WriteHSG5ZHMETUD3PD (JobProperty):
    """Produce the HSG5 UNBOOSTED ZH->nunubb D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_HSG5ZHMETU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "HSG5DPDUtils/HSG5_ZHnunubb_unboosted_D3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5ZHMETUFile')

# GAMH D3PD (should no longer be used)
class WriteHSG5GAMHD3PD (JobProperty):
     """Produce the HSG5 boosted gamma-H D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_HSG5GAMH'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "HSG5DPDUtils/HSG5_gammaHbb_boosted_D3PD_prodJobOFragment.py"
     TreeNames = ['physics']
     SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5GAMHFile')

# GAMZ D3PD
class WriteHSG5GAMZD3PD (JobProperty):
    """Produce the HSG5 boosted gamma-Z D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_HSG5GAMZ'
    FileName = ''
    isVirtual = False
    DPDMakerScript =  "HSG5DPDUtils/HSG5_gammaZbb_boosted_D3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5GAMZFile')

# ZBB D3PD
class WriteHSG5ZBBD3PD (JobProperty):
     """Produce the HSG5 boosted Z->bb D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_HSG5ZBB'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "HSG5DPDUtils/HSG5_Zbb_boosted_D3PD_prodJobOFragment.py"
     TreeNames = ['physics']
     SubSteps = ['a2d', 'e2d']
allowedD3PDList.append('outputNTUP_HSG5ZBBFile')        
