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
    """Selects IBL 3D modules, rejecting clusters going over the sensor edge"""
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "(xAOD_PixelClusters.layer == 0) && ((xAOD_PixelClusters.eta_module >= -10 && xAOD_PixelClusters.eta_module <= -6) || (xAOD_PixelClusters.eta_module >= 5 && xAOD_PixelClusters.eta_module <= 9)) && (((xAOD_PixelClusters.eta_module <= -7||xAOD_PixelClusters.eta_module >= 6) && (xAOD_PixelClusters.eta_pixel_index > 0 && (xAOD_PixelClusters.eta_pixel_index + xAOD_PixelClusters.sizeZ-1) < 79)) || ((xAOD_PixelClusters.eta_module > -7 && xAOD_PixelClusters.eta_module < 6) && (xAOD_PixelClusters.eta_pixel_index > 0 && (xAOD_PixelClusters.eta_pixel_index + xAOD_PixelClusters.sizeZ - 1) < 159))) && (xAOD_PixelClusters.phi_pixel_index > 0 && (xAOD_PixelClusters.phi_pixel_index + xAOD_PixelClusters.sizePhi-1) < 335)"
jobproperties.DxAODPixLumiJobPropertyContainer.add_JobProperty(ModulesSelection)

DxAODPixLumiFlags = jobproperties.DxAODPixLumiJobPropertyContainer
