# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class doNTuple(JobProperty):
    """ Create private NTuple
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class doMSRefit(JobProperty):
    """ 
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True    

class doLHR(JobProperty):
    """ 
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class PtLowerLimit(JobProperty): 
    """  
    """ 
    statusOn = True 
    allowedTypes = ['float'] 
    StoredValue = 0. # it was 3000
 
class momentumLowerLimit(JobProperty): 
    """  
    """ 
    statusOn = True 
    allowedTypes = ['float'] 
    StoredValue = 0. # it was 3000
	 
class nIDhits(JobProperty): 
    """  
    """ 
    statusOn = True 
    allowedTypes = ['int'] 
    StoredValue = 5 

class MuGirlFlags(JobPropertyContainer):
    """ MuGirl Job Properties
    """
    pass

jobproperties.add_Container(MuGirlFlags)
jobproperties.MuGirlFlags.add_JobProperty(doNTuple)
jobproperties.MuGirlFlags.add_JobProperty(doMSRefit)
jobproperties.MuGirlFlags.add_JobProperty(doLHR)
jobproperties.MuGirlFlags.add_JobProperty(PtLowerLimit) 
jobproperties.MuGirlFlags.add_JobProperty(momentumLowerLimit) 
jobproperties.MuGirlFlags.add_JobProperty(nIDhits) 

muGirlFlags = jobproperties.MuGirlFlags
#muGirlFlags.print_JobProperties('tree&value')
