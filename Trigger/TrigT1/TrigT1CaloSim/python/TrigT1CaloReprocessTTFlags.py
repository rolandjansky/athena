# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer

# get logger for this file
from AthenaCommon.Logging import logging

_flags = list() 

class NoiseCutsDB(JobProperty):
    """
    Path to TriggerL1CaloV2CalibrationPhysicsPprChanCalib-00-00
    See ATR-16373 to find the available settings 
    """
    statusOn = True
    allowedType = ['str']
    StoredValue = "V2-PHYSICS-CHANCALIB-00-00"
_flags.append(NoiseCutsDB)

class ReprocessTT(JobPropertyContainer):
    """ ReprocessTT class"""
    pass 

from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.add_Container( ReprocessTT )

ReprocessTTFlags = tf.ReprocessTT 

for flag in _flags: 
    ReprocessTTFlags.add_JobProperty(flag) 

del _flags 
