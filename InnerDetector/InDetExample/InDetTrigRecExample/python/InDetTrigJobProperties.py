# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

##
## @file InDetTrigRecExample/python/InDetTrigJobProperties.py
## @brief Python module with InDetTrigJobProperties to hold
##          an independent configuration of EFID
##          some classes need to be duplicated from InDetJobProperties 

""" InDetTrigJobProperties
    Python module with a copy of InDetJobProperties to hold
         an independent configuration of EFID
    the module defines the same set of classes as InDetJobProperties
    interim solution -  to be replaced without much code duplication

    1) import of classes can be automated
    2) list of properties can be automated
    3) inheriting from InDetTrigFlagsProperty may be superfluous
    
"""

__author__ = "J. Masik"
__version__= "$Revision: 1.15 $"
__doc__    = "InDetTrigJobProperties"

__all__    = [ "InDetTrigJobProperties" ]

# stolen from InDetJobProperties which was stolen from
#     AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 0th step: define infrastructure JPs DO NOT MODIFY THEM!!!

from InDetRecExample.InDetJobProperties import InDetFlagsJobProperty

## this is the culprit why we cannot just inherit and copy properties
class InDetTrigFlagsJobProperty(JobProperty):
  """ special flag class ANDing with jobproperies.CaloRecFlags.Enabled
  """
  def get_Value(self):
    return self.statusOn and self.StoredValue and jobproperties.InDetTrigJobProperties.Enabled()

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes


#
##----------------------------------------------------------------------------
#
class doMCBytestream(InDetFlagsJobProperty):
  """ Select cabling maps appropriate for MC/Data  """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = True
  
class doDCSAccess(InDetFlagsJobProperty):
  """ Control access to the DCS    """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False


class useConditionsClasses(InDetFlagsJobProperty):
  """ conditions set up by conditions objects """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = True

class doOverlapSP(InDetFlagsJobProperty):
  """ Control access to the DCS    """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False

class doCommissioning(InDetFlagsJobProperty):
  """ Enable/disable commissioning cuts """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = True

class doSCTIntimeHits(InDetFlagsJobProperty):
    """ defines if the X1X mode is used for the offline or not """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class TRTOff(InDetFlagsJobProperty):
  """ Disable TRT (for upgrade studies) """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False

        
##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container

from InDetRecExample.InDetJobProperties import InDetJobProperties

class InDetTrigJobProperties(InDetJobProperties):
  """Container for the InDetTrig flags
  """
  def __init__(self, context='EFID'):
    InDetJobProperties.__init__(self,context)

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(InDetTrigJobProperties)


##-----------------------------------------------------------------------------
## 4th step
## adding ID flags to the InDetJobProperties container
#done in InDetTrigFlags

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select InDetJobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags = jobproperties.InDetJobProperties
