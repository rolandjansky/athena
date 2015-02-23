#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @brief: Pre and Post debug_stream analysis operations for trigger transform
# @details: Code to carry out operations that are needed for running the trigger debug_stream analysis in transform
# @author: Carlos Chavez

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)


import os
import eformat
from TrigTransform.dbgEventInfo import dbgEventInfo
from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
from PyCool import cool

from PyUtils import RootUtils
ROOT = RootUtils.import_root()
from ROOT import TFile

def dbgPreRun(inputFileList,outputFileList):
     
    msg.info('Running debug_stream analysis PreRun operations on files :{0} '.format(inputFileList))
    msg.info('Running debug_stream analysis PreRun, histogram output in :{0} '.format(outputFileList))
     
    total = 0
    #open root output file
    out_file = outputFileList[0]
    hfile = TFile( out_file , 'UPDATE' )
    #inicialize dbgEventInfo,  this is the main event analysis class
    eventInfo = dbgEventInfo("_Pre")
    data = []
    l1Info = []
    hltInfo = []
    for inputFile in inputFileList.value:
        
        bsfile = eformat.istream(inputFile)
        events = len(bsfile)
        total += events
        n = 0 
        isFirstEvent = True

        for event in bsfile:
            # if fist event get l1 and hlt counter and chain info from DB or XML file
            if isFirstEvent:
                #e = bsfile[0] 
                l1Info, hltInfo = TriggerDBInfo(event.run_no())        
                isFirstEvent = False
            n += 1
            if n < 5:
                data = [event.run_no(),event.lumi_block(),event.global_id(),event.lvl1_id(),event.bc_time_seconds(),event.bc_time_nanoseconds()]
                msg.info('Event details :{0}'.format(data))
            #Run debug event analysis and fill output TTree
            eventInfo.event_count(event)
            eventInfo.event_info(event, l1Info, hltInfo)
            eventInfo.fillTree()
    #close output TFile
    hfile.Write()
    hfile.Close()  
    msg.info('Finished running debug_stream analysis PreRun operations')     
        
def dbgPostRun(inputFileList,outputFileList):
    msg.info('Running debug_stream analysis PostRun operations on files :{0} '.format(inputFileList))
    msg.info('Running debug_stream analysis PostRun, histogram output in :{0} '.format(outputFileList))
     
    total = 0
    #open root output file
    out_file = outputFileList[0]
    hfile = TFile( out_file , 'UPDATE' )
    #inicialize dbgEventInfo,  this is the main event analysis class
    eventInfo = dbgEventInfo("_Pos")
    data = []
    l1Info = []
    hltInfo = []
    for inputFile in inputFileList.value:
        
        bsfile = eformat.istream(inputFile)
        events = len(bsfile)
        total += events
        n = 0 
        isFirstEvent = True

        for event in bsfile:
            # if fist event get l1 and hlt counter and chain info from DB or XML file
            if isFirstEvent:
                #e = bsfile[0] 
                l1Info, hltInfo = TriggerDBInfo(event.run_no())        
                isFirstEvent = False
            n += 1
            if n < 5:
                data = [event.run_no(),event.lumi_block(),event.global_id(),event.lvl1_id(),event.bc_time_seconds(),event.bc_time_nanoseconds()]
                msg.info('Event details :{0}'.format(data))
            #Run debug event analysis and fill output TTree
            eventInfo.event_count(event)
            eventInfo.event_info(event, l1Info, hltInfo)
            eventInfo.fillTree()
    #close output TFile
    hfile.Write()
    hfile.Close()  
    msg.info('Finished running debug_stream analysis PostRun operations')


def TriggerDBInfo(run):
    #Get the connection to CONDBR2  
    dbconn  = TriggerCoolUtil.GetConnection("CONDBR2")
    #dbconn  = TriggerCoolUtil.GetConnection("COMP")
    l1Info = []
    
    limmin=run<<32
    limmax=(run+1)<<32

    ## Get L1 Info from DB
    l1Conn= dbconn.getFolder( "/TRIGGER/LVL1/Menu" )
    l1Chansel=cool.ChannelSelection.all()
    l1Objs = l1Conn.browseObjects( limmin,limmax,l1Chansel)
    itemName = {}
    l1Counts = 0
    while l1Objs.goToNext():
        l1Obj=l1Objs.currentRef()
        l1Channel = l1Obj.channelId()
        l1Payload = l1Obj.payload()
        itemname = l1Payload['ItemName']
        itemName[l1Channel] = itemname
        if (l1Channel>l1Counts): l1Counts = l1Channel

    for i in range(l1Counts+1):
        l1Info.insert(i,0)

    for c in itemName:
        name = itemName[c]
        l1Info.pop(c)
        l1Info.insert(c,name)

    # L1 Info from DB failed, now try from XML file 
    if len(l1Info) == 1 and l1Info[0] == 0  :
        msg.info('Failed to get L1Info from DB now trying using xml file hack')
        l1Info = getL1InfoXML()



    ## Get HLT Info
    f = dbconn.getFolder( "/TRIGGER/HLT/Menu" )
    chansel=cool.ChannelSelection.all()
    objs = f.browseObjects( limmin,limmax,chansel)
    hltInfo = []
    chainCount = 0
    chainNamesHLT = {}

    while objs.goToNext():
        hltObj=objs.currentRef()
        hltPayload=hltObj.payload()
        hltName     = hltPayload['ChainName']
        hltCounter  = hltPayload['ChainCounter']
        chainNamesHLT[int(hltCounter)] = hltName
        hltInfo = (max(chainNamesHLT.keys())+1) * [0]

    for channel in chainNamesHLT:
        hltInfo.pop(channel)
        hltInfo.insert(channel,chainNamesHLT[channel])

    return (l1Info, hltInfo) 

def getL1InfoXML():
    # Try to find  env var $AtlasPatchArea, then opens LVL1config xml file to extract L1 info.
    AtlasPatchArea = str()
    eVar = 'AtlasPatchArea'
    if eVar in os.environ:
        AtlasPatchArea = os.environ[eVar].rstrip()
    else:
        msg.error('failed to find env varriable : $'+eVar)
        
    lvl1XML = AtlasPatchArea+'/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_pp_v5.xml'

    l1Info = []
    ctpNames={}
    l1Counts = 0
    file=open(lvl1XML,'r')
    if not file:
        msg.info('failed to open: {0}'.format(lvl1XML))
        sys.exit(1)
    #print "dirty hack to read ctpid->name mapping"
    for line in file.readlines():
        if line.find('<TriggerItem')!=-1:
            ctpid=-1
            name='UNKNOWN'
            for item in line.split(' '):
                if item.find('ctpid')==0:
                    ctpid=item.split('"')[1]
                if item.find('name')==0:
                    name=item.split('"')[1]
            if ctpid==-1 or name=='UNKNOWN':
                print "ERROR, could not decode:",line
            else:
                ctpNames[int(ctpid)]=name
            if (int(ctpid)>l1Counts): l1Counts = int(ctpid)
    file.close()



    for i in range(l1Counts+1):
        l1Info.insert(i,0)

    for c in ctpNames:
        name = ctpNames[c]        
        l1Info.pop(c)
        l1Info.insert(c,name)
        #print "CTP counter : "+str(c)+"  chain :"+name

    return l1Info
