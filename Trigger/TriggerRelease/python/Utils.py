# Copyright (C) 1995-2017 CERN for the benefit of the ATLAS collaboration
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

######################################################################
## @file   Utils.py
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

   
