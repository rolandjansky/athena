# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDMaker/python/egammaD3PDProdFlags.py
# @author scott snyder
# @date Oct, 2012
# @brief Define flags and configuration settings used when making 
#        egamma D3PDs from production transforms.

from AthenaCommon.JobProperties import JobProperty


# For historical reasons, the configuration for this is in D3PDMakerConfig.
class WriteEgammaD3PD (JobProperty):
    """Produce the egamma D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_EGAMMA'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "D3PDMakerConfig/EgammaD3PD_prodJobOFragment.py"
    TreeNames = ['egamma', 'egammaTrigDec']
    SubSteps = ['a2d', 'e2d']


class WriteClusterCorrectionD3PD (JobProperty):
    """Produce the  D3PD for cluster correction studies."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_CLUSTERCORRECTION'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "egammaD3PDMaker/ClusterCorrectionD3PD_prodJobOFragment.py"
    TreeNames = ['clusters']
    SubSteps = ['a2d', 'e2d']


