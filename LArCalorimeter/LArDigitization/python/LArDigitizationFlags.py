# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# @file: LArDigitizationFlags.py
# @purpose: a container of flags for LArDigitization making configuring
# @author: G.Unal

"""  A container of flags for LArDigitization making configuring

"""
#
#
__author__  = 'Guillaume Unal'
__version__ = "$Revision: 1.11 $"
__doc__     = "A container of flags for LArDigitization making configuring"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#
class useFcalHighGain(JobProperty):
    """ Flag to decide if using high gain for Fcal or not
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False

#
class useEmecIwHighGain(JobProperty):
    """ Flag to decide if using high gain for EMEC IW or not
    """  
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

                           
# Defines the container for the cell making flags
class LArDigitizationFlags(JobPropertyContainer):
    """ The LArDigitization making flag property container
    """
    pass



# add the perfmon flags container to the top container 
jobproperties.add_Container(LArDigitizationFlags)


# We want always the following flags in the container  
list_jobproperties = [
    useFcalHighGain,useEmecIwHighGain
    ]

for i in list_jobproperties:
    jobproperties.LArDigitizationFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties
