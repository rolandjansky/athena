# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class MuonPtCalibNtupleMaker_Flags(JobPropertyContainer):
     """Muon pt calib ntuple maker flags"""  
jobproperties.add_Container(MuonPtCalibNtupleMaker_Flags)

class outputFilename(JobProperty):
     """ Specify the output file name""" 
     statusOn=True
     allowedTypes=['str']
     StoredValue='mcscale.root'
jobproperties.MuonPtCalibNtupleMaker_Flags.add_JobProperty(outputFilename)
