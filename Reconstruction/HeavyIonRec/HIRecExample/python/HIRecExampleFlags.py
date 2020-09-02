# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: HIRecExampleFlags.py
# @purpose: a container of flags for Heavy Ion Reconstruction
# @author: Andrzej Olszewski <Andrzej.Olszewski@ifj.edu.pl>

"""  A container of flags for heavy ion reconstruction  

"""
#
#
__author__  = 'Andrzej Olszewski'
__version__ = "$Revision: 1.7 $"
__doc__     = "A container of flags for Heavy Ion Reconstruction"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class doHIReReco(JobProperty):
    """ Run heavy ion global reconstruction 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doHIGlobal(JobProperty):
    """ Run heavy ion global reconstruction 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doHIJetRec(JobProperty):
    """ Run heavy ion jet reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doHIegamma(JobProperty):
    """ Run egamma reconstruction on subtracted cells
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doHIAODFix(JobProperty):
    """ Run AOD fix
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doHIJetAnalysis(JobProperty):
    """ Run heavy ion jet analysis
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doHIEventView(JobProperty):
    """ Run heavy ion event view
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doValidation(JobProperty):
    """ Run algorithms with validation histograms
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class withHijingEventPars(JobProperty):
    """ Indicates run on events with HijingEventPars info
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class ppMode(JobProperty):
    """ Indicates run on pp events with mostly pp default reco
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class ptCutOn(JobProperty):
    """ Turnes on pt cut tracking mod
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class ptCut(JobProperty):
    """ pt cut in tracking
    """
    statusOn     = True
    allowedTypes=['float']
    StoredValue=1.500

# Defines the container for heavy ion reco flags  
class HIRecExampleFlags(JobPropertyContainer):
    """ The global heavy ion reconstruction flag/job property container.
    """
    pass

# add the flags container to the top container 
jobproperties.add_Container(HIRecExampleFlags)


# We want always the following flags in the container  
list_jobproperties = [
    doHIReReco,
    doHIGlobal,
    doHIJetRec,
    doHIegamma,
    doHIAODFix,
    doHIJetAnalysis,
    doHIEventView,
    doValidation,
    withHijingEventPars,
    ppMode,
    ptCutOn,
    ptCut
    ]

for i in list_jobproperties:
    jobproperties.HIRecExampleFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties
