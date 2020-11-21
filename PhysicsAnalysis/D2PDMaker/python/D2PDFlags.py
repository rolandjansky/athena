# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        D2PDFlags
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2009
##
## Description: Here, all neccessary job flags for the D2PDMaker
##              are defined.
##
##=============================================================================

__doc__ = """Here, all neccessary job flags for the D2PDMaker are defined."""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the D2PDMaker flags  
#=====================================================================
class D2PDFlags(JobPropertyContainer):
    """ The D2PDMaker flag/job property  container."""
jobproperties.add_Container(D2PDFlags)

#short-cut to get the D2PDFlags container with this one line:
#'from D2PDMaker.D2PDFlags import D2PDFlags'
#Note that name has to be different to avoid problems with pickle
D2PDFlags = jobproperties.D2PDFlags

#=====================================================================
# Now define each flag and add it to the container
#=====================================================================

#------------------------
# Input/output
#------------------------
class OutputDirectoryName(JobProperty):
    """ Name of the directory where the output files are written. """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.D2PDFlags.add_JobProperty(OutputDirectoryName)

class OutputMiddleName(JobProperty):
    """ Keyword added in the middle of the output files names. """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.D2PDFlags.add_JobProperty(OutputMiddleName)

class OutputPoolRootFileNamePrefix(JobProperty):
    """ File name prefix for the pool.root output files."""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.D2PDFlags.add_JobProperty(OutputPoolRootFileNamePrefix)


##--------------------------------
## Flags to turn on/off each possible DPD
## Note: each DPD has its own internal flags.
##--------------------------------
listAODtoD2PD=[]
listESDtoD2PD=[]
