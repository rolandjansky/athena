#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# GenerateHVDatFiles.py

import sqlite3, datetime, os, sys, datetime

# no arguments = recreate
# ./GetHVHistory.py startID = append evth starting from given id


from optparse import OptionParser
import time
from time import localtime
from time import mktime
from calendar import timegm
from datetime import datetime
from PyCool import cool
import math

outputList = {}
dataDir = 'HVHistoryDat'

def PrintHVInfo(hvline,tmstmp,folderName):
   
    from PyCool import cool
    outputList["voltage"] = []
    outputList["current"] = []
    outputList["status"] = []
    outputList["time"] = []
 
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    db=dbSvc.openDatabase("oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200")
    folder=db.getFolder(folderName)
    
    range=3*60*60*1e9 # 1 day in ns
    t1=long(tmstmp-range)
    t2=long(tmstmp+range)
    
    itr=folder.browseObjects(t1, t2, cool.ChannelSelection(hvline))
    while itr.goToNext():
        obj=itr.currentRef()
        pl=obj.payload()
        time_out = time.strftime("%Y-%m-%d:%H:%M:%S",localtime(obj.since()/1e9))
        outputList["voltage"].append(pl["R_VMEAS"])
        outputList["current"].append(pl["R_IMEAS"])
        outputList["status"].append(pl["R_STAT"])
        outputList["time"].append(time_out)

    itr.close()
    db.closeDatabase()
    return

def PrintLumiBlockTimes(tmstmp, minLB, maxLB, file):

    file.write("LumiBlocks:\n")
    tmstmp = int(time.mktime(time.strptime(tmstmp,"%Y-%m-%d %H:%M:%S")))*1000000000L

    dbSvc = cool.DatabaseSvcFactory.databaseService()
    db=dbSvc.openDatabase("oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200")
    folder=db.getFolder("/TRIGGER/LUMI/LBTIME")
    timerange=60*60*1e9 # 1 hour in ns
    t1=long(tmstmp-timerange)
    t2=long(tmstmp+timerange)
    
    itr=folder.browseObjects(t1,t2,cool.ChannelSelection.all())
    while itr.goToNext():
        obj=itr.currentRef()
        if obj.until()>t1:
            pl=obj.payload()
            run=pl["Run"]
            lb=pl["LumiBlock"]
            if(lb >= minLB) and (lb <= maxLB + 1):
                s = '{0:5d} {1:19s}\n'.format(lb, time.strftime("%Y-%m-%d:%H:%M:%S",localtime(obj.since()/1e9))) 
                file.write(s)
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



def GetHVlineHistory(tripID, hvline, timestamp, startLB, stopLB):

    hv = int(hvline)
    TimeStamp=int(time.mktime(time.strptime(timestamp,"%Y-%m-%d %H:%M:%S")))*1000000000L
	
    PrintHVInfo(hv,TimeStamp,"/LAR/DCS/HV/BARREl/I16")

    if len(outputList["time"]) == 0:
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

    filename = dataDir + '/trip_' + str(tripID) + '.dat' 
    f = file(filename,'w')

    s = 'HVLINE: ' + str(hvline) + '\n'
    f.write(s)


    # How much time before TripLb and after RecoveryLB to consider (in LB)
    offset = 3
    prefilled = False
    postfilled = False

    for i,v in enumerate(outputList["voltage"]):
		
        lb = outputList["LumiBlock0"][i]
	
        if (lb == startLB) and (not prefilled):
            # "Pre trip history"
			
            sec_time = time.mktime(time.strptime(outputList["time"][i], "%Y-%m-%d:%H:%M:%S"))
			
            # Checking for the previous entry
            if( sec_time - time.mktime(time.strptime(outputList["time"][i-1], "%Y-%m-%d:%H:%M:%S")) < 30) and (i >= 2):
			
                # If it is too close, we consider the one before as a stable

                voltageStable = outputList["voltage"][i-2]
                currentStable = outputList["current"][i-2]
                statusStable = outputList["status"][i-2]
                sec_time = time.mktime(time.strptime(outputList["time"][i-1], "%Y-%m-%d:%H:%M:%S"))

                # Writing down few entries before with stable values

                for j in range(0, offset):
                    lb = outputList["LumiBlock0"][i-1] - offset + j
                    node_time = time.strftime("%Y-%m-%d:%H:%M:%S", time.localtime(sec_time - 60 * (offset - j)))
                    s = '{0:19s} {1:5d} {2:.2f} {3:.3f} {4:5d}\n'.format(node_time, lb, voltageStable, currentStable, statusStable) 
                    f.write(s)
				
                # Writing down current entry (it is the real beginning of the trip)
                s = '{0:19s} {1:5d} {2:.2f} {3:.3f} {4:5d}\n'.format(outputList["time"][i-1],
										     outputList["LumiBlock0"][i-1],
										     outputList["voltage"][i-1],
 										     outputList["current"][i-1],
										     outputList["status"][i-1]) 
                f.write(s)
				


            else:

                # Otherwise the current one is considered stable
                voltageStable = outputList["voltage"][i-1]
                currentStable = outputList["current"][i-1]
                statusStable = outputList["status"][i-1]

                # Writing down few entries before with stable values

                for j in range(0, offset):	
                    lb = startLB - offset + j
                    node_time = time.strftime("%Y-%m-%d:%H:%M:%S", time.localtime(sec_time - 60 * (offset - j)))
                    s = '{0:19s} {1:5d} {2:.2f} {3:.3f} {4:5d}\n'.format(node_time, lb, voltageStable, currentStable, statusStable) 
                    f.write(s)
                    
            prefilled = True
		


        elif(lb >= startLB) and (lb <= stopLB) :

            s = '{0:19s} {1:5d} {2:.2f} {3:.3f} {4:5d}\n'.format(outputList["time"][i],
                                                                 outputList["LumiBlock0"][i],
                                                                 outputList["voltage"][i],
                                                                 outputList["current"][i],
                                                                 outputList["status"][i]) 
            f.write(s)




        elif (lb > stopLB) and prefilled and (not postfilled):
            # "Post trip history"

            voltage = outputList["voltage"][i]
            current = outputList["current"][i]
            status = outputList["status"][i]

            sec_time = time.mktime(time.strptime(outputList["time"][i-1], "%Y-%m-%d:%H:%M:%S"))

            for j in range(1, offset+1):
                lb = stopLB + j
                sec_time = sec_time + 60;
                node_time = time.strftime("%Y-%m-%d:%H:%M:%S", time.localtime(sec_time))
			
                s = '{0:19s} {1:5d} {2:.2f} {3:.3f} {4:5d}\n'.format(node_time,
                                                                     lb,
                                                                     outputList["voltage"][i],
                                                                     outputList["current"][i],
                                                                     outputList["status"][i]) 

                f.write(s)

            postfilled = True

    # Writing down LumiBlocks' times
    PrintLumiBlockTimes(timestamp, startLB - offset, stopLB + offset, f)

    f.close()
    
    return


# Main program

startID = int(0)

parser=OptionParser()
(options,args)=parser.parse_args()

# If no argument provided, we recreate everything
# Otherwise, append only new for trip with ID > startID

if len(args) == 0:
	newDB = True
else:
	newDB = False
	startID = int(args[0])


# Clearing data and picture directories

try:
	if not os.path.exists(dataDir):
		os.mkdir(dataDir)
	elif newDB:
		for the_file in os.listdir(dataDir):
	    		os.unlink(os.path.join(dataDir, the_file))		
except Exception, e:
	print e

#Connect to the database

dbName='trips.db'
conn = sqlite3.connect(dbName,detect_types=sqlite3.PARSE_DECLTYPES|sqlite3.PARSE_COLNAMES)

c = conn.cursor()

# Get TripId and HVLINE

if newDB:
	query = 'SELECT trips.rowid, trips.HVLINE, trips.TripTimeStamp, trips.TripLB, trips.RecoveryLB, trips.StableZero from trips WHERE (trips.StableBeams=\'Y\') ORDER BY trips.rowid'
else:
	query = 'SELECT trips.rowid, trips.HVLINE, trips.TripTimeStamp, trips.TripLB, trips.RecoveryLB, trips.StableZero from trips WHERE (trips.StableBeams=\'Y\') & (trips.rowID>=' + str(startID) + ') ORDER BY trips.rowid'

c.execute(query); 


# Get history for each trip

r = c.fetchall();


print len( r ), " trips found"
   
for i in range(len(r)):
	print "Working with tripId = ", r[i][0], ", HVLINE = ", r[i][1], ", timestamp = ", r[i][2], ", TripLB = ", r[i][3], ", RecoveryLB = ", r[i][4], "StableZero = ", r[i][5]
	if r[i][3] <= r[i][4]:
		GetHVlineHistory(r[i][0], r[i][1], r[i][2], r[i][3], r[i][4])
	else:
		# RecoveryLB is smaller then TripLB, using StableZero instead
		GetHVlineHistory(r[i][0], r[i][1], r[i][2], r[i][3], r[i][5])

# Closing

c.close()
conn.close()
print 'Done!'
