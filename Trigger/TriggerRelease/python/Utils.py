# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################################
## @file   TriggerRelease/python/Utils.py
## @brief  Utilities for TriggerRelease
## @author Frank Winklmeier
## $Id: Utils.py,v 1.1 2008-08-20 13:07:06 fwinkl Exp $
######################################################################

def uniqueFileName():
   """Return a unique string for the application, which can be
   used as part of a file name"""

   import os
   # Unique for templated applications
   name = os.environ.get("TDAQ_APPLICATION_NAME","")
   # Unqiue for normal applications
   if name == "": name = os.environ.get("TDAQ_APPLICATION_OBJECT_ID","")
      
   if name != "":
      # Make sure no "/" is in the name
      name = name.replace("/","_")
   else:
      # Last resort: use process ID
      name = "PID-%d" % os.getpid()

   return name

   
