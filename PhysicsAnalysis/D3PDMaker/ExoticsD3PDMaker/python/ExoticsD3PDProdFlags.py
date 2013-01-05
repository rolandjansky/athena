# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ExoticsD3PDProdFlags.py 496598 2012-04-19 08:58:42Z mkaneda $
#
# @file ExoticsD3PDMaker/python/ExoticsD3PDProdFlags.py
# @author Michiru Kaneda<Michiru.Kaneda@cern.ch>
# @date Apr, 2012
# @brief Define flags and configuration settings used when making 
#        Exotics D3PDs from production transforms.

from AthenaCommon.JobProperties import JobProperty

class WriteZPrimeEED3PD (JobProperty):
    """Produce the ZPrimeEE D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_ZPRIMEEE'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "ExoticsD3PDMaker/ZPrimeEED3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']

class WriteZPrimeMMD3PD (JobProperty):
    """Produce the ZPrimeMM D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_ZPRIMEMM'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "ExoticsD3PDMaker/ZPrimeMMD3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']

class WriteWPrimeEND3PD (JobProperty):
    """Produce the WPrimeEN D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_WPRIMEEN'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "ExoticsD3PDMaker/WPrimeEND3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']

class WriteWPrimeMND3PD (JobProperty):
    """Produce the WPrimeMN D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_WPRIMEMN'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "ExoticsD3PDMaker/WPrimeMND3PD_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['a2d', 'e2d']
