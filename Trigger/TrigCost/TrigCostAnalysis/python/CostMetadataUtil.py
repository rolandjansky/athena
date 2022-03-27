#!/usr/bin/env python
#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''
@file CostMetadataUtil.py
@brief Helper functions to create cost metadata json file based on input ntuple
        and histogram under/overflows
'''

from AthenaCommon.Logging import logging
log = logging.getLogger('CostAnalysisPostProcessing')


def saveMetadata(inputFile, userDetails, processingWarnings=[], readOKSDetails=False, partition="ATLAS"):
    ''' @brief Save metadata from ntuple to json file
    '''
    import json

    metatree = inputFile.Get("metadata")
    if metatree is None:
        return None

    metatree.GetEntry(0)
    metadata = []

    metadata.append({'runNumber' : metatree.runNumber})

    metadata.append({'Details' : userDetails})

    metadata += readHLTConfigKeys(metatree.runNumber)

    if readOKSDetails and metatree.hostname:
        metadata += addOKSDetails(metatree.hostname, metatree.runNumber, partition)

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
    import xml.etree.ElementTree as ET
    
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
    for host in hostname.split(","):
        hltApplication = findHLTApplication(partitionRoot, hltRoot, host, partition)

        oksMetadata.append([{'Hostname' : host},
                            {'Forks' : hltRoot.findall("./*[@id='{0}']/*[@name='numForks']".format(hltApplication))[0].get("val")},
                            {'Threads' : hltRoot.findall("./*[@id='{0}']/*[@name='numberOfAthenaMTThreads']".format(hltApplication))[0].get("val")},
                            {'Slots' : hltRoot.findall("./*[@id='{0}']/*[@name='numberOfEventSlots']".format(hltApplication))[0].get("val")}])

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


def readHLTConfigKeys(runNumber):
    ''' 
    Returns list of config keys:
        DB - database alias
        REL - release
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

        configMetadata.append({'HLTPSK' : TriggerCoolUtil.getHLTPrescaleKeys(dbconn, [[runNumber, runNumber]])[runNumber]['HLTPSK2']})
        configMetadata.append({'LVL1PSK' : TriggerCoolUtil.getL1ConfigKeys(dbconn, [[runNumber, runNumber]])[runNumber]['LVL1PSK']})

    else:
        log.warning("Config keys not found in COOL")

    return configMetadata
