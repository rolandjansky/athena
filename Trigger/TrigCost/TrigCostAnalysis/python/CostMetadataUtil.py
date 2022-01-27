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


def saveMetadata(inputFile, userDetails, processingWarnings=[]):
    ''' @brief Save metadata from ntuple to json file
    '''
    import json

    metatree = inputFile.Get("metadata")
    if metatree is None:
        return None

    metatree.GetEntry(0)
    metadata = []

    metadata.append({'runNumber' : metatree.runNumber})
    metadata.append({'AtlasProject' : str(metatree.AtlasProject)})
    metadata.append({'AtlasVersion' : str(metatree.AtlasVersion)})

    metadata.append({'ChainMonitor' : metatree.ChainMonitor})
    metadata.append({'AlgorithmMonitor' : metatree.AlgorithmMonitor})
    metadata.append({'AlgorithmClassMonitor' : metatree.AlgorithmClassMonitor})
    metadata.append({'ROSMonitor' : metatree.ROSMonitor})
    metadata.append({'GlobalsMonitor' : metatree.GlobalsMonitor})
    metadata.append({'ThreadMonitor' : metatree.ThreadMonitor})

    metadata.append({'AdditionalHashMap' : str(metatree.AdditionalHashMap)})
    metadata.append({'DoEBWeighting' : metatree.DoEBWeighting})
    metadata.append({'BaseEventWeight' : metatree.BaseEventWeight})

    metadata.append({'HLTMenu' : json.loads(str(metatree.HLTMenu))})

    metadata.append({'Details' : userDetails})

    metadata.append({'Histogram under/overflows' : processingWarnings})

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
