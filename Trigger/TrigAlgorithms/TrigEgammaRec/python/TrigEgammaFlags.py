#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright 2016 Ryan Mackenzie White <ryan.white@cern.ch>
#
# Distributed under terms of the  license.

"""
Job properties for TrigEgamma
Allows configuring PID, calibration, cluster corrections, etc...
"""
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class ringerVersion (JobProperty):
    """ Version ringer tunes """
    statusOn=False
    allowedTypes=['str','None']
    StoreValues=None
    
class pidVersion (JobProperty):
    """ Version of PID tunes
    """
    statusOn=True
    allowedTypes=['str','None']
    StoredValue='ElectronPhotonSelectorTools/trigger/rel21_20161021/'

class clusterCorrectionVersion (JobProperty):
    """
    Cluster correction version for HLT Calo
    """
    statusOn=True
    allowedTypes=['str','None']
    allowedValues=['v12phiflip_noecorrnogap']
    StoredValue=None

class calibMVAVersion (JobProperty):
    """
    MVA calibration version
    """
    statusOn=True
    allowedTypes=['str','None']
    StoredValue='egammaMVACalib/online/v3'

class TrigEgammaFlags(JobPropertyContainer):
    """TrigEgamma information"""

jobproperties.add_Container(TrigEgammaFlags)

jobproperties.TrigEgammaFlags.add_JobProperty(pidVersion)
jobproperties.TrigEgammaFlags.add_JobProperty(clusterCorrectionVersion)
jobproperties.TrigEgammaFlags.add_JobProperty(calibMVAVersion)
