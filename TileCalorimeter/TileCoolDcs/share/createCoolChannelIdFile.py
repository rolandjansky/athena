#!/bin/env python
# Author: nils.gollub@cern.ch

import sys, os, re
import time
import ROOT
from TileCoolDcs import TileDCSDataInfo


buffer=[]

re_ai     = re.compile("ATLTILLV(\d{2})(:ELMB/LVCAN.*_)(\d*)/AI/M5VMB_TEMP2.value")
re_states = re.compile("ATLTILLV(\d{2})(:ELMB/LVCAN.*_)(\d*)/states.ForDAQ_MBHV")
re_hv     = re.compile("ATLTILLV(\d{2}):Drawer(\d*).Readings.Monitoring.hvOut1.value")
re_hvset  = re.compile("ATLTILLV(\d{2}):Drawer(\d*).Readings.Requests.hvOut1.order")
re_daq    = re.compile("ATLTILSCS:DAQ(.*).state")

#=== mapping of system name number to partition
partitionID = { 0 : "EBA",
                1 : "LBA",
                2 : "LBC",
                3 : "EBC"}


#=== connect to Oracle DB

par = sys.argv
info = TileDCSDataInfo.TileDCSDataInfo()
dbstring=info.get_dbstring('ORACLE',int(par[1]) if len(par)>1 and par[1].isdigit() else 2)
if dbstring is None:
    print "Unknown DB"
    sys.exit(2)

print "Connecting to",dbstring[0]

db = ROOT.TSQLServer.Connect( dbstring[0], dbstring[1], dbstring[2])


statement  = "select ELEMENT_NAME, SYS_ID, DP_ID from ATLAS_PVSSTIL.elements"
stmt = db.Statement(statement)
if stmt.Process()==True:
    stmt.StoreResult()
    while stmt.NextResultRow():

        element_name = stmt.GetString(0)
        sys_id       = stmt.GetString(1)
        dp_id        = stmt.GetString(2)

        #=== build COOL channel id
        channel_id = dp_id + ('000'+sys_id)[-3:]

        #=== find components
        result_ai     = re_ai.search(element_name)
        result_states = re_states.search(element_name)
        result_hv     = re_hv.search(element_name)
        result_hvset  = re_hvset.search(element_name)
        result_daq    = re_daq.search(element_name)

        if result_ai:
            partition = partitionID[int(result_ai.groups()[0])]
            drawer    = partition+('00'+result_ai.groups()[2])[-2:]
            oracleId  = "ATLTILLV"
            for part in result_ai.groups(): oracleId += part
            oracleId += "/AI"
            buffer.append( "/TILE/DCS/AI\t%s\t%s\t%s\n"%(drawer,channel_id,oracleId) )

        elif result_states:
            partition = partitionID[int(result_states.groups()[0])]
            drawer    = partition+('00'+result_states.groups()[2])[-2:]
            oracleId  = "ATLTILLV"
            for part in result_states.groups(): oracleId += part
            oracleId += "/states"
            buffer.append( "/TILE/DCS/STATES\t%s\t%s\t%s\n"%(drawer,channel_id,oracleId) )

        elif result_hv:
            part = result_hv.groups()
            partition = partitionID[int(part[0])]
            drawer    = partition+('00'+part[1])[-2:]
            oracleId  = "ATLTILLV"+part[0]
            oracleId += ":Drawer"+part[1]+".Readings.Monitoring"
            buffer.append( "/TILE/DCS/HV\t%s\t%s\t%s\n"%(drawer,channel_id,oracleId) )

        elif result_hvset:
            part = result_hvset.groups()
            partition = partitionID[int(part[0])]
            drawer    = partition+('00'+part[1])[-2:]
            oracleId  = "ATLTILLV"+part[0]
            oracleId += ":Drawer"+part[1]+".Readings.Requests"
            buffer.append( "/TILE/DCS/HVSET\t%s\t%s\t%s\n"%(drawer,channel_id,oracleId) )

        elif result_daq:
            part = result_daq.groups()
            partition = part[0][:3]
            oracleId  = "ATLTILSCS:DAQ"+partition+".state"
            #=== only one entry per partition, set module number to "XX"
            drawer = partition + "XX"
            buffer.append( "/DAQdummy/%s/DAQdummy\t%s\t%s\t%s\n"%(partition,drawer,channel_id,oracleId) )


#=== write file
oldname=info.find_data_file("cool_channel_id.dat")
newname="cool_channel_id.dat"
print "Old file:", oldname
print "New file:", newname
buffer.sort()
file = open(newname,"w")
for item in buffer:
    file.write( item )
