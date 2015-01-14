#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# CheckHVStatus_daily.py
# Script to return the HV status and additonal info from COOL

import os,sys
from optparse import OptionParser
import time
from time import localtime
from calendar import timegm
import cPickle

def printStatus(stat):

    if stat & 0x400:
        result="Channel ON"
    else:
        result="Channel OFF"
    
    if stat & 1:
        result+= " Current Trip"
    if stat & 2:
        result+= " Sum error"
    if stat & 0x200:
        result+= " Input error"
    if stat & 0x800:
        result+=" Ramping"
    if stat & 0x1000:
        result+=" Emergency stop"
    if stat & 0x2000:
        result+=" Kill enable"
    if stat & 0x4000:
        result+= " Current Limit Error"
    if stat & 0x8000:
        result+= " Voltage Limit Error"
    return result


def isAllOk(stat):
    #Ignore kill-enable bit, after that must be "channel on" and nothing else
    #return (stat & 0xffffdfff) == 0x400 
    return (stat & 0xde81)==0x400 #ignores all undefined bits

def isOff(stat):
    return (stat & 0x400)==0

def isRamp(stat):
    return (stat & 0x800)!=0

def isRampUp(stat):
    # ramp bit set, no current or voltage limit error, channel on
    return ((stat & 0x800)!=0 and (stat & 0xD000)==0 and (stat & 0x400)!=0)

def isStableZero(stat,v,i):
    return (not isRamp(stat) and v<50 and i<0.50)

def findLArHVTrips(folderName,t1,t2,lbtimes=None):

    dbSvc = cool.DatabaseSvcFactory.databaseService()
    db=dbSvc.openDatabase("oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=CONDBR2")
    
    allTrips={}

    tripCount = int(0)
    nullCount = int(0)

    if not db.existsFolder(folderName):
        print "ERROR: Folder",folderName,"does not exist"
        return None
    
    f=db.getFolder(folderName)

    print "Checking",folderName

    #lastVoltage=dict()
    #lastStat=dict()

    offMap=dict()

    tripped=set()
    rampingUp=set()
    stablezero=set()

    itr=f.browseObjects(cool.ValidityKey(t1),cool.ValidityKey(t2),
                        cool.ChannelSelection.all())

    prev=0

    while itr.goToNext():
        obj=itr.currentRef()
        chid=obj.channelId();
        pl=obj.payload()
        stat=pl["R_STAT"]
        v=pl["R_VMEAS"]
        i=pl["R_IMEAS"]

        #if chid==286013:
        #    print obj.payload(),prev.payload()

        if stat is None:
            nullCount += 1
            continue

        if not offMap.has_key(chid):
            offMap[chid]=isOff(stat)

        if offMap[chid]==True:
            if not isOff(stat):
                print "Line",chid," was OFF but is now ON"
            continue

        ##if isOff(stat): continue #Ignore channels that are switched off

        if not isAllOk(stat):

            if chid in tripped:
                if isRampUp(stat):
                    if not chid in rampingUp:
                        rampingUp.add(chid)
                        rampStartTime = time.strftime("%Y-%m-%d:%H:%M:%S",localtime(obj.since()/1e9))
                        allTrips[chid][len(allTrips[chid])-1][3] = rampStartTime

                if chid in rampingUp:
                    if not isRampUp(stat):
                        rampingUp.remove(chid)

                if chid in stablezero:
                    if not isStableZero(stat,v,i):
                        stablezero.remove(chid)
                else:
                    if isStableZero(stat,v,i):
                        stablezero.add(chid)
                        stableZeroTime = time.strftime("%Y-%m-%d:%H:%M:%S",localtime(obj.since()/1e9))
                        allTrips[chid][len(allTrips[chid])-1][2] = stableZeroTime
            
            if chid not in tripped:
                #if prev and abs(obj.payload()["R_VMEAS"]-prev.payload()["R_VMEAS"])>10.:
                if prev:
                    tripCount += 1
                    #print obj.payload(),prev.payload()
                    #print "\nTrip: ",eventInfo(obj,stat,v,lbtimes)
                    tripTime = time.strftime("%Y-%m-%d:%H:%M:%S",localtime(obj.since()/1e9))
                    #print obj.channelId(),tripTime
                    tripped.add(chid)
                    if chid not in allTrips.keys():
                        allTrips[chid] = []
                    allTrips[chid].append([tripTime,"none","none","none"])

        else: #Not tripped
            if chid in tripped:
                #print "Recovered: ",eventInfo(obj,stat,v,lbtimes)
                tripped.remove(chid)
                recoTime = time.strftime("%Y-%m-%d:%H:%M:%S",localtime(obj.since()/1e9))
                #print obj.channelId(),recoTime
                allTrips[chid][len(allTrips[chid])-1][1] = recoTime

                if chid in rampingUp: rampingUp.remove(chid)
                if chid in stablezero: stablezero.remove(chid)


        prev=obj

    if not nullCount==0:
        print "Found",nullCount,"null modules (COOL database issue, please investigate)"

    print "Found",tripCount,"hv trips"
    itr.close()
    return allTrips


def GetCOOLInfo(folderName,dbName,channel,tmstmp,params):
    from PyCool import cool
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    db=dbSvc.openDatabase(dbName)
    folder=db.getFolder(folderName)
    range=0.5*60*60*1e9 # 1 day in ns

    coolInfoDict = {}

    chanList = []
    tmstmpList = []

    if type(channel)==type(int()):
        chanList.append(channel)
    elif type(channel)==type(list()):
        chanList = channel
    else:
        print channel," should be a list or an integer"
        sys.exit(-1)

    if type(tmstmp)==type(long()):
        tmstmpList.append(tmstmp)
    elif type(tmstmp)==type(list()):
        tmstmpList = tmstmp
    else:
        print tmstmp," should be a list or a long"
        sys.exit(-1)

##     for p in params:
##         for ch in chanList:
##             coolInfoDict[p+str(ch)]=[]
    for t in tmstmpList:
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
                        coolInfoDict[p+str(ch)] = pl[p]
                        infoFound=True
                    break
            itr.close()
            if infoFound==False:
                for p in params:
                    coolInfoDict[p+str(ch)] = 0
    db.closeDatabase()
    return coolInfoDict

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
    if len(args)==1:
        ts = ConvertToUTC(args[0])
    else:
        now = time.strftime("%Y-%m-%d:%H:%M:%S",(localtime(time.time())))
        print "no timestamp provided; using now:",now
        ts = ConvertToUTC(now)

    #print timegm(ts),timegm(ts) - 60*60*24
  
    TimeStamp=int(timegm(ts))*1000000000L
    TimeStamp1=int(timegm(ts) - 60*60*24)*1000000000L
    #TimeStamp1=int(timegm(ts) + 60*60*1)*1000000000L

    allTrips8 = findLArHVTrips("/LAR/DCS/HV/BARREL/I8",TimeStamp1,TimeStamp)
    allTrips16 = findLArHVTrips("/LAR/DCS/HV/BARREl/I16",TimeStamp1,TimeStamp)

    #allTrips = allTrips16
    allTrips = dict(allTrips8.items() + allTrips16.items())

##     print allTrips

##    allTrips = {154009L: [['2011-06-08:07:14:36', '2011-06-08:07:15:54', 'none', '2011-06-08:07:14:45']], 40015L: [['2011-06-07:08:25:46', '2011-06-07:08:27:24', 'none', '2011-06-07:08:25:53']], 75009L: [['2011-06-08:01:37:32', 'none', '2011-06-08:01:40:27', '2011-06-08:01:37:41']]}

    FILE = open("/afs/cern.ch/user/l/larmon/public/WebTools-scratch/HVTripsDB/dailyFiles/HVTrips_"+args[0]+".dat","w")

    #print " HV line    TRIP Date/Time [Run,LB]          Stable Zero   Ramp Up     RECOVERY Date/Time [Run,LB]       StableBeams  Fill  #CollBunches  Solenoid  Toroid"

    for hvline,tripList in allTrips.iteritems():

        #print tripList

        stableZeroLB = -1

        for itrip in tripList:

            ts = int(timegm(ConvertToUTC(itrip[0])))*1000000000L
            tripDict = GetCOOLInfo("/TRIGGER/LUMI/LBTIME","oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2",0,ts,["Run","LumiBlock"])

            if itrip[1] != "none":
                recots = int(timegm(ConvertToUTC(itrip[1])))*1000000000L
                recoDict = GetCOOLInfo("/TRIGGER/LUMI/LBTIME","oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2",0,recots,["Run","LumiBlock"])

            if itrip[2] != "none":
                stableZerots = int(timegm(ConvertToUTC(itrip[2])))*1000000000L
                stableZeroLB = GetCOOLInfo("/TRIGGER/LUMI/LBTIME","oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2",0,stableZerots,["Run","LumiBlock"])["LumiBlock0"]
                #print "stable zero",stableZeroLB

            if itrip[3] != "none":
                rampStartts = int(timegm(ConvertToUTC(itrip[3])))*1000000000L
                rampStartLB = GetCOOLInfo("/TRIGGER/LUMI/LBTIME","oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2",0,rampStartts,["Run","LumiBlock"])["LumiBlock0"]
                #print "ramp start",rampStartLB

            fillDict = GetCOOLInfo("/LHC/DCS/FILLSTATE","oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200",1,ts,["StableBeams","FillNumber","NumBunchColl"])
            magnetDict = GetCOOLInfo("/EXT/DCS/MAGNETS/SENSORDATA","oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=CONDBR2",[1,3],ts,["value"])

            if abs(magnetDict["value1"]-7730.) < 10:
                magnetDict["value1"] = "ON"
            elif abs(magnetDict["value1"]-0.) < 10:
                magnetDict["value1"] = "OFF"
            else:
                magnetDict["value1"] = "RAMP"

            if abs(magnetDict["value3"]-20400.) < 10:
                magnetDict["value3"] = "ON"
            elif abs(magnetDict["value3"]-0.) < 10:
                magnetDict["value3"] = "OFF"
            else:
                magnetDict["value3"] = "RAMP"

            writeString = "%d " % (hvline)
            writeString += "%s " % (itrip[0])
            writeString += "%d " % tripDict["Run0"]
            writeString += "%d " % tripDict["LumiBlock0"]
            if itrip[1] == "none":
                writeString += "%d " % stableZeroLB
                writeString += "-1 "
                writeString += "9999-12-31:23:00:00 "
                writeString += "0 "
                writeString += "0 "
            else:
                if itrip[2] == "none":
                    writeString += "-1 "
                else:
                    writeString += "%d " % stableZeroLB
                if itrip[3] == "none":
                    writeString += "-1 "
                else:
                    writeString += "%d " % rampStartLB
                writeString += "%s " % (itrip[1])
                writeString += "%d " % recoDict["Run0"]
                writeString += "%d " % recoDict["LumiBlock0"]
            writeString += ("N","Y")[fillDict["StableBeams1"]]
            writeString += " %d " % fillDict["FillNumber1"]
            writeString += "%d " % fillDict["NumBunchColl1"]
            writeString += magnetDict["value1"]
            writeString += " "
            writeString += magnetDict["value3"]
            writeString += " \n"

            FILE.write(writeString)

##             print ('{0:8d}'.format(hvline)).rjust(8),
##             print "  ",
##             print "%19s" % (itrip[0]),
##             print ('[{0:d},{1:d}]'.format(tripDict["Run0"],tripDict["LumiBlock0"])),
##             if itrip[1] != "none":
##                 print ('{0:d}'.format(rampStartLB)),
##                 print "  ",
##                 print ('{0:d}'.format(rampEndLB)),
##             else:
##                 print "            ",
##             print "%19s" % (itrip[1]),
##             if itrip[1] != "none":
##                 print ('[{0:d},{1:d}]'.format(recoDict["Run0"],recoDict["LumiBlock0"])),
##             else:
##                 print "            ",
##             print "     ",("N","Y")[fillDict["StableBeams1"]],
##             print "    ",
##             print ('{0:5d}'.format(fillDict["FillNumber1"])).rjust(5),
##             print "  ",
##             print ('{0:5d}'.format(fillDict["NumBunchColl1"])).rjust(4),
##             print "      ",magnetDict["value1"].rjust(4),
##             print "   ",magnetDict["value3"].rjust(4)
    

    FILE.close()
