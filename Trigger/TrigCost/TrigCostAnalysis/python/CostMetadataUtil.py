#!/usr/bin/env python
#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''
@file CostMetadataUtil.py
@brief Helper functions to create cost metadata json file based on input ntuple
        and histogram under/overflows
'''
import xml.etree.ElementTree as ET
from AthenaCommon.Logging import logging
log = logging.getLogger('CostAnalysisPostProcessing')


def saveMetadata(inputFile, argsMetadata={}, processingWarnings=[]):
    ''' @brief Save metadata from ntuple to json file
    '''
    import json

    metatree = inputFile.Get("metadata")
    if metatree is None:
        return None

    metatree.GetEntry(0)
    metadata = []

    metadata.append({'runNumber' : metatree.runNumber})
    metadata.append({'Details' : argsMetadata["userDetails"]})
    metadata.append({'JIRA' : argsMetadata["jira"]})
    metadata.append({'AMITag' : argsMetadata["amiTag"]})
    if "ProcessedRanges" in metadata:
        metadata.append({'ProcessedRanges' : str(metatree.ProcessedRanges)})

    if argsMetadata["amiTag"]:
        metadata += readHLTConfigKeysFromAMI(argsMetadata["amiTag"])
    else:
        metadata += readHLTConfigKeysFromCOOL(metatree.runNumber)

    if metatree.hostname and argsMetadata["readOKSDetails"]:
        metadata.append({'OKS configuration' : addOKSDetails(str(metatree.hostname), metatree.runNumber, argsMetadata["partition"])})
    elif metatree.hostname:
        metadata.append({'Hostnames' : str(metatree.hostname)})

    metadata.append({'AtlasCostProcessingProject' : str(metatree.AtlasProject)})
    metadata.append({'AtlasCostProcessingVersion' : str(metatree.AtlasVersion)})

    metadata.append({'ChainMonitor' : metatree.ChainMonitor})
    metadata.append({'AlgorithmMonitor' : metatree.AlgorithmMonitor})
    metadata.append({'AlgorithmClassMonitor' : metatree.AlgorithmClassMonitor})
    metadata.append({'ROSMonitor' : metatree.ROSMonitor})
    metadata.append({'GlobalsMonitor' : metatree.GlobalsMonitor})
    metadata.append({'ThreadMonitor' : metatree.ThreadMonitor})

    metadata.append({'AdditionalHashMap' : str(metatree.AdditionalHashMap)})
    metadata.append({'DoEBWeighting' : metatree.DoEBWeighting})
    metadata.append({'BaseEventWeight' : metatree.BaseEventWeight})

    metadata.append({'Histogram under/overflows' : processingWarnings})

    metadata.append({'HLTMenu' : json.loads(str(metatree.HLTMenu))})

    with open('metadata.json', 'w') as outMetaFile:
        metafile = {}
        metafile['text'] = 'metadata'
        metafile['children'] = metadata
        json.dump(obj=metafile, fp=outMetaFile, indent=2, sort_keys=True)


def createOverflowSummary(warnings):
    ''' @brief Create summery of under/overflows based on passed warnings
    '''
    histogramStats = {}
    log.debug("Received %s warnings", len(warnings))
    for entry in warnings:
        histFullName = entry.split(" ")[-1]
        histType = histFullName.split("_")[-2] + "_" + histFullName.split("_")[-1]
        summary = entry.split(" ")[-1].split("HLT")[0] + "HLT"

        if "LumiBlock" in summary:
            # format LumiBlock_000XX_SummaryName...
            summary = summary.split('_', 1)[1]
            summary = summary.split('_', 1)[1]
        elif "All" in summary:
            # format All_SummaryName...
            summary = summary.split('_', 1)[1]

        entryName = summary + "_" + histType
        if entryName in histogramStats:
            histogramStats[entryName] += 1
        else:
            histogramStats[entryName] = 1

    histogramStatsStr = []
    for name, value in histogramStats.items():
        histogramStatsStr.append("{0}: {1} histograms with over/underflows".format(name, value))

    return {"Summary": histogramStatsStr}


def ignoreUnderflow(histogramName):
    ''' @brief Filter out the histograms to ignore in underflow check
    '''

    # Ignore time histograms for filters
    if "FStep" in histogramName and "Time" in histogramName:
        log.debug("Filter %s underflow will be ignored", histogramName)
        return True

    return False


def addOKSDetails(hostname, runNumber, partition):
    ''' @brief Retrieve additional run metadata from oks repository
    '''
    oksMetadata = []

    # Clone TDAQ repository
    oksTag = "r{0}@{1}".format(runNumber, partition)
    log.info("Cloning tdaq-09-04-00 Tag " + oksTag)
    import os
    os.system("git clone https://gitlab.cern.ch/atlas-tdaq-oks/p1/tdaq-09-04-00.git --branch " + oksTag + " --single-branch")

    # Browse OKS    
    try:
        if partition == "TDAQ":
            partitionRoot = ET.parse('tdaq-09-04-00/daq/partitions/TDAQ.data.xml').getroot()
            hltRoot = ET.parse('tdaq-09-04-00/daq/segments/HLT/HLT-TDAQ.data.xml').getroot()
        elif partition == "ATLAS":
            partitionRoot = ET.parse('tdaq-09-04-00/combined/partitions/ATLAS.data.xml').getroot()
            hltRoot = ET.parse('tdaq-09-04-00/daq/segments/HLT/HLT-internal.data.xml').getroot()
    except FileNotFoundError as e:
        log.warning("OKS files not available: {0}".format(e))
        return []


    # Read F/T/S
    racksToComp = dict()
    if 'pc' in hostname:
        # Convert computer names to set of racks
        for computerName in hostname.split(","):
            rackName = findRackForComputer(computerName)
            if rackName not in racksToComp:
                racksToComp[rackName] = list()
            racksToComp[rackName].append(computerName)

        hostname = ",".join(racksToComp.keys())

    for rack in hostname.split(","):
        hltApplication = findHLTApplication(partitionRoot, hltRoot, rack, partition)

        metadataDict = [{'Hostname' : rack},                     
                        {'Forks' : hltRoot.findall("./*[@id='{0}']/*[@name='numForks']".format(hltApplication))[0].get("val")},
                        {'Threads' : hltRoot.findall("./*[@id='{0}']/*[@name='numberOfAthenaMTThreads']".format(hltApplication))[0].get("val")},
                        {'Slots' : hltRoot.findall("./*[@id='{0}']/*[@name='numberOfEventSlots']".format(hltApplication))[0].get("val")}]

        if rack in racksToComp:
            metadataDict.append({'Computers' : str(racksToComp[rack])})

        oksMetadata.append(metadataDict)

    # Cleanup cloned repository
    os.system("rm -rf tdaq-09-04-00")
    return oksMetadata


def findHLTApplication(partitionRoot, hltRoot, hostname, partitionName="ATLAS"):
    ''' @brief Find HLT application based on hostname and disabled segments
    '''
    segments = []

    if hostname:
        # Find segment based on hostname
        for segment in hltRoot.findall("./*[@class='TemplateSegment']/*[@name='Racks']/*[@id='{0}'].../...".format(hostname)):
            segments.append(segment.get("id"))
    else:
        # Get available segments
        segments = []
        for segment in hltRoot.findall("./*[@class='Segment']/*[@name='Segments']/*[@class='TemplateSegment']"):
            segments.append(segment.get("id"))

    log.debug("Found segments {0}".format(segments))

    # disabled segments
    for segment in partitionRoot.findall("./*[@class='Partition']/*[@name='Disabled']/*[@class='TemplateSegment']"):
        if segment.get("id") in segments:
            segments.remove(segment.get("id"))

    if len(segments) > 1:
        log.warning("Found more than one enabled segment, will use {0}".format(segments[0]))

    return hltRoot.findall("./*[@id='{0}']/*[@name='Applications']/*[@class='HLTRCApplication']".format(segments[0]))[0].get("id")


def findRackForComputer(computerName):
    ''' Find rack for computer name '''

    import re
    m = re.search(r'pc-tdq-tpu-(.*?)\.cern\.ch', computerName)
    if m:
        computerNum = m.group(1)
        rackNumber = computerNum[0:2]
        return "tpu-rack-{0}".format(rackNumber)

    log.warning("Cannot retrieve rack number from {0}".format(computerName))
    return ""


def readHLTConfigKeysFromCOOL(runNumber):
    ''' 
    Returns list of config keys read from COOL:
        DB - database alias
        Release - release
        SMK - Super Master Key
        HLTPSK - HLT Prescale keys
        LVL1PSK - L1 Prescale keys
        AtlasProject - Atlas project
    '''

    configMetadata = []

    from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
    dbconn = TriggerCoolUtil.GetConnection("CONDBR2")
    configKeys = TriggerCoolUtil.getHLTConfigKeys(dbconn, [[runNumber, runNumber]])

    if configKeys and runNumber in configKeys.keys():
        configKeys = configKeys[runNumber]

        configMetadata.append({'Release' : configKeys['REL']})
        configMetadata.append({'SMK' : configKeys['SMK']})
        
        # Split Run 3 db info into dbalias and atlas project,
        #  for example TRIGGERDBDEV1;22.0.20;Athena -> TRIGGERDBDEV1
        dbInfo = configKeys['DB'].split(';')
        
        configMetadata.append({'DB' : dbInfo[0]})
        if len(dbInfo) == 3:
            # Run 3 format
            configMetadata.append({'AtlasProject' : dbInfo[2]})

        configMetadata.append({'HLTPSK' : str(TriggerCoolUtil.getHLTPrescaleKeys(dbconn, [[runNumber, runNumber]])[runNumber]['HLTPSK2'])})
        configMetadata.append({'LVL1PSK' : str(TriggerCoolUtil.getL1ConfigKeys(dbconn, [[runNumber, runNumber]])[runNumber]['LVL1PSK'])})

    else:
        log.warning("Config keys not found in COOL")

    return configMetadata



def readHLTConfigKeysFromAMI(amiTag):
    ''' 
    Returns list of config keys read from AMI Tag:
        DB - database alias
        Release - release
        SMK - Super Master Key
        HLTPSK - HLT Prescale keys
        LVL1PSK - L1 Prescale keys
    '''

    configMetadata = []

    try:
        import pyAMI.client
        import pyAMI.atlas.api as AtlasAPI
    except ModuleNotFoundError:
        log.warning("Unable to import AMIClient from pyAMI. Maybe you didn't do localSetupPyAMI?")
        return configMetadata

    amiclient = pyAMI.client.Client('atlas')
    AtlasAPI.init()

    command = [ 'AMIGetAMITagInfo', '-amiTag="%s"' % amiTag,]
    amiTagInfo = amiclient.execute(command, format = 'dict_object').get_rows('amiTagInfo')[0]

    configMetadata.append({'Release' : amiTagInfo['SWReleaseCache']})
    configMetadata.append({'SMK' : amiTagInfo['DBsmkey'] if "DBsmkey" in amiTagInfo else None}) 
    configMetadata.append({'DB' : amiTagInfo['DBserver'] if "DBserver" in amiTagInfo else None})
    configMetadata.append({'HLTPSK' : amiTagInfo['DBhltpskey'] if "DBhltpskey" in amiTagInfo else None})
    configMetadata.append({'LVL1PSK' : amiTagInfo['DBl1pskey'] if "DBl1pskey" in amiTagInfo else None})

    return configMetadata