#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys,time
from PyCool import cool, coral

runNum = int(sys.argv[1])
since = (runNum << 32)
until = ((runNum+1) << 32)-1
dbSvc = cool.DatabaseSvcFactory.databaseService()
RunCtrlDB = 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO'
RunCtrlDb = dbSvc.openDatabase( RunCtrlDB )
try: RunCtrlDb = dbSvc.openDatabase( RunCtrlDB )
except Exception,e:
    print 'Problem opening database', e
    sys.exit(-1)
folder_runCtrl = RunCtrlDb.getFolder('/TDAQ/RunCtrl/EOR_Params')
chList = folder_runCtrl.listChannels()
if not chList:
    print 'ERROR : no data in /TDAQ/RunCtrl/EOR_Params'

for Id in chList:
    objs = folder_runCtrl.browseObjects(since,until,cool.ChannelSelection(Id))
    for obj in objs:
        payl = obj.payload()
#        print payl
        SORTime = payl['SORTime']
        EORTime = payl['EORTime']
        iovstart = obj.since()
        iovend = obj.until()
        nLB = iovend-(iovstart+1)
    objs.close()
    break
RunCtrlDb.closeDatabase()

times1 = SORTime/1000000000L
times2 = EORTime/1000000000L
ts1=time.gmtime(times1)
ts2=time.gmtime(times2)
startTime = time.strftime('%Y-%m-%d:%H:%M:%S/UTC',ts1)
endTime = time.strftime('%Y-%m-%d:%H:%M:%S/UTC',ts2)
#print startTime, endTime, nLB

fw = open('time.txt', 'w')
fw.write(startTime+' '+endTime+' '+str(nLB))
fw.close()
