#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# CheckHVStatus.py
# Script to return the HV status and additonal info from COOL

import os,sys
from optparse import OptionParser
import time
from time import localtime
from calendar import timegm
import math

outputList = {}

def PrintHVInfo(hvline,tmstmp,folderName):
    from PyCool import cool
    outputList["voltage"] = []
    outputList["current"] = []
    outputList["status"] = []
    outputList["time"] = []
    #from time import asctime,localtime
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    year = args[1][:4]
    if int(year) < 2014:
        db=dbSvc.openDatabase("oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200")
    else:
        db=dbSvc.openDatabase("oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=CONDBR2")
    folder=db.getFolder(folderName)
    range=3*60*60*1e9 # 1 day in ns
    t1=long(tmstmp-range)
    t2=long(tmstmp+range)
    itr=folder.browseObjects(t1,t2,cool.ChannelSelection(hvline))
    while itr.goToNext():
        obj=itr.currentRef()
        pl=obj.payload()
        time_out = time.strftime("%Y-%m-%d:%H:%M:%S",localtime(obj.since()/1e9))
        #print '{0:.2f}  {1:5d}  {2:23s}'.format(voltage,status,time_out)
        outputList["voltage"].append(pl["R_VMEAS"])
        outputList["current"].append(pl["R_IMEAS"])
        outputList["status"].append(pl["R_STAT"])
        outputList["time"].append(time_out)
    itr.close()
    db.closeDatabase()
    return

def GetCOOLInfo(folderName,dbName,channel,tmstmp,params):
    from PyCool import cool
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    db=dbSvc.openDatabase(dbName)
    folder=db.getFolder(folderName)
    range=0.5*60*60*1e9 # 1 day in ns
    chanList = []

    if type(channel)==type(int()):
        chanList.append(channel)
    elif type(channel)==type(list()):
        chanList = channel
    else:
        print channel," should be a list or an integer"
        sys.exit(-1)
    
    for p in params:
        for ch in chanList:
            outputList[p+str(ch)]=[]
    for t in tmstmp:
        t1=long(t-range)
        t2=long(t+range)
        for ch in chanList:
            itr=folder.browseObjects(t1,t2,cool.ChannelSelection(ch))
            infoFound=False
            while itr.goToNext():
                obj=itr.currentRef()
                if obj.until()>t:
                    pl=obj.payload()
                    for p in params:
                        outputList[p+str(ch)].append(pl[p])
                        infoFound=True
                    break
            itr.close()
            if infoFound==False:
                for p in params:
                    outputList[p+str(ch)].append(0)
    db.closeDatabase()
    return

                
def ConvertToUTC(ts_in):
    return time.gmtime(time.mktime(time.strptime(ts_in,"%Y-%m-%d:%H:%M:%S")))


if __name__=='__main__':
    if (os.system('which AtlCoolCopy.exe  2>/dev/null 1>/dev/null')!=0):
        print "Cannot find AtlCoolCopy.exe - need offline release setup"
        sys.exit(-1)
    try:
        from PyCool import cool
    except Exception, e:
        print e
        print "ERROR: Can't import PyCool module, wrong setup?"
        sys.exit(-1)

    parser=OptionParser()
    (options,args)=parser.parse_args()
    if len(args)!=2:
        print "ERROR: you must provide hvline timestamp (local time):\n eg. ./CheckHVStatus.py 161014 2011-03-23:01:12:54"
        sys.exit(-1)
    hv = int(args[0])
    #print hv

    #print args[1]
    ts = ConvertToUTC(args[1])
    #print time.strftime("%Y-%m-%d:%H:%M:%S",ts),"UTC"
    
    TimeStamp=int(timegm(ts))*1000000000L

    PrintHVInfo(hv,TimeStamp,"/LAR/DCS/HV/BARREl/I16")
    if len(outputList["time"])==0:
        PrintHVInfo(hv,TimeStamp,"/LAR/DCS/HV/BARREL/I8")

    ts_list = []
    for it in outputList["time"]:
        ts_list.append(int(timegm(ConvertToUTC(it)))*1000000000L)

    GetCOOLInfo("/TRIGGER/LUMI/LBTIME","oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200",0,ts_list,["Run","LumiBlock"])
    GetCOOLInfo("/LHC/DCS/FILLSTATE","oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200",1,ts_list,["StableBeams","FillNumber","NumBunchColl"])
    GetCOOLInfo("/EXT/DCS/MAGNETS/SENSORDATA","oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200",[1,3],ts_list,["value"])

    for s,solc in enumerate(outputList["value1"]):
        if abs(solc-7730.) < 10:
            outputList["value1"][s] = "ON"
        elif abs(solc-0.) < 10:
            outputList["value1"][s] = "OFF"
        else:
            outputList["value1"][s] = "RAMP"

    for s,torc in enumerate(outputList["value3"]):
        if abs(torc-20400.) < 10:
            outputList["value3"][s] = "ON"
        elif abs(torc-0.) < 10:
            outputList["value3"][s] = "OFF"
        else:
            outputList["value3"][s] = "RAMP"

    print "\nInformation for HV line",hv,"\n"
    print "      Date/Time       Voltage     Current   Status     Run       LB  StableBeams  Fill  #CollBunches  Solenoid  Toroid"
    for i,v in enumerate(outputList["voltage"]):
        print "%19s" % (outputList["time"][i]),
        print " ",
        print ('{0:.2f}'.format(v)).rjust(7),
        print "  ",
        print ('{0:.3f}'.format(outputList["current"][i])).rjust(7),
        print "  ",
        print ('{0:5d}'.format(outputList["status"][i])).rjust(5),
        print "  ",
        print ('{0:5d}'.format(outputList["Run0"][i])).rjust(6),
        print " ",
        print ('{0:5d}'.format(outputList["LumiBlock0"][i])).rjust(4),
        print "     ",("N","Y")[outputList["StableBeams1"][i]],
        print "    ",
        print ('{0:5d}'.format(outputList["FillNumber1"][i])).rjust(5),
        print "  ",
        print ('{0:5d}'.format(outputList["NumBunchColl1"][i])).rjust(4),
        print "      ",outputList["value1"][i].rjust(4),
        print "   ",outputList["value3"][i].rjust(4)

