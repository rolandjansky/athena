#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#COOLOFL_INDET /Indet/Align InDetAlign-RUN2-BLK-UPD4-13 
#logging.debug( "%s, %s, %s, %s" %(pcaltag.tag,pcaltag.folder,pcaltag.schema,pcaltag.status) )
#pcaltag.tag COOLOFL_INDET
#p3caltag.folder /Indet/Align
#pcaltag.schema InDetAlign-RUN2-BLK-UPD4-13 
#pcaltag.status 1

import time
from PyCool import cool
from collections import OrderedDict
import json
import urllib2  # the lib that handles the url stuff
import logging
from CoolRunQuery.utils.AtlRunQueryIOV    import IOVTime, IOVRange

def getDBNumberFromUPD4(run, tag, foldername, connstr):
   readonly=(connstr.find('//')==-1)
   tokens=connstr.split('/')
   schema=tokens[0]
   dbname=tokens[1]
   connstr2='oracle://ATLAS_COOLPROD;schema=ATLAS_%s;dbname=%s' % (schema,dbname)
   dbSvc=cool.DatabaseSvcFactory.databaseService()
   dbconn=dbSvc.openDatabase(connstr2,readonly)
   folder=dbconn.getFolder(foldername)   
   iovstart=int(run)<<32
   iovend=iovstart+0xFFFFFFFF
   nobj=0
   objs=folder.browseObjects(iovstart,iovend,cool.ChannelSelection.all(),tag)
   for obj in objs:
      payload = obj.payload()
      payload_string =  "%s" %( payload )
      DB_number =  payload_string.split()[-1].split("]")[0].split("[")[1] 
      #print "since [r,l]: [", obj.since() >> 32,',',obj.since()%0x100000000,']',
      #print "payload  : %s" %(DB_number)
      #print 
   return DB_number

def getRunNumberFromUPD1(DB_number, tag, run, foldername, connstr):
   readonly=(connstr.find('//')==-1)
   tokens=connstr.split('/')
   schema=tokens[0]
   dbname=tokens[1]
   connstr2='oracle://ATLAS_COOLPROD;schema=ATLAS_%s;dbname=%s' % (schema,dbname)
   dbSvc=cool.DatabaseSvcFactory.databaseService()
   dbconn=dbSvc.openDatabase(connstr2,readonly)
   folder=dbconn.getFolder(foldername)   
   iovstart=int(run)<<32
   nobj=0
   objs=folder.browseObjects( cool.ValidityKeyMin,
                              #iovstart, 
                              cool.ValidityKeyMax,
                              cool.ChannelSelection.all(),
                              tag)
   print "ref", DB_number
   for obj in objs:
      payload = obj.payload()
      payload_string =  "%s" %( payload )
      test_number =  payload_string.split()[-1].split("]")[0].split("[")[1] 
      #print test_number
      run_string = obj.since() >> 32,',',obj.since()%0x100000000
      #print "since [r,l]: [", obj.since() >> 32,',',obj.since()%0x100000000,']',
      #print "payload  : %s" %(test_number)
      if test_number == DB_number:
         return run_string 
   return 0

      #return run_string
      #else:
      #   return 0
      




from optparse import OptionParser

parser=OptionParser(usage="\n ./checkUPD1.py --upd4 <UPD4 folder tag name> --upd1 <UPD1 folder tag name> --folder <e.g. /Indet/Align> -r <run> \n")
parser.add_option("-r","--run" ,type="string"        ,dest="run"   ,default=None    ,help="runnumber in UPD4 tag")
parser.add_option("-b","--upd4" ,type="string"        ,dest="upd4"   ,default='InDetAlign-RUN2-BLK-UPD4-13'    ,help="runnumber in UPD4 tag")
parser.add_option("-e","--upd1" ,type="string"        ,dest="upd1"   ,default='InDetAlign-RUN2-ES1-UPD1-06'    ,help="runnumber in UPD1 tag")
parser.add_option("-f","--folder" ,type="string"        ,dest="folder"   ,default='/Indet/Align'    ,help="runnumber in UPD1 tag")
(options,args)=parser.parse_args()

run = options.run
upd4_tag = options.upd4
upd1_tag = options.upd1
 
foldername = options.folder
connstr = 'COOLOFL_INDET/CONDBR2'

if foldername == '/TRT/Align':
   connstr = 'COOLOFL_TRT/CONDBR2'
#print connstr
#print foldername

DB_number = getDBNumberFromUPD4(run, upd4_tag, foldername, connstr)
run = getRunNumberFromUPD1(DB_number, upd1_tag, run,  foldername, connstr)
if run:
   print "first run in UPD1 tag with matching UPD4 conditions is run %i"  %(run[0])
else:
   print "no matching conditions found in UPD1 tag!!!"

