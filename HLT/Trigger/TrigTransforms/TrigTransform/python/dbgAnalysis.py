#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @brief: Pre and Post debug_stream analysis operations for trigger transform
# @details: Code to carry out operations that are needed for running the trigger debug_stream analysis in transform
# @author: Carlos Chavez

from __future__ import print_function

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

import os
import re
import eformat
from TrigTransform.dbgEventInfo import dbgEventInfo
from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
from PyCool import cool

import ROOT
from ROOT import TFile

def dbgPreRun(inputFileList,outputFileList):
     
    msg.info('Running debug_stream analysis PreRun operations on files :{0} '.format(inputFileList))
    msg.info('Running debug_stream analysis PreRun, histogram output in :{0} '.format(outputFileList))
     
    total = 0
    #open root output file
    out_file = outputFileList[0]
    hfile = TFile( out_file , 'RECREATE' )
    #inicialize dbgEventInfo,  this is the main event analysis class
    eventInfo = dbgEventInfo("_Pre",inputFileList.value[0])
    data = []
    l1Info = []
    hltInfo = []
    relInfo = []
    runInfo = 0
    for inputFile in inputFileList.value:
        
        bsfile = eformat.istream(inputFile)
        events = len(bsfile)
        total += events
        n = 0 
        isFirstEvent = True

        for event in bsfile:
            # if fist event get l1 and hlt counter and chain info from DB or XML file
            if isFirstEvent:
                if event.run_no() == 0 : runInfo = int(inputFile.split(".")[1])
                else : runInfo =  event.run_no() 
                #e = bsfile[0] 
                l1Info, hltInfo, relInfo = TriggerDBInfo(runInfo)        
                isFirstEvent = False
                #runInfo.append(relInfo)  
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

    #Release format should be good, if relInfo is 'uknown' then print this error 
    release = relInfo[0]
    if not re.match(r'(\d+\.{0,1})+$', release):
        msg.error('Not able to find release from DB (or it was badly formatted), release : %s' % release )
        msg.error('Problem with DB configuration in COOL DB, most likely during data-taking' )
        
    msg.info('Finished running debug_stream analysis PreRun operations')     
    #returns the local asetupString from runs in input files and to be used by asetup 
    return getAsetupString(*relInfo)
    
def dbgPostRun(inputFileList,outputFileList):
    msg.info('Running debug_stream analysis PostRun operations on files :{0} '.format(inputFileList))
    msg.info('Running debug_stream analysis PostRun, histogram output in :{0} '.format(outputFileList))
     
    total = 0
    #open root output file
    out_file = outputFileList[0]
    hfile = TFile( out_file , 'UPDATE' )
    #inicialize dbgEventInfo,  this is the main event analysis class
    eventInfo = dbgEventInfo("_Pos",inputFileList.value[0])
    data = []
    l1Info = []
    hltInfo = []
    relInfo = []
    for inputFile in inputFileList.value:
    
        if not os.path.isfile(inputFile):
            msg.error('No BS file created with file name :{0} '.format(inputFileList))
            continue        
        bsfile = eformat.istream(inputFile)
        events = len(bsfile)
        total += events
        n = 0 
        isFirstEvent = True

        for event in bsfile:
            # if fist event get l1 and hlt counter and chain info from DB or XML file
            if isFirstEvent:
                #e = bsfile[0] 
                l1Info, hltInfo, relInfo = TriggerDBInfo(event.run_no())        
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
    dbconn = TriggerCoolUtil.GetConnection("CONDBR2")
    #dbconn  = TriggerCoolUtil.GetConnection("COMP")
    l1Info = []
    
    limmin = run<<32
    limmax = (run+1)<<32

    ## Get L1 Info from DB
    l1Conn = dbconn.getFolder( "/TRIGGER/LVL1/Menu" )
    l1Chansel = cool.ChannelSelection.all()
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
    chansel = cool.ChannelSelection.all()
    objs = f.browseObjects( limmin,limmax,chansel)
    hltInfo = []
    chainCount = 0
    chainNamesHLT = {}

    while objs.goToNext():
        hltObj = objs.currentRef()
        hltPayload = hltObj.payload()
        hltName = hltPayload['ChainName']
        hltCounter = hltPayload['ChainCounter']
        chainNamesHLT[int(hltCounter)] = hltName
        hltInfo = (max(chainNamesHLT.keys())+1) * [0]

    for channel in chainNamesHLT:
        hltInfo.pop(channel)
        hltInfo.insert(channel,chainNamesHLT[channel])

    # Get Atlas Project and HLT Release number
    f = dbconn.getFolder( "/TRIGGER/HLT/HltConfigKeys" )
    chansel = cool.ChannelSelection.all()
    objs = f.browseObjects( limmin,limmax,chansel)
    relInfo = 'unknown'
        
    while objs.goToNext():
        relObj = objs.currentRef()
        relPayload = relObj.payload()
        confsrc = relPayload['ConfigSource'].split(',')

        if len(confsrc) > 2:
            # Skip database name
            relInfo = confsrc[1:]

        msg.info("release: %s", relInfo)

    return (l1Info, hltInfo, relInfo) 

def getL1InfoXML():
    # Try to find  env var $AtlasPatchArea, then opens LVL1config xml file to extract L1 info.
    AtlasPatchArea = str()
    eVar = 'AtlasPatchArea'
    if eVar in os.environ:
        AtlasPatchArea = os.environ[eVar].rstrip()
    else:
        msg.error('failed to find env variable : $'+eVar)
        
    lvl1XML = AtlasPatchArea+'/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_pp_v6.xml'

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
                print ("ERROR, could not decode:",line)
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
        #print ("CTP counter : "+str(c)+"  chain :"+name)

    return l1Info

    
def getAsetupString(release, AtlasProject):
    # From release and os.environ, sets asetupString for runwrapper.BSRDOtoRAW.sh

    if not AtlasProject:
        msg.warn("Atlas Project not available in TRIGGERDB - reading env variable")

        if os.environ['AtlasProject']:
            AtlasProject = os.environ['AtlasProject'].rstrip()
            msg.info("Found Atlas Project %s" % AtlasProject)
        else:
            msg.error("Couldn't find Atlas Project!")

    asetupString = AtlasProject + ',' + release

    # If TestArea is for tzero (tzero/software/patches/AtlasP1HLT-RELEASE), 
    #   then returns tzero/software/patches/AtlasP1HLT-release where release is 
    #   the parameter given to this function getAsetupString(release)    
    if os.environ['TestArea']:
        TestArea = os.environ['TestArea']
        if TestArea.find("tzero/software/patches/AthenaP1-") > 0 :
            testarea = TestArea.split('-')
            TestArea = testarea[0] + '-' + release
        asetupString += ' --testarea '+ TestArea

    return asetupString
