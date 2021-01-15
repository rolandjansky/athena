#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# @brief: Pre and Post debug_stream analysis operations for trigger transform
# @details: Code to carry out operations that are needed for running
#   the trigger debug_stream analysis in transform
# @author: Carlos Chavez

from __future__ import print_function

import os
import sys
import re
import eformat
from TrigTransform.dbgEventInfo import dbgEventInfo
from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
from PyCool import cool
from ROOT import TFile

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)


def dbgPreRun(inputFileList, outputFileList):
    msg.info('Running debug_stream analysis PreRun operations on files :{0} '.format(inputFileList))
    msg.info('Running debug_stream analysis PreRun, histogram output in :{0} '.format(outputFileList))

    # Open root output file
    outFile = outputFileList[0]
    hfile = TFile(outFile, 'RECREATE')

    # Inicialize dbgEventInfo, this is the main event analysis class
    eventInfo = dbgEventInfo('_Pre', inputFileList.value[0])
    data = []
    l1Info = []
    hltInfo = []
    relInfo = []
    for inputFile in inputFileList.value:
        bsfile = eformat.istream(inputFile)
        n = 0
        isFirstEvent = True

        for event in bsfile:
            # If first event get l1 and hlt counter and chain info from DB or XML file
            if isFirstEvent:
                runInfo = 0
                if event.run_no() == 0:
                    runInfo = int(inputFile.split(".")[1])
                else:
                    runInfo = event.run_no()
                l1Info, hltInfo, relInfo = TriggerDBInfo(runInfo)
                isFirstEvent = False
            n += 1
            # Log the details of first 5 events
            if n < 5:
                data = [event.run_no(), event.lumi_block(), event.global_id(),
                        event.lvl1_id(), event.bc_time_seconds(), event.bc_time_nanoseconds()]
                msg.info('Event details :%s', data)

            # Run debug event analysis and fill output TTree
            eventInfo.eventCount(event)
            eventInfo.eventInfo(event, l1Info, hltInfo)
            eventInfo.fillTree()

    # Close output TFile
    hfile.Write()
    hfile.Close()

    # Check release format, if relInfo is 'unknown' then print error
    release = relInfo[0]
    if not re.match(r'(\d+\.{0,1})+$', release):
        msg.error('Not able to find release from DB (or it was badly formatted), release : {0}'.format(release))
        msg.error('Problem with DB configuration in COOL DB, most likely during data-taking')

    msg.info('Finished running debug_stream analysis PreRun operations')

    # Returns the local asetupString from runs in input files and to be used by asetup
    return getAsetupString(*relInfo)


def dbgPostRun(inputFileList, outputFileList):
    msg.info('Running debug_stream analysis PostRun operations on files :{0} '.format(inputFileList))
    msg.info('Running debug_stream analysis PostRun, histogram output in :{0} '.format(outputFileList))

    # Open root output file
    outFile = outputFileList[0]
    hfile = TFile(outFile, 'UPDATE')

    # Inicialize dbgEventInfo,  this is the main event analysis class
    eventInfo = dbgEventInfo('_Pos', inputFileList.value[0])
    data = []
    l1Info = []
    hltInfo = []
    for inputFile in inputFileList.value:
        if not os.path.isfile(inputFile):
            msg.error('No BS file created with file name :{0} '.format(inputFileList))
            continue

        bsfile = eformat.istream(inputFile)
        n = 0
        isFirstEvent = True

        for event in bsfile:
            # If fist event get l1 and hlt counter and chain info from DB or XML file
            if isFirstEvent:
                l1Info, hltInfo, _ = TriggerDBInfo(event.run_no())
                isFirstEvent = False
            n += 1

            # Log the details of first 5 events
            if n < 5:
                data = [event.run_no(), event.lumi_block(), event.global_id(),
                        event.lvl1_id(), event.bc_time_seconds(), event.bc_time_nanoseconds()]
                msg.info('Event details :{0}'.format(data))

            # Run debug event analysis and fill output TTree
            eventInfo.eventCount(event)
            eventInfo.eventInfo(event, l1Info, hltInfo)
            eventInfo.fillTree()

    # Close output TFile
    hfile.Write()
    hfile.Close()
    msg.info('Finished running debug_stream analysis PostRun operations')


def TriggerDBInfo(runNumber):
    # Get the connection to CONDBR2
    dbconn = TriggerCoolUtil.GetConnection("CONDBR2")
    #dbconn  = TriggerCoolUtil.GetConnection("COMP")
    limMin = runNumber << 32
    limMax = (runNumber+1) << 32

    # Get L1 Info from DB
    l1Conn = dbconn.getFolder('/TRIGGER/LVL1/Menu')
    chanSel = cool.ChannelSelection.all()
    objs = l1Conn.browseObjects(limMin, limMax, chanSel)
    l1Info = []
    itemNames = {}
    while objs.goToNext():
        l1Obj = objs.currentRef()
        l1TriggerItem = l1Obj.channelId()
        l1Payload = l1Obj.payload()
        itemNames[l1TriggerItem] = l1Payload['ItemName']

    # Insert into map a pair for every l1 count and fill with item names if available
    l1Info = (max(itemNames.keys()) + 1) * [0]

    for triggerItem in itemNames:
        l1Info[triggerItem] = itemNames[triggerItem]

    # TODO revisit and remove the hack: left for standalone testing purposes
    # L1 Info from DB failed, now try from XML file
    if len(l1Info) == 1 and l1Info[0] == 0:
        msg.info('Failed to get L1Info from DB now trying using xml file hack')
        l1Info = getL1InfoXML()

    # Get HLT Info
    hltConn = dbconn.getFolder('/TRIGGER/HLT/Menu')
    chanSel = cool.ChannelSelection.all()
    objs = hltConn.browseObjects(limMin, limMax, chanSel)
    hltInfo = []
    chainNames = {}
    while objs.goToNext():
        hltObj = objs.currentRef()
        hltPayload = hltObj.payload()
        hltCounter = hltPayload['ChainCounter']
        chainNames[int(hltCounter)] = hltPayload['ChainName']

    hltInfo = (max(chainNames.keys()) + 1) * [0]

    for chainCounter in chainNames:
        hltInfo[chainCounter] = chainNames[chainCounter]

    # Get Atlas Project and HLT Release number
    hltConn = dbconn.getFolder('/TRIGGER/HLT/HltConfigKeys')
    chanSel = cool.ChannelSelection.all()
    objs = hltConn.browseObjects(limMin, limMax, chanSel)
    relInfo = 'unknown'

    while objs.goToNext():
        relObj = objs.currentRef()
        relPayload = relObj.payload()
        confsrc = relPayload['ConfigSource'].split(',')

        if len(confsrc) > 2:
            # Skip database name
            relInfo = confsrc[1:]

        msg.info('Found release: %s', relInfo)

    return (l1Info, hltInfo, relInfo)


# will be removed
def getL1InfoXML():
    # Try to find env var $AtlasPatchArea, then opens LVL1config xml file to extract L1 info
    AtlasPatchArea = str()
    eVar = 'AtlasPatchArea'
    if eVar in os.environ:
        AtlasPatchArea = os.environ[eVar].rstrip()
    else:
        msg.error('failed to find env variable: %s', eVar)

    lvl1XML = AtlasPatchArea + '/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_pp_v6.xml'

    l1Info = []
    ctpNames = {}
    l1Counts = 0
    file = open(lvl1XML, 'r')
    if not file:
        msg.info('failed to open: {0}'.format(lvl1XML))
        sys.exit(1)

    # dirty hack to read ctpid->name mapping
    for line in file.readlines():
        if line.find('<TriggerItem') != -1:
            ctpid = -1
            name = 'UNKNOWN'

            for item in line.split(' '):
                if item.find('ctpid') == 0:
                    ctpid = item.split('"')[1]
                if item.find('name') == 0:
                    name = item.split('"')[1]

            if ctpid == -1 or name == 'UNKNOWN':
                msg.error('Could not decode: %s', line)
            else:
                ctpNames[int(ctpid)] = name

            if int(ctpid) > l1Counts:
                l1Counts = int(ctpid)

    file.close()

    for i in range(l1Counts + 1):
        l1Info.insert(i, 0)

    for c in ctpNames:
        name = ctpNames[c]
        l1Info.pop(c)
        l1Info.insert(c, name)
        msg.debug("CTP counter: {0} chain: {1}".format(c, name))

    return l1Info


def getAsetupString(release, AtlasProject):
    # From release and os.environ, set asetupString for runwrapper.BSRDOtoRAW.sh
    if not AtlasProject:
        msg.warn('Atlas Project not available in TRIGGERDB - reading env variable')

        if os.environ['AtlasProject']:
            AtlasProject = os.environ['AtlasProject'].rstrip()
            msg.info('Found Atlas Project %s', AtlasProject)
        else:
            msg.error("Couldn't find Atlas Project!")

    asetupString = AtlasProject + ',' + release

    # If TestArea is for tzero (tzero/software/patches/AtlasP1HLT-RELEASE),
    #   then returns tzero/software/patches/AtlasP1HLT-release where release is
    #   the parameter given to this function getAsetupString(release)
    if os.environ['TestArea']:
        TestArea = os.environ['TestArea']
        if TestArea.find('tzero/software/patches/AthenaP1-') > 0:
            testarea = TestArea.split('-')
            TestArea = testarea[0] + '-' + release
        asetupString += ' --testarea ' + TestArea

    return asetupString
