# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Common signature flags  """

from AthenaCommon.JobProperties import JobProperty
from AthenaCommon.JobProperties import jobproperties # noqa: F401

__author__  = 'T. Bold'
__version__=""
__doc__="Common signature flags  "

    
class doL2ID(JobProperty):
    """ On/Off L2 InnerDetector trigger """
    statusOn=True
    allowedType=['bool']
    StoredValue=True
    
class doL2Calo(JobProperty):
    """ On/Off L2 Calorimetry trigger """
    statusOn=True
    allowedType=['bool']
    StoredValue=True
    
class doL2Muon(JobProperty):
    """ On/Off L2 Muons trigger """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

class doEFID(JobProperty):
    """ On/Off EventFilter Inner Detector trigger """
    statusOn=True
    allowedType=['bool']
    StoredValue=True
    
class doEFCalo(JobProperty):
    """ On/Off EventFilter Calorimetry trigger """
    statusOn=True
    allowedType=['bool']
    StoredValue=True
    
class doEFMuon(JobProperty):
    """ On/Off L2 Muons trigger """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

