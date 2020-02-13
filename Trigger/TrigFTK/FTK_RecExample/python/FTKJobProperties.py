# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##
## @file FTK_RecExample/python/FTKJobProperties.py
## @purpose Allow configuration of FTK settings via JobProperties

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

class Enabled(JobProperty):
  statusOn=True
  allowedTypes=['bool']
  StoredValue=True
  
  def set_On_NoDoAction(self, obj):
    """ Sets statusOn equals to True w/o _do_action.
    """
    if not(obj._locked):
      obj.__dict__['statusOn']=True
    else:
      obj._log.info('The JobProperty %s is blocked' % obj.__name__)
 
  def set_Off_NoDoAction(self, obj):
    """ Sets statusOn equals to False w/o _undo_action.
    """
    if not(obj._locked):
      obj.__dict__['statusOn']=False
    else:
      obj._log.info('The JobProperty %s is blocked' % obj.__name__)
 
  def _do_action(self):
    for obj in self._the_same_context_objects():
      self.set_On_NoDoAction(obj)

  def _undo_action(self):
    for obj in self._the_same_context_objects():
      self.set_Off_NoDoAction(obj)
 
  def _the_same_context_objects(self):
    context = self._context_name.partition('.'+self.__name__)
    objects = list()

    for i in self.__class__._nInstancesContextDict.keys():
      oname = self.__class__._nInstancesContextDict[i].__name__
      ocontext = self.__class__._nInstancesContextDict[i]._context_name.partition('.'+oname)
      if context !=  ocontext:
        #print ocontext
        if context[0] == ocontext[0]:
          objects.append(self.__class__._nInstancesContextDict[i])
    return objects

class FTKFlagsJobProperty(JobProperty):
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

class doPrintConfigurables(FTKFlagsJobProperty):
  """if this is on the all the print lines are activated"""
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = True

class doSmearing(FTKFlagsJobProperty):
  """ Simulate FTK tracks by smearing HLT tracks """
  statusOn=True
  allowedTypes=['bool']
  StoredValue=False

class doTruthLinks(FTKFlagsJobProperty):
  """ Create Duplicate xAOD collections with Truth Links """
  statusOn=True
  allowedTypes=['bool']
  StoredValue=False

class doDetailedTruth(FTKFlagsJobProperty):
  """ Add Additional parameters to TruthParticles """
  statusOn=True
  allowedTypes=['bool']
  StoredValue=False

class doTruthSmearing(FTKFlagsJobProperty):
  """ Simulate FTK tracks by smearing truth particles """
  statusOn=True
  allowedTypes=['bool']
  StoredValue=False

 
class FTKJobProperties(JobPropertyContainer):
  """Container for the FTK flags
  """

  def __init__(self, context=''):
     #allow specifying context (used by the trigger) 
     JobPropertyContainer.__init__(self,context)
   
  def checkThenSet (self, jp, value):
    # checks if a variable has been changed by the user before
    if not jp.setByUser:
      jp.set_Value (value)
       
  def setupDefaults(self):
    self.checkThenSet(self.doSmearing   , False)
    self.checkThenSet(self.doTruthSmearing, False)


  def init(self):
    #Method to do the final setup of the flags according to user input before.
    #This method MUST ONLY BE CALLED once in FTK_RecExample/FTKRec_jobOptions.py!!
    if not self.Enabled:
      print ('FTKFlags.init(): FTK flags are disabled. Locking container and not doing anything else.')
    else:
      print ('Initializing FTKJobProperties with DetFlags, GlobalFlags and other high level flags.')
      # THIS METHOD MUST BE THE FIRST TO BE CALLED. DO NOT MOVE IT OR ADD THINGS IN FRONT
      self.setupDefaults()
      
  def printInfo(self) :
    print ('****** FTK Flags ********************************************************')
    if self.doSmearing() :
      print ('*')
      print ('* --------------------> FTK tracks will be simulated by smearing HLT tracks')
      print ('*')
    if self.doTruthSmearing() :
      print ('*')
      print ('* --------------------> FTK tracks will be simulated by smearing truth particles')
      print ('*')

     
## adding the container to the general top-level container
jobproperties.add_Container(FTKJobProperties)

## adding FTK flags to the FTKJobProperties container
_list_FTKJobProperties = [Enabled,
                          doPrintConfigurables,
                          doSmearing,
                          doTruthLinks,
                          doDetailedTruth,
                          doTruthSmearing,
                          ]

for j in _list_FTKJobProperties: 
  jobproperties.FTKJobProperties.add_JobProperty(j)

#define shortcut
## Note: you still have to import it:
## >>> from FTK_RecExample.FTKJobProperties import FTKFlags
FTKFlags = jobproperties.FTKJobProperties
