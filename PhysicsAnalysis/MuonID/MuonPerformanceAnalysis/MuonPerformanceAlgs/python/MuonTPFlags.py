# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class MuonTPFlags(JobPropertyContainer):
     """Muon TP flags"""  
jobproperties.add_Container(MuonTPFlags)

class outputFilename(JobProperty):
     """ Specify the output file name""" 
     statusOn=True
     allowedTypes=['str']
     StoredValue='muontp.root'
jobproperties.MuonTPFlags.add_JobProperty(outputFilename)


class doPlots(JobProperty):
     """ toggle saving of detailed plots""" 
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False
jobproperties.MuonTPFlags.add_JobProperty(doPlots)


class doEff(JobProperty):
     """ toggle efficiency histrograms for the defined plots""" 
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False
jobproperties.MuonTPFlags.add_JobProperty(doEff)


class doClosure(JobProperty):
     """ toggle running of closure test within the analysis""" 
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False
jobproperties.MuonTPFlags.add_JobProperty(doClosure)


class IncludeMinimalCutTree(JobProperty):
     """ toggle writing a tree with minimal cuts (heavy on disk space)""" 
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False
jobproperties.MuonTPFlags.add_JobProperty(IncludeMinimalCutTree)

class GRL(JobProperty):
     """if non empty string, can pass a GRL to apply""" 
     statusOn=True
     allowedTypes=['str']
     StoredValue=''
jobproperties.MuonTPFlags.add_JobProperty(GRL)

