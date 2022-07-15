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

from AthenaCommon.Logging import logging
log = logging.getLogger('RatesAnalysisOnlineProcessing')

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
    limmin=(runNumber << 32)+0
    limmax=((runNumber+1) << 32)+0

    from PyCool import cool
    objs = f.browseObjects( limmin, limmax, cool.ChannelSelection(0) )
    counter = 1 # lumiblocks start indexing from 1
    while objs.goToNext() and counter < lb:
        counter+=1

    if counter == lb:
        return objs.currentRef().payload()
    else:
        log.error("Maximum lumiblock for run {0} is {1}".format(runNumber, lb))
        return None


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


def getKeyForLimits(psk, lbStart, lbEnd):
    ''' 
    Retrieve prescale within given lumiblocks
    '''
    # Convert str to list of ntups
    it = iter("".join(c for c in psk if c not in "()[] ").split(","))
    pskList = [(int(x), int(y), int(z)) for x, y, z in zip(it, it, it)]

    log.debug("Reading the key ranges for {0}".format(psk))

    # COOL format of prescale entry is (key, lbStart, lbEnd)
    foundKeys = []
    for prescaleEntry in pskList:
        if lbStart >= prescaleEntry[1] and lbEnd <= prescaleEntry[2]:
            log.info("Found prescale key in the range: {0}".format(prescaleEntry))
            foundKeys.append(prescaleEntry[0])

    if not foundKeys:
        log.error("Cannot find one prescale for lumiblocks {0} to {1}. Available values: {2}".format(lbStart, lbEnd, pskList))
        return -1
    elif len(foundKeys) > 1:
        log.error("Found more than one prescale key! {0}".format(pskList))
        return -1

    return foundKeys[0]


def toCSV(fileName, dirName, data):
    ''' 
    Save rates to csv file
    '''
    import csv, os

    os.makedirs(dirName, exist_ok=True)
    with open(dirName + '/' + fileName, mode='w') as outputCSV_file:
        rates_csv_writer = csv.writer(outputCSV_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        rates_csv_writer.writerow(['Name','Group','Rate [Hz]','Rate Err [Hz]', 'Prescale'])
        rates_csv_writer.writerow(['Trigger name','The group this chain belongs to','Online rate','Error on rate','The prescale of this chain. Only displayed for simple combinations'])

        for trig in data:
            rates_csv_writer.writerow(trig)
    

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
    # TODO save metadata to metadata.json

    hltPrescales = readHLTPrescales(getKeyForLimits(configMetadata[4]["HLTPSK"], args.lbStart, args.lbEnd), configMetadata[2]["DB"])
    l1Prescales = readL1Prescales(getKeyForLimits(configMetadata[5]["LVL1PSK"], args.lbStart, args.lbEnd), configMetadata[2]["DB"])
    chainGroups = readChainsGroups(configMetadata[1]["SMK"], configMetadata[2]["DB"])

    # Read prescaled IS rates from pbeast and calculate unprescaled rates
    # Queries are based on TRP grafana dashboard
    try:
        import libpbeastpy
        pbeast = libpbeastpy.ServerProxy(args.server)
        hltRates =  pbeast.get_data('ATLAS', 'HLT_Rate', 'Output', 'ISS_TRP.HLT_.*', True, startOfRange, endOfRange)[0].data
        l1Rates =  pbeast.get_data('ATLAS', 'L1_Rate', 'TAV', 'ISS_TRP.L1_.*', True, startOfRange, endOfRange)[0].data
        streamRates = pbeast.get_data('ATLAS', 'HLT_Rate', 'Output', 'ISS_TRP.str_.*', True, startOfRange, endOfRange)[0].data
        groupRates = pbeast.get_data('ATLAS', 'HLT_Rate', 'Output', 'ISS_TRP.grp_.*', True, startOfRange, endOfRange)[0].data
        recordingRates = pbeast.get_data('ATLAS', 'SFOngCounters', 'WritingEventRate', 'DF.TopMIG-IS:HLT.Counters.*', True, startOfRange, endOfRange)[0].data
    except ImportError:
        log.error("Exeption when reading the pbeast information. Remember to setup the tdaq release!")
        return
    except RuntimeError:
        log.error("Exception when reading the pbeast information. Remeber to export the pbeast server sso!")
        return

    HLTChains = []
    HLTChainsUnps = []
    for chain in hltRates:
        chainName = chain.replace("ISS_TRP.", "")
        if chainName not in chainGroups:
            log.debug("Chain {0} is missing from the current menu".format(chainName))
            continue

        meanRate = 0
        meanCounter = 0
        for dataPoint in hltRates[chain]:
            meanRate += dataPoint.value
            meanCounter += 1 if dataPoint.value > 0 else 0

        l1Item = "L1_" + chainName.split("L1")[1]
        l1psk = l1Prescales[l1Item] if l1Item in l1Prescales else "-"
        rate = meanRate/meanCounter if meanCounter > 0 else 0
        rateUn = rate * hltPrescales[chainName] * (l1Prescales[l1Item] if l1Item in l1Prescales else 1)

        HLTChains.append([chainName, chainGroups[chainName],  round(rate, 3), "0", "L1:{0} HLT:{1}".format(l1psk, hltPrescales[chainName])])
        HLTChainsUnps.append([chainName, chainGroups[chainName], round(rateUn, 3), "0", "L1:{0} HLT:{1}".format(l1psk, hltPrescales[chainName])])

    L1Items = []
    L1ItemsUnps = []
    for item in l1Rates:
        
        meanRate = 0
        meanCounter = 0
        for dataPoint in l1Rates[item]:
            meanRate += dataPoint.value
            meanCounter += 1 if dataPoint.value > 0 else 0

        itemName = item.replace("ISS_TRP.", "")
        if "--enabled" not in itemName:
            continue
        else:
            itemName = itemName.replace("--enabled", "")
        rate = meanRate/meanCounter if meanCounter > 0 else 0
        rateUn = rate * l1Prescales[itemName]

        L1Items.append([itemName, "-", round(rate, 3), "0", "L1:{0}".format(l1Prescales[itemName])])
        L1Items.append([itemName, "-", round(rateUn, 3), "0", "L1:{0}".format(l1Prescales[itemName])])

    groups = []
    groupRates = groupRates | streamRates | recordingRates
    for group in groupRates:
        meanRate = 0
        for dataPoint in groupRates[group]:
            meanRate += dataPoint.value
        groupName = group.replace("ISS_TRP.", "") \
                         .replace("grp", "RATE") \
                         .replace("str", "STREAM") \
                         .replace("DF.TopMIG-IS:HLT.Counters.", "HLT_recording_")
        groups.append([groupName, "-", round(meanRate/len(groupRates[group]), 3), "0", "Multiple"])

    # Save the results
    from RatesAnalysis.Util import getTableName
    L1Table = getTableName("L1")
    HLTTable = getTableName("HLT")
    GroupTable = getTableName("Group")


    prescaledDirName = "costMonitoring_OnlineTRPRates-onlinePS-LB{0}-{1}_{2}/csv/".format(args.lbStart, args.lbEnd, args.runNumber)
    unprescaledDirName = "costMonitoring_OnlineTRPRates-noPS-LB{0}-{1}_{2}/csv/".format(args.lbStart, args.lbEnd, args.runNumber)
    log.info("Exporting " + HLTTable)
    toCSV(HLTTable, prescaledDirName, HLTChains)
    toCSV(HLTTable, unprescaledDirName, HLTChainsUnps)
    log.info("Exporting " + L1Table)
    toCSV(L1Table, prescaledDirName, L1Items)
    toCSV(L1Table, unprescaledDirName, L1ItemsUnps)
    log.info("Exporting " + GroupTable)
    toCSV(GroupTable, prescaledDirName, groups)


if __name__== "__main__":
    main()
