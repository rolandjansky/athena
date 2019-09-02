# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file PrimaryDPDMaker/python/DxAODPixLumiJobProperties.py
## @brief Python module to hold common flags to configure xAOD-making jobOptions
##

""" DxAODPixLumiJobProperties
    Python module to hold common flags to configure ID xAOD-making jobOptions.

"""


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class DxAODPixLumiJobPropertyContainer(JobPropertyContainer):
    """ The jobProperty container for DxAODPixLumi-making flags """
jobproperties.add_Container(DxAODPixLumiJobPropertyContainer)

class DumpPixelInfo(JobProperty):
    """write out pixel info """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
jobproperties.DxAODPixLumiJobPropertyContainer.add_JobProperty(DumpPixelInfo)

class ModulesSelection(JobProperty):
    """Z event selection based on DRAW ZMUMU"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "(PixelClusters.layer == 0) && ((PixelClusters.eta_module >= -10 && PixelClusters.eta_module <= -6) || (PixelClusters.eta_module >= 5 && PixelClusters.eta_module <= 9)) && (((PixelClusters.eta_module <= -7||PixelClusters.eta_module >= 6) && (PixelClusters.eta_pixel_index > 0 && (PixelClusters.eta_pixel_index + PixelClusters.sizeZ-1) < 79)) || ((PixelClusters.eta_module > -7 && PixelClusters.eta_module < 6) && (PixelClusters.eta_pixel_index > 0 && (PixelClusters.eta_pixel_index + PixelClusters.sizeZ - 1) < 159))) && (PixelClusters.phi_pixel_index > 0 && (PixelClusters.phi_pixel_index + PixelClusters.sizePhi-1) < 335)"
jobproperties.DxAODPixLumiJobPropertyContainer.add_JobProperty(ModulesSelection)

DxAODPixLumiFlags = jobproperties.DxAODPixLumiJobPropertyContainer
