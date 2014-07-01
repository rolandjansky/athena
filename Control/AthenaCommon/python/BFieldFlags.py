# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   AthenaCommon/python/BeamFlags.py
#=======================================================================
""" Flags for beam structure
"""
#
#
__author__  = 'D. Rousseau'
__version__="$Revision: 1.2 $"
__doc__="magnetic field flags "




#=======================================================================
# imports
#=======================================================================
from JobProperties import JobProperty, JobPropertyContainer
from JobProperties import jobproperties

class override(JobProperty):
    """ True wether magnetic filed is taken from these flags rather than cond db (when available)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class solenoidOn (JobProperty):
    """ True wether solenoid is on
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class barrelToroidOn (JobProperty):
    """ True wether barrel toroid is on
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class endcapToroidOn (JobProperty):
    """ True wether endcap toroid are on
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True





 
# Defines a sub-container for the algorithm switches
class BField(JobPropertyContainer):
    """ Beam information """
    # computes on the fly the expected luminosity
    def allToroidOn(self):
        return jobproperties.BField.barrelToroidOn() and  jobproperties.BField.endcapToroidOn()
        
# add the beam flags container to the top container 
jobproperties.add_Container(BField)

# I want always the following flags in the BField container  
for j in [override,solenoidOn,barrelToroidOn,endcapToroidOn]: 
    jobproperties.BField.add_JobProperty(j)



#=======================================================================

