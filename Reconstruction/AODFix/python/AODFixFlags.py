# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   AODFix/python/AODFixFlags.py
#=======================================================================
""" Flags used to steer AODFix.
"""
#
#
__author__  = 'R.Seuster'
__version__="$Revision: 1.00 $"
__doc__="AODFix specific flags . "

# from AthenaCommon.Constants import *

#=======================================================================
# other imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class AODFixVersion(JobProperty):
    """ AODFix version number, none if not available
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="n/a"
#
class AODFixPreviousMinorVersion(JobProperty):
    """ AODFix minor version number of previously applied AODFix, 0 if not available
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
#
class doPostAtlfast(JobProperty):
    """ If false switch off AODFix for Atlfast
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostTrigger(JobProperty):
    """ If false switch off AODFix for 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostSystemRec(JobProperty):
    """ If false switch off AODFix for SystemRec (careful !!)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostCombinedRec(JobProperty):
    """ If false switch off AODFix for CombinedRec (careful !!)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostHeavyIon(JobProperty):
    """ If false switch off AODFix for heavy ions
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostCaloRec(JobProperty):
    """ If false switch off AODFix for calo
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostInDetRec(JobProperty):
    """ If false switch off AODFix for indet
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostMuonRec(JobProperty):
    """ If false switch off AODFix for muons
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostForwardRec(JobProperty):
    """ If false switch off AODFix for forward dets
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostEgammaRec(JobProperty):
    """ If false switch off AODFix for egamma
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostMuonCombinedRec(JobProperty):
    """ If false switch off AODFix for combined muons
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostJetRec(JobProperty):
    """ If false switch off AODFix for jets
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPosttauRec(JobProperty):
    """ If false switch off AODFix for taus
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doPostMissingETRec(JobProperty):
    """ If false switch off AODFix for MET
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


# Defines the container for the reconstruction flags  
class AODFix(JobPropertyContainer):
    """ The AODFix flag/job property  container.
    """

# add the reconstruction flags container to the top container 
jobproperties.add_Container(AODFix)

# I want always the following flags in the Rec container  
_list_AODFix=[ AODFixVersion, AODFixPreviousMinorVersion, doPostAtlfast, doPostTrigger, doPostSystemRec, doPostCombinedRec, 
doPostHeavyIon, doPostCaloRec, doPostInDetRec, doPostMuonRec, doPostForwardRec, 
doPostEgammaRec, doPostMuonCombinedRec, doPostJetRec, doPosttauRec, doPostMissingETRec ]
for j in _list_AODFix: 
    jobproperties.AODFix.add_JobProperty(j)
del _list_AODFix

# The short name (carefull do not select same name as shot name as well. 
# otherwise problems with pickle)  
aodfix=jobproperties.AODFix

#=======================================================================

