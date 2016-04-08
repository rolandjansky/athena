# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################################
## @file   TrigPyHelper.py
## @brief  Helpers for C++ Python bindings
## @author Frank Winklmeier
## $Id: TrigPyHelper.py,v 1.1 2008-09-02 16:37:18 fwinkl Exp $
######################################################################

from GaudiPython import *
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
      
      jobOptSvc = InterfaceCast(gbl.IJobOptionsSvc)(Helper.service(gbl.Gaudi.svcLocator(), "JobOptionsSvc"))
      if not jobOptSvc:
         log.error("Cannot find JobOptionsSvc")
         return

      import re
      reClient = re.compile(clientName)
      reProp = re.compile(propertyName)

      ## Loop over all clients/properties
      for client in jobOptSvc.getClients():
         for prop in jobOptSvc.getProperties(client):      
            if reClient.match(client) and reProp.match(prop.name()):
               self.log.info("Changing %s.%s from '%s' to '%s'" % \
                             (client, prop.name(), prop.value(), newValue))
               iprop = iProperty(client)
               setattr(iprop, prop.name(), newValue)
               
      return

            
## Instantiate an application helper
trigApp = TrigApp()

