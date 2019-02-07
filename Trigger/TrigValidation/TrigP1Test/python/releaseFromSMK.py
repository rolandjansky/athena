#!/usr/bin/python

import sys
from TrigConfigSvc import TrigConfigSvcUtils

if len(sys.argv)<3:
  print "Please indicate DB and SMK"
 
else:
   connDB = str(sys.argv[1])
   smk = str(sys.argv[2])
   print "Going to get release for ",connDB," and ",smk

   result = TrigConfigSvcUtils.getPropertyFromDB(connDB,smk,"TagInfoMgr","ExtraTagValuePairs")

   print result

   array = result[0][3].split("'")
   new_array = [s for s in array if "AthenaP1" in s]
   new_array = new_array[0]
   new_array = new_array.replace("\\","")
   if "T" in new_array:
      release = new_array.replace("AthenaP1-","r")
   else:
      release = new_array.replace("AthenaP1-","")

   print "release found: ",release
   print "export release="+release
