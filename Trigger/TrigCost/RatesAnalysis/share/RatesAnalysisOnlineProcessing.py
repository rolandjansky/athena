#!/usr/bin/env python
#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''
@file RatesAnalysisOnlineProcessing.py
@brief Script to create summaries with online P1 rates retrieved from pbeast, prescales and unprescaled.
       Used for validation of Enhanced Bias weighting.
       For the pbeast authentication use: export PBEAST_SERVER_SSO_SETUP_TYPE=AutoUpdateKerberos
'''
from DQUtils.sugar import RunLumi

from AthenaCommon.Logging import logging
log = logging.getLogger('RatesAnalysisOnlineProcessing')

# Class representing entry in the csv table summary
class RateEntry:

    def __init__(self, name, group, prescale):
        self.name = name
        self.group = group
        self.rateSum = 0
        self.unprescaledRateSum = 0
        self.prescale = prescale
        self.counter = 0

    # Add to the rates' sums rate and unprescaled rate values
    def appendRate(self, rate, unprescaledRate):
        self.rateSum += rate
        self.unprescaledRateSum += unprescaledRate
        self.counter += 1

    # Add used pair of keys for unprescaled rate calculation
    def appendKeys(self, prescaleKeys):
        self.prescale += "," + prescaleKeys

    def getAvgRate(self):
        return self.rateSum/self.counter if self.counter > 0 else 0

    def getAvgUnprescaledRate(self):
        return self.unprescaledRateSum/self.counter if self.counter > 0 else 0

    def getCsvEntry(self):
        #['Name','Group','Rate [Hz]','Rate Err [Hz]', 'Prescale']
        return [self.name, self.group, round(self.getAvgRate(), 3), 0, self.prescale]

    def getUnprescaledCsvEntry(self):
        return [self.name, self.group, round(self.getAvgUnprescaledRate(), 3), 0, self.prescale]


def readLbStartFromCool(runNumber, lb):
    ''' 
    Returns start of the lumiblock from given run in microseconds
    '''
    timestamp = readLbFromCool(runNumber, lb)["StartTime"]

    # Returns value UTC nanoseconds since 1970  - convert to microseconds (used by IS)
    return int(timestamp/1000) if timestamp else -1


def readLbEndFromCool(runNumber, lb):
    ''' 
    Returns end of the lumiblock from given run in microseconds
    '''
    timestamp = readLbFromCool(runNumber, lb)["EndTime"]

    # Returns value UTC nanoseconds since 1970  - convert to microseconds (used by IS)
    return int(timestamp/1000) if timestamp else -1


def readLbFromCool(runNumber, lb):
    ''' 
    Returns payload for a lumiblock from given run read from COOL database
    Based on TriggerCoolUtil.getRunStartTime
    '''
    from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
    dbconn = TriggerCoolUtil.GetConnection("CONDBR2")
    f = dbconn.getFolder( "/TRIGGER/LUMI/LBLB" )

    limmin = RunLumi(runNumber, lb)
    limmax = RunLumi(runNumber, lb+1)

    from PyCool import cool
    objs = f.browseObjects(limmin, limmax, cool.ChannelSelection(0))
    objs.goToNext()
    return objs.currentRef().payload()


def readChainsGroups(smkey="", dbAlias=""):
    ''' 
    Returns dictionary of chain and it's groups based on HLT Menu
    '''
    log.debug("Reading HLT menu from {0} {1}".format(smkey, dbAlias))
    from TrigConfIO.HLTTriggerConfigAccess import HLTMenuAccess
    hltChains = HLTMenuAccess(dbalias = dbAlias, smkey = smkey).chains()

    groups = {}
    for chainName in hltChains:
        groups[chainName] = " ".join(hltChains[chainName]["groups"])

    return groups


def readHLTPrescales(pskey="", dbAlias=""):
    ''' 
    Returns dictionary of chain and it's prescale
    '''
    log.debug("Reading HLT prescales set from {0} {1}".format(pskey, dbAlias))
    from TrigConfIO.HLTTriggerConfigAccess import HLTPrescalesSetAccess
    prescalesSet = HLTPrescalesSetAccess(dbalias = dbAlias, hltpskey = pskey).prescales()

    prescales = {}
    for chainName in prescalesSet:
        prescales[chainName] = prescalesSet[chainName]["prescale"]

    return prescales


def readL1Prescales(pskey="", dbAlias=""):
    ''' 
    Returns dictionary of item and it's prescale
    '''
    log.debug("Reading L1 prescales set from {0} {1}".format(pskey, dbAlias))
    from TrigConfIO.L1TriggerConfigAccess import L1PrescalesSetAccess
    prescalesAccess = L1PrescalesSetAccess(dbalias = dbAlias, l1pskey = pskey)

    prescales = {}
    for itemName in prescalesAccess.itemNames():
        prescales[itemName] = prescalesAccess.prescale(itemName)

    return prescales


def findKeysForRange(psk, lbStart, lbEnd):
    ''' 
    Retrieve all prescale keys combinations within given lumiblocks
    '''
    # Convert str to list of ntups
    it = iter("".join(c for c in psk if c not in "()[] ").split(","))
    pskList = [(int(x), int(y), int(z)) for x, y, z in zip(it, it, it)]

    log.debug("Reading the key ranges for {0}".format(psk))

    # COOL format of prescale entry is (key, lbStart, lbEnd)
    foundKeys = []
    for prescaleEntry in pskList:
        if prescaleEntry[1] <= lbEnd and prescaleEntry[2] >= lbStart:
            log.debug("Found prescale key in the range: {0}".format(prescaleEntry))
            foundKeys.append(prescaleEntry)

    if not foundKeys:
        log.warning("Cannot find one prescale for lumiblocks {0} to {1}. Available values: {2}".format(lbStart, lbEnd, pskList))
        return []

    return foundKeys


def toCSV(fileName, dirName, data):
    ''' 
    Save rates to csv file
    '''
    import csv, os

    os.makedirs(dirName, exist_ok=True)
    with open(dirName + '/' + fileName, mode='w') as outputFile:
        ratesWriter = csv.writer(outputFile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        ratesWriter.writerow(['Name','Group','Rate [Hz]','Rate Err [Hz]', 'Prescale'])
        ratesWriter.writerow(['Trigger name','The group this chain belongs to','Online rate','Error on rate','The prescale of this chain. Only displayed for simple combinations'])

        for trig in data:
            ratesWriter.writerow(trig)


def saveMetadata(dirName, data):
    ''' 
    Save metadata dictionary
    '''

    mdDict = {}
    mdDict['text'] = 'metadata'
    mdDict['children'] = data

    outFilename = dirName + '/' + "metadata.json"
    with open(outFilename, mode='w') as outputFile:
        import json
        json.dump(obj=mdDict, fp=outputFile, indent=2, sort_keys=True)


def readAvgLumi(runNumber, lbStart, lbEnd):
    ''' 
    Read Inst Lumi and calculate the average for given run number and lumiblock ranges
    '''
    from PyCool import cool
    from DQUtils.sugar import RunLumi
    from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
    db = TriggerCoolUtil.GetConnection('CONDBR2')
    folder = db.getFolder("/TRIGGER/LUMI/OnlPrefLumi")
    folderIterator = folder.browseObjects(RunLumi(runNumber, lbStart), RunLumi(runNumber, lbEnd), cool.ChannelSelection())    

    avg = 0
    counter = 0
    while folderIterator.goToNext():
        payload=folderIterator.currentRef().payload()
        avg += payload["LBAvInstLumi"]
        counter +=1

    if counter == 0:
        log.error("No entries in COOL database, in /TRIGGER/LUMI/OnlPrefLumi folder were found for run {0}, lumiblocks {1}-{2}".format(runNumber, lbStart, lbEnd))
        return 0

    log.debug("Avg inst lumi {0} for {1} events".format((avg/counter), counter))

    return avg/counter * 1e30

def main():
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--runNumber', required=True, type=int, 
                        help='Number of run to process')
    parser.add_argument('--lbStart', required=True, type=int, 
                        help='First lumiblock to record Enhanced Bias data')
    parser.add_argument('--lbEnd', required=True, type=int, 
                        help='Last lumiblock to record Enhanced Bias data')
    parser.add_argument('-s','--server', default='https://atlasop.cern.ch',
                        help="Pbeast server url. For GPN: https://atlasop.cern.ch, for P1: http://pc-tdq-bst-05.cern.ch:8080")
    parser.add_argument('--loglevel', type=int, default=3, 
                        help='Verbosity level: 1 - VERBOSE, 2 - DEBUG, 3 - INFO')
    args = parser.parse_args()
    log.setLevel(args.loglevel)

    startOfRange = readLbStartFromCool(args.runNumber, args.lbStart)
    endOfRange = readLbEndFromCool(args.runNumber, args.lbEnd)

    from time import ctime
    log.info("Rates will be retrieved for lumiblocks {0}-{1}: {2} - {3}".format(args.lbStart, args.lbEnd, ctime(startOfRange/1E6), ctime(endOfRange/1E6)))
    log.debug("Start and end timestamps: {0} {1}".format(startOfRange, endOfRange))

    # Read prescales and groups to save in the csv summary
    from TrigCostAnalysis.CostMetadataUtil import readHLTConfigKeysFromCOOL
    configMetadata = readHLTConfigKeysFromCOOL(args.runNumber)

    # The start and stop lumiblock should not include the lumiblocks where we did the keys change
    hltAvailableKeys = findKeysForRange(configMetadata[4]["HLTPSK"], args.lbStart, args.lbEnd)
    l1AvailableKeys = findKeysForRange(configMetadata[5]["LVL1PSK"], args.lbStart, args.lbEnd)

    # Create list of key ranges - it will store tuples (l1Psk, hltPsk, rangeStart, rangeEnd)
    # Four cases are handled: L1 range starts before HLT range and ends later, HLT range starts before L1 range and ends later, 
    #       L1 range is withing HLT range, and HLT range is within L1 range
    keyRangesList = []
    for hltEntry in hltAvailableKeys:
        hltLbStart = hltEntry[1]
        hltLbEnd = hltEntry[2]

        for l1Entry in l1AvailableKeys:
            l1LbStart = l1Entry[1]
            l1LbEnd = l1Entry[2]

            if (hltLbStart >= l1LbStart) and (hltLbEnd >= l1LbEnd) and (hltLbStart <= l1LbEnd):
                keyRangesList.append((l1Entry[0], hltEntry[0], hltLbStart, l1LbEnd))
            elif (hltLbStart <= l1LbStart) and (hltLbEnd <= l1LbEnd) and (l1LbStart <= hltLbEnd):
                keyRangesList.append((l1Entry[0], hltEntry[0], l1LbStart, hltLbEnd))
            elif (hltLbStart <= l1LbStart) and (hltLbEnd >= l1LbEnd):
                keyRangesList.append((l1Entry[0], hltEntry[0], l1LbStart, l1LbEnd))
            elif (hltLbStart >= l1LbStart) and (hltLbEnd <= l1LbEnd):
                keyRangesList.append((l1Entry[0], hltEntry[0], hltLbStart, hltLbEnd))

    log.debug("Available key ranges are {0}".format(keyRangesList))
    chainGroups = readChainsGroups(configMetadata[1]["SMK"], configMetadata[2]["DB"])

    pbeast = None
    try:
        import libpbeastpy
        pbeast = libpbeastpy.ServerProxy(args.server)
    except ImportError as e:
        log.error("Exeption when reading the pbeast information. Remember to setup the tdaq release!\n{0}".format(e))
        return

    hltChains = {}
    l1Items = {}
    groups = {}
    for keysRange in keyRangesList:
        # Save the range start and end and make sure we are in given limits
        lbStart = keysRange[2] if keysRange[2] > args.lbStart else args.lbStart
        lbEnd = keysRange[3] if keysRange[3] < args.lbEnd else args.lbEnd
        startOfKeysRange = readLbStartFromCool(args.runNumber, lbStart)
        endOfKeysRange = readLbEndFromCool(args.runNumber, lbEnd)
        log.debug("Current range is {0}-{1}. Timestamps are {2}-{3}".format(lbStart, lbEnd, ctime(startOfKeysRange/1E6), ctime(endOfKeysRange/1E6)))

        hltPrescales = readHLTPrescales(keysRange[1], configMetadata[2]["DB"])
        l1Prescales = readL1Prescales(keysRange[0], configMetadata[2]["DB"])

        # Read prescaled IS rates from pbeast and calculate unprescaled rates
        # Queries are based on TRP grafana dashboard
        try:
            hltRates =  pbeast.get_data('ATLAS', 'HLT_Rate', 'Output', 'ISS_TRP.HLT_.*', True, startOfKeysRange, endOfKeysRange)[0].data
            l1Rates =  pbeast.get_data('ATLAS', 'L1_Rate', 'TAV', 'ISS_TRP.L1_.*', True, startOfKeysRange, endOfKeysRange)[0].data
            streamRates = pbeast.get_data('ATLAS', 'HLT_Rate', 'Output', 'ISS_TRP.str_.*', True, startOfKeysRange, endOfKeysRange)[0].data
            groupRates = pbeast.get_data('ATLAS', 'HLT_Rate', 'Output', 'ISS_TRP.grp_.*', True, startOfKeysRange, endOfKeysRange)[0].data
            recordingRates = pbeast.get_data('ATLAS', 'SFOngCounters', 'WritingEventRate', 'DF.TopMIG-IS:HLT.Counters.*', True, startOfKeysRange, endOfKeysRange)[0].data
        except RuntimeError as e:
            log.error("Exception when reading the pbeast information. Remember to export the pbeast server sso!\n{0}".format(e))
            return

        for chain in hltRates:
            chainName = chain.replace("ISS_TRP.", "")
            if chainName not in chainGroups:
                log.warning("Chain {0} is missing from the current menu".format(chainName))
                continue

            l1Item = "L1_" + chainName.split("L1")[1]
            l1psk = l1Prescales[l1Item] if l1Item in l1Prescales else "-"
            keysStr = "L1:{0} HLT:{1}".format(l1psk, hltPrescales[chainName])
            if chainName not in hltChains:
                hltChains[chainName] = RateEntry(chainName, chainGroups[chainName], keysStr)
            else:
                hltChains[chainName].appendKeys(keysStr)
            
            for dataPoint in hltRates[chain]:
                if dataPoint.value == 0:
                    # Skip avg rate = 0
                    continue
                elif dataPoint.ts < startOfKeysRange or dataPoint.ts > endOfKeysRange:
                    # Skip data point outside analyzed range
                    continue

                rate = dataPoint.value
                if rate > 0 and hltPrescales[chainName] <=0:
                    log.warning("Rate for disabled chain {0} is higher than 0! {1} timestamp {2}".format(chainName, rate, ctime(dataPoint.ts/1E6)))

                rateUn = rate * hltPrescales[chainName] * (l1Prescales[l1Item] if l1Item in l1Prescales else 1)
                hltChains[chainName].appendRate(rate, rateUn)

        for item in l1Rates:
            itemName = item.replace("ISS_TRP.", "")
            if "--enabled" not in itemName:
                continue
            else:
                itemName = itemName.replace("--enabled", "")

            if itemName not in l1Items:
                l1Items[itemName] = RateEntry(itemName, "-", "L1:{0}".format(l1Prescales[itemName]))
            else:
                l1Items[itemName].appendKeys("L1:{0}".format(l1Prescales[itemName]))

            for dataPoint in l1Rates[item]:
                if dataPoint.value == 0:
                    continue
                elif dataPoint.ts < startOfKeysRange or dataPoint.ts > endOfKeysRange:
                    continue
                rate = dataPoint.value
                if rate > 0 and l1Prescales[itemName] <=0:
                    log.warning("Rate for disabled chain {0} is higher than 0! {1} timestamp {2}".format(chainName, rate, ctime(dataPoint.ts/1E6)))
                rateUn = rate * l1Prescales[itemName]

                l1Items[itemName].appendRate(rate, rateUn)

        groupRates = groupRates | streamRates | recordingRates
        for group in groupRates:
            groupName = group.replace("ISS_TRP.", "") \
                            .replace("grp", "RATE") \
                            .replace("str", "STREAM") \
                            .replace("DF.TopMIG-IS:HLT.Counters.", "HLT_recording_")     

            if groupName not in groups:    
                groups[groupName] = RateEntry(groupName, "-", "Multiple")

            for dataPoint in groupRates[group]:
                if dataPoint.value == 0:
                    continue
                elif dataPoint.ts < startOfKeysRange or dataPoint.ts > endOfKeysRange:
                    continue
                groups[groupName].appendRate(dataPoint.value, 0)

    # Save the results
    from RatesAnalysis.Util import getTableName
    L1Table = getTableName("L1")
    HLTTable = getTableName("HLT")
    GroupTable = getTableName("Group")

    metadata = [
        {'PredictionLumi' : readAvgLumi(args.runNumber, args.lbStart, args.lbEnd)},
        {'RunNumber' : args.runNumber},
        {"First lumiblock" : args.lbStart},
        {"Last lumiblock" : args.lbEnd},
        {'SMK' :  configMetadata[1]["SMK"]},
        {'DB' : configMetadata[2]["DB"]},
        {'LVL1PSK' :  l1AvailableKeys},
        {'HLTPSK' : hltAvailableKeys}
    ]

    prescaledDirName = "costMonitoring_OnlineTRPRates-onlinePS-LB{0}-{1}_{2}/".format(args.lbStart, args.lbEnd, args.runNumber)
    unprescaledDirName = "costMonitoring_OnlineTRPRates-noPS-LB{0}-{1}_{2}/".format(args.lbStart, args.lbEnd, args.runNumber)
    log.info("Exporting " + HLTTable)
    hltChainsList = [hltChains[chain].getCsvEntry() for chain in hltChains]
    hltChainsUnpsList = [hltChains[chain].getUnprescaledCsvEntry() for chain in hltChains]
    toCSV(HLTTable, prescaledDirName + "csv/", hltChainsList)
    toCSV(HLTTable, unprescaledDirName + "csv/", hltChainsUnpsList)

    log.info("Exporting " + L1Table)
    l1ItemsList = [l1Items[item].getCsvEntry() for item in l1Items]
    l1ItemsUnpsList = [l1Items[item].getUnprescaledCsvEntry() for item in l1Items]
    toCSV(L1Table, prescaledDirName + "csv/", l1ItemsList)
    toCSV(L1Table, unprescaledDirName + "csv/", l1ItemsUnpsList)

    log.info("Exporting " + GroupTable)
    groupsList = [groups[group].getCsvEntry() for group in groups]
    toCSV(GroupTable, prescaledDirName + "csv/", groupsList)

    prescaledMd = [*metadata, {"Details" : "Averaged rates with online prescales from online monitoring"}]
    unprescaledMd = [*metadata, {"Details" : "Averaged rates with prescales removed from online monitoring"}]
    saveMetadata(prescaledDirName, prescaledMd)
    saveMetadata(unprescaledDirName, unprescaledMd)


if __name__== "__main__":
    main()
