# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################################
# @file:   TrigValTools/python/Utils.py
# @brief:  Python utilties for TrigValTools scripts
# @author: Frank Winklmeier, Will Buttinger
#
# $Id: Utils.py,v 1.1 2008-07-28 11:47:34 fwinkl Exp $
######################################################################

import os

def findInstallDir( dir="TrigValTools",
                    searchpath="DATAPATH" ):
   """Find the installation area of the package"""

   if not os.environ.has_key(searchpath): return None

   dirs = os.environ[searchpath].split(":")
   if not dirs: return None
   for d in dirs:
      path = os.path.join(d,dir)
      if os.path.exists(path): return path

   return None

   
   
def getPreviousNightlyPath( numDaysInPast=1 ):
   """Get path to previous nightly release results"""

   myPath= os.environ.get("NICOS_PROJECT_RELNAME_COPY","")
   #replace rel_x with rel_(x-1)
   for i in range(0,7):
         if ("rel_%d" % i) in myPath:
            myPath = myPath.replace( ("rel_%d" % i), ("rel_%d" % ( (i-numDaysInPast)%7 )) )
            break
   refFile = os.environ.get("NICOS_COPY_HOME","") + "/" + myPath + "/NICOS_area/NICOS_atntest" + os.environ.get("NICOS_SUFFIX","") + "/" + os.path.basename(os.environ.get("ATN_WORK_AREA",""))

   return refFile
