# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

######################################################################
## @file   TrigPyHelper.py
## @brief  Helpers for C++ Python bindings
## @author Frank Winklmeier
######################################################################

from GaudiPython import *    # noqa
from GaudiPython import gbl, InterfaceCast
from GaudiPython.Bindings import Helper, iProperty
from AthenaCommon.Logging import logging
    
class TrigApp(object):
   """Helper class to work with Python bindings in athenaMT/PT"""

   log = logging.getLogger("TrigApp" )
   
   def service(self, name, iface = gbl.IService):
      """Retrieve service with interface iface.
         Example: trigApp.service('MessageSvc',gbl.IMessageSvc)"""
      
      svc = InterfaceCast(iface)(Helper.service(gbl.Gaudi.svcLocator(), name))
      return svc

   def changeJobProperties(self, clientName, propertyName, newValue):
      """Change the property of a client to a new value. Both client and property
         can be a regular expression in which case all properties matching will
         be changed to the new value. Note, this will only change properties that
         already exist in the JobOptionsCatalogue.
         Example: trigApp.changeJobProperties('.*', 'OutputLevel', DEBUG)"""
      
      jobOptSvc = InterfaceCast(gbl.Gaudi.Interfaces.IOptionsSvc)(Helper.service(gbl.Gaudi.svcLocator(), "JobOptionsSvc"))
      if not jobOptSvc:
         self.log.error("Cannot find JobOptionsSvc")
         return

      import re
      reClient = re.compile(clientName)
      reProp = re.compile(propertyName)

      ## Loop over all properties
      ## (cannot figure out how to use IOptionsSvc::broadcast from Python)
      for p in jobOptSvc.items():
         name = p._0
         value = p._1
         client, prop = name.rsplit('.',1)
         if reClient.match(client) and reProp.match(prop):
            self.log.info("Changing %s.%s from '%s' to '%s'", client, prop, value, newValue)
            iprop = iProperty(client)
            setattr(iprop, prop, newValue)
               
      return

            
## Instantiate an application helper
trigApp = TrigApp()

