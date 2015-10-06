# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file InDetTrigRecExample/python/InDetTrigFlags.py
## @purpose link InDetTrigFlags to InDetTRigJobProperties container
##          automate adding of certain InDetJobProperties

""" InDetTrigFlags
    The entry point to the EFID configuration properties
"""

__author__ = "Jiri Masik"
__version__= "$Revision: 1.8 $"
__doc__    = "InDetTrigFlags"

__all__    = [ "InDetTrigFlags" ]

from AthenaCommon.JobProperties import jobproperties, JobProperty
#from InDetRecExample.InDetJobProperties import InDetJobProperties, InDetFlags, InDetFlagsJobProperty
#from InDetTrigRecExample.InDetTrigJobProperties import *

from InDetTrigRecExample.InDetTrigJobProperties import *
#
#class InDetTrigJobProperties(InDetJobProperties):
#  """Container for EF ID job properties"""
#jobproperties.add_Container(InDetTrigJobProperties)

#list all jobProperty classes defined in InDetRecExample.InDetJobProperties
# and add them to our container if they are also in InDetFlags
# (there seems to be no list of jobProperties)

import pyclbr
moddict = pyclbr.readmodule('InDetTrigRecExample.InDetTrigJobProperties')
moddict2= pyclbr.readmodule('InDetRecExample.InDetJobProperties')
objs = moddict.values() + moddict2.values()

if moddict.has_key('InDetTrigFlagsJobProperty'):
  classid = moddict['InDetTrigFlagsJobProperty']
else:
  classid = 'InDetTrigFlagsJobProperty'


# #check if the object is also in InDetFlags
# from InDetRecExample.InDetJobProperties import InDetFlags
# for obj in objs:
#   if isinstance(obj, pyclbr.Class):
#     if classid in obj.super or 'JobProperty' in obj.super:
#       if obj.name in InDetFlags.__dict__:
#         try:
#           exec 'from InDetTrigRecExample.InDetTrigJobProperties import %s' % obj.name
#           exec 'jobproperties.InDetTrigJobProperties.add_JobProperty(%s)' % obj.name
#         except:
#           print 'Jobproperty %s missing in the EFID config' % obj.name
       

#

from InDetRecExample.InDetJobProperties import _list_InDetJobProperties
for j in _list_InDetJobProperties: 
    jobproperties.InDetTrigJobProperties.add_JobProperty(j)

from InDetTrigRecExample.InDetTrigJobProperties import \
    doMCBytestream, doDCSAccess, useConditionsClasses, \
    doOverlapSP,doCommissioning,doSCTIntimeHits,TRTOff
jobproperties.InDetTrigJobProperties.add_JobProperty(doMCBytestream)
jobproperties.InDetTrigJobProperties.add_JobProperty(doDCSAccess)
jobproperties.InDetTrigJobProperties.add_JobProperty(useConditionsClasses)
jobproperties.InDetTrigJobProperties.add_JobProperty(doOverlapSP)
jobproperties.InDetTrigJobProperties.add_JobProperty(doCommissioning)
jobproperties.InDetTrigJobProperties.add_JobProperty(doSCTIntimeHits)   #temporary
jobproperties.InDetTrigJobProperties.add_JobProperty(TRTOff)

#define shortcut
InDetTrigFlags = jobproperties.InDetTrigJobProperties
