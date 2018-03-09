# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## purpose Python module to hold common flags to configure the InDetPhysValMonitoring 
##

""" InDetPhysValJobProperties
    Python module to hold common flags to configure InDetPhysValMonitoring JobOptions.

"""
__doc__    = "InDetPhysValJobProperties"
__all__    = [ "InDetPhysValJobProperties" ]

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
# import AthenaCommon.SystemOfUnits as Units


def isMC() :
    '''
    Test whether the input is monte carlo.
    @return true if this is simulation and should have truth information
    '''
    from AthenaCommon.GlobalFlags  import globalflags
    return  globalflags.DataSource() != 'data'


class InDetPhysValFlagsJobProperty(JobProperty):
    """ This class stores if a user changed a property once in the variable setByUser
    """
    setByUser = False

    def _do_action (self):
       self.setByUser = True

    def _undo_action (self):
       self.setByUser = True

    def get_Value(self):
       if (self.allowedTypes[0] == 'bool'):
          return self.statusOn and self.StoredValue
       else:
          return self.StoredValue

class doValidateDBMTracks(InDetPhysValFlagsJobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doValidateGSFTracks(InDetPhysValFlagsJobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doValidateLooseTracks(InDetPhysValFlagsJobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doValidateTightPrimaryTracks(InDetPhysValFlagsJobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doValidateTracksInJets(InDetPhysValFlagsJobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

from InDetRecExample.InDetJobProperties import Enabled

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class InDetPhysValJobProperties(JobPropertyContainer):
  """Container for the InDetPhysValMonitoring flags
  """

  def __init__(self, context=''):
    #allow specifying context (used by the trigger)
    JobPropertyContainer.__init__(self,context)

  def checkThenSet (self, jp, value):
    # checks if a variable has been changed by the user before
    if not jp.setByUser:
      jp.set_Value (value)

  def setupDefaults(self):
      pass

  def init(self):
    #Method to do the final setup of the flags according to user input before.
    #This method MUST ONLY BE CALLED once in InDetRecExample/InDetRec_jobOptions.py!!
    if not self.Enabled:
      print 'InDetPhysValFlags.init(): ID PhysValFlags are disabled. Locking container and not doing anything else.'
    else:
      self.setup()

    # do this also if Enabled == False
    print "Initialization of InDetFlags finished - locking container!"
    self.lock_JobProperties()

  def setup(self) :
    print 'Initializing InDetJobPhysValProperties with InDetFlags.'
    # THIS METHOD MUST BE THE FIRST TO BE CALLED. DO NOT MOVE IT OR ADD THINGS IN FRONT
    self.setupDefaults()

    # for backward compatibility check whether DBM has been added already
    from  InDetRecExample.InDetJobProperties import InDetFlags
    if  hasattr(InDetFlags,'doDBM') and not InDetFlags.doDBM() :
        self.checkThenSet(self.doValidateDBMTracks,  False)
    print self
    
  def printInfo(self) :
      pass

jobproperties.add_Container(InDetPhysValJobProperties)

##-----------------------------------------------------------------------------
## 4th step
## adding ID flags to the InDetJobProperties container
_list_InDetPhysValJobProperties = [
    Enabled,
    doValidateDBMTracks,
    doValidateGSFTracks,    
    doValidateLooseTracks,
    doValidateTightPrimaryTracks,
    doValidateTracksInJets
]

for j in _list_InDetPhysValJobProperties:
    jobproperties.InDetPhysValJobProperties.add_JobProperty(j)

InDetPhysValFlags = jobproperties.InDetPhysValJobProperties
print 'DEBUG InDetPhysValJobProberties'
print InDetPhysValFlags
