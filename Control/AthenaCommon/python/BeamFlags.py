# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   RecExConfig/python/BeamFlags.py
#=======================================================================
""" Flags for beam structure
"""
#
#
__author__  = 'D. Rousseau'
__version__="$Revision: 1.3 $"
__doc__="Beam structure flags . "




#=======================================================================
# imports
#=======================================================================
from JobProperties import JobProperty, JobPropertyContainer
from JobProperties import jobproperties

class override(JobProperty):
    """ True wether beam info has to be taken from python rather than filemetadata
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True



class bunchSpacing(JobProperty):
    """ Bunch spacing in ns
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=25  
#
#
class numberOfCollisions(JobProperty):
    """ Number of collisions per beam crossing
    Should be 2.3*(L/10**33)*(bunchSpacing/25 ns)
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.
#
# 
class beamType(JobProperty):
    """ Specify data taking type : with colliding beams (default),
    single beam or cosmics 
    """ 
    statusOn=True
    allowedTypes=['str']
    allowedValues=['collisions','singlebeam','cosmics']
    StoredValue='collisions'
#
# 
class energy (JobProperty):
    """ Specify beam energy (MeV)
    """ 
    from AthenaCommon.SystemOfUnits import TeV
    statusOn=True
    allowedTypes=['float']
    StoredValue=7*TeV



# 
#
class Beam(JobPropertyContainer):
    """ Beam information """
    # computes on the fly the expected luminosity
    def estimatedLuminosity(self):
        return 1E33*(jobproperties.Beam.numberOfCollisions()/2.3)*(25./jobproperties.Beam.bunchSpacing())
    # true if luminosity is zero
    def zeroLuminosity(self):
        if jobproperties.Beam.numberOfCollisions()>0:
            return False
        else:
            return True
        
   

# add the beam flags container to the top container 
jobproperties.add_Container(Beam)

# I want always the following flags in the Beam container  
_list_Beam=[override,bunchSpacing,numberOfCollisions,beamType,energy]
for j in _list_Beam: 
    jobproperties.Beam.add_JobProperty(j)
del _list_Beam

# For convenience, a shorthand alias to the beam flags
beamFlags = jobproperties.Beam

#=======================================================================
