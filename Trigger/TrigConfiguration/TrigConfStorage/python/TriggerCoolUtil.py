# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import copy, sys
import subprocess
from re import match
from time import ctime
try:
    Set = set
except NameError:
    from sets import Set

from PyCool import cool

from CoolConvUtilities.AtlCoolLib import indirectOpen

from TrigConfStorage.CompL1Menu import CompareL1XML
from TrigConfStorage.CompHLTMenu import CompareHLTXML

class TriggerCoolUtil:

    @staticmethod
    def diffL1Menu(xml1fn,xml2fn, **args):
        verbose=False
        if 'verbose' in args:
            verbose=args['verbose']
        if verbose:
            print "CompareL1XML([%s, %s], verbose=True)" % (xml1fn,xml2fn)
        comparer = CompareL1XML([xml1fn, xml2fn], verbose=verbose)
        comparer.diff()

    @staticmethod
    def diffHLTMenu(xml1fn, xml2fn, **args):
        verbose=False
        if 'verbose' in args:
            verbose=args['verbose']
        if verbose:
            print "CompareHLTXML([%s, %s], verbose=True)" % (xml1fn,xml2fn)
        comparer = CompareHLTXML([xml1fn, xml2fn], verbose=verbose)
        comparer.diff()

    @staticmethod
    def MergeRanges(listOfRanges):
        newRR=[]
        for rr in listOfRanges:
            if len(newRR)==0:
                newRR.append(copy.deepcopy(rr))
            else:
                if rr[0]-1<=newRR[-1][1]:
                    newRR[-1][1] = max(rr[1],newRR[-1][1])
                else:
                    newRR.append(copy.deepcopy(rr))
        return newRR

    @staticmethod
    def GetRunRanges(runRange):
        listOfRanges = []
        runRanges = runRange.split(',')
        for rr in runRanges:
            startend = rr.split('-')
            if len(startend)==1: startend += [startend[0]]
            firstlast = [0, (cool.ValidityKeyMax>>32)-1]
            if startend[0]: firstlast[0] = int(startend[0])
            if startend[1]: firstlast[1]  = int(startend[1])
            firstlast.sort()
            listOfRanges += [firstlast]
        listOfRanges.sort()
        return TriggerCoolUtil.MergeRanges(listOfRanges)

    @staticmethod
    def GetConnection(dbconn,verbosity=0):
        connection = None
        m = match(".*?([^/.]+)\.db",dbconn)
        if dbconn=="COMP":
            connection = 'COOLONL_TRIGGER/COMP200'
        elif dbconn=="OFLP":
            connection = 'COOLONL_TRIGGER/OFLP200'
        elif dbconn=="CONDBR2":
            connection = 'COOLONL_TRIGGER/CONDBR2'
        elif m:
            dbname=m.group(1).upper()
            connection = "sqlite://;schema=%s;dbname=%s;" % (dbconn,dbname)
        else:
            raise RuntimeError, "Can't connect to COOL db %s" % dbconn
        try:
            openConn = indirectOpen(connection,readOnly=True,oracle=True,debug=(verbosity>0))
        except Exception, e:
            print e
            sys.exit(-1)
        return openConn

    @staticmethod
    def getHLTConfigKeys(db,runlist):
        configKeys = {}
        f = db.getFolder( "/TRIGGER/HLT/HltConfigKeys" )
        for rr in runlist:
            limmin=(rr[0] << 32)+0
            limmax=((rr[1]+1) << 32)-1
            objs = f.browseObjects( limmin, limmax, cool.ChannelSelection(0))
            while objs.goToNext():
                obj=objs.currentRef()
                runNr   = obj.since()>>32
                if runNr>1000000: continue
                payload=obj.payload()
                smk     = payload['MasterConfigurationKey']
                hltpsk  = payload['HltPrescaleConfigurationKey']
                confsrc = payload['ConfigSource'].split(',')
                release = 'unknown'
                if len(confsrc)>1: release = confsrc[1]
                configKeys[runNr] = { "REL"     : release,
                                      "SMK"     : smk,
                                      "HLTPSK"  : hltpsk }
        return configKeys

    @staticmethod
    def getHLTPrescaleKeys(db,runlist):
        lbmask = 0xFFFFFFFF
        configKeys = {}
        f = db.getFolder( "/TRIGGER/HLT/PrescaleKey" )
        for rr in runlist:
            limmin=(rr[0] << 32)+0
            limmax=((rr[1]+1) << 32)-1
            objs = f.browseObjects( limmin, limmax, cool.ChannelSelection(0))
            while objs.goToNext():
                obj=objs.currentRef()
                runNr   = obj.since()>>32
                if runNr>1000000: continue
                payload = obj.payload()
                hltpsk  = payload['HltPrescaleKey']
                firstLB = obj.since() & lbmask
                lastLB  = (obj.until() & lbmask) -1
                configKeys.setdefault(runNr,{}).setdefault( "HLTPSK2", [] ).append((hltpsk,firstLB,lastLB))
        return configKeys

    @staticmethod
    def getL1ConfigKeys(db,runlist):
        lbmask = 0xFFFFFFFF
        configKeys = {}
        f = db.getFolder( "/TRIGGER/LVL1/Lvl1ConfigKey" )
        for rr in runlist:
            limmin=(rr[0] << 32)+0
            limmax=((rr[1]+1) << 32)-1
            objs = f.browseObjects( limmin, limmax, cool.ChannelSelection(0))
            while objs.goToNext():
                obj=objs.currentRef()
                runNr  = obj.since()>>32
                if runNr>1000000: continue
                payload=obj.payload()
                firstLB = obj.since() & lbmask
                lastLB =  (obj.until() & lbmask) -1
                l1psk = payload['Lvl1PrescaleConfigurationKey']
                configKeys.setdefault(runNr,{}).setdefault( "LVL1PSK", [] ).append((l1psk,firstLB,lastLB))
        return configKeys
    
    @staticmethod
    def getBunchGroupKey(db,runlist):
        lbmask = 0xFFFFFFFF
        configKeys = {}
        f = db.getFolder( "/TRIGGER/LVL1/BunchGroupKey" )
        for rr in runlist:
            limmin=(rr[0] << 32)+0
            limmax=((rr[1]+1) << 32)+0
            objs = f.browseObjects( limmin, limmax, cool.ChannelSelection(0))
            while objs.goToNext():
                obj=objs.currentRef()
                runNr  = obj.since()>>32
                if runNr>1000000: continue
                payload=obj.payload()
                firstLB = obj.since() & lbmask
                lastLB =  (obj.until() & lbmask) -1
                bgkey = payload['Lvl1BunchGroupConfigurationKey']
                configKeys.setdefault(runNr,{}).setdefault( "BGKey", [] ).append((bgkey,firstLB,lastLB))
        return configKeys

    @staticmethod
    def merge(l1, hlt):
        split = list(Set([b[1] for b in l1] + [b[1] for b in hlt]))
        split.sort()
        c = []
        print "merging:", split
        for i,startlb in enumerate(split):
            if i<len(split)-1:
                endlb = split[i+1]-1
            else:
                endlb = -1
            for k,s,e in l1:
                if startlb>=s:
                    l1key = k
            for k,s,e in hlt:
                if startlb>=s:
                    hltkey = k
            c.append((startlb,endlb,l1key,hltkey))
        return c

    @staticmethod
    def combinedKeys(l1keys, hltkeys):
        runs = Set(l1keys.keys())
        runs.update( Set(hltkeys.keys()) )
        comb={}
        for r in runs:
            if r in l1keys:  l1  = l1keys[r]['LVL1PSK']
            else:            l1  = [(0, 1, -1)]
            if r in hltkeys: hlt = hltkeys[r]['HLTPSK2']
            else:            hlt = [(0, 1, -1)]
            comb[r] = TriggerCoolUtil.merge(l1,hlt)
        return comb
    

    @staticmethod
    def getRunStartTime(db,runlist, runs):
        latestRunNr=0
        startTime = {}
        f = db.getFolder( "/TRIGGER/LUMI/LBLB" )
        for rr in runlist:
            limmin=(rr[0] << 32)+0
            limmax=((rr[1]+1) << 32)+0
            objs = f.browseObjects( limmin, limmax, cool.ChannelSelection(0) )
            while objs.goToNext():
                obj=objs.currentRef()
                runNr  = obj.since()>>32
                if runNr==latestRunNr: continue
                latestRunNr=runNr
                if not runNr in runs: continue
                payload=obj.payload()
                starttime = payload['StartTime']
                startTime[runNr] = { "STARTTIME" : ctime(starttime/1E9).replace(' ','_') }
        return startTime


    @staticmethod
    def printL1Menu(db, run, verbosity):
        limmin=run<<32
        limmax=(run+1)<<32

        printPrescales = verbosity>0
        printDisabled = verbosity>1
        print "LVL1 Menu:"
        f = db.getFolder( "/TRIGGER/LVL1/Menu" )
        chansel=cool.ChannelSelection.all()
        objs = f.browseObjects( limmin,limmax,chansel)
        fps = db.getFolder( "/TRIGGER/LVL1/Prescales" )
        objsps = fps.browseObjects( limmin,limmax,chansel)
        itemName = {}
        itemPrescale = {}
        longestName = 0
        while objs.goToNext():
            obj=objs.currentRef()
            channel = obj.channelId()
            payload=obj.payload()
            itemname = payload['ItemName']
            itemName[channel] = itemname
            longestName=max(longestName,len(itemname))
            while objsps.goToNext():
                objps=objsps.currentRef()
                channel = objps.channelId()
                payloadps=objps.payload()
                ps = payloadps['Lvl1Prescale']
                if channel in itemPrescale:
                    itemPrescale[channel] += [ ps ]
                else:
                    itemPrescale[channel] = [ ps ]
        for channel in itemName:
            doPrint = False
            for x in itemPrescale[channel]:
                if x>0 or printDisabled: doPrint = True
            if not doPrint: continue
            if printPrescales:
                print "%4i: %-*s PS " % (channel, longestName, itemName[channel]), itemPrescale[channel]
            else:
                print "%4i: %s" % (channel, itemName[channel])
                

    @staticmethod
    def printHLTMenu(db, run, verbosity, printL2=True, printEF=True):
        limmin=run<<32
        limmax=((run+1)<<32)-1
        print "HLT Menu:"
        f = db.getFolder( "/TRIGGER/HLT/Menu" )
        chansel=cool.ChannelSelection.all()
        objs = f.browseObjects( limmin,limmax,chansel)
        sizeName=0
        sizePS=0
        sizePT=0
        sizeStr=0
        sizeLow=0
        chainNames = {}
        chainExtraInfo = {}
        while objs.goToNext():
            obj=objs.currentRef()
            payload=obj.payload()
            level    = payload['TriggerLevel']
            if level=='L2' and not printL2: continue
            if level=='EF' and not printEF: continue
            name     = payload['ChainName']
            sizeName=max(sizeName,len(name))
            counter  = payload['ChainCounter']
            chainNames[(level,counter)] = name
            if verbosity>0:
                version  = payload['ChainVersion']
                prescale = payload['Prescale']
                passthr  = payload['PassThrough']
                stream   = payload['StreamInfo']
                lower    = payload['LowerChainName']
                sizePS=max(sizePS,prescale)
                sizePT=max(sizePT,passthr)
                sizeStr=max(sizeStr,len(stream))
                sizeLow=max(sizeLow,len(lower))
                chainExtraInfo[(name,level)] = (version, prescale, passthr, stream, lower)
        sizePS = len("%i"%sizePS)
        sizePT = len("%i"%sizePT)
        counters = chainNames.keys()
        counters.sort()
        for c in counters:
            name = chainNames[c]
            print "%s %4i: %-*s" % (c[0], c[1], sizeName, name),
            if verbosity>0:
                (version, prescale, passthr, stream, lower) = chainExtraInfo[(name,c[0])]
                print "[V %1s, PS %*i, PT %*i, by %-*s , => %-*s ]" % \
                      (version, sizePS, prescale, sizePT, passthr, sizeLow, lower, sizeStr, stream),
            print

    @staticmethod
    def printStreams(db, run, verbosity):
        limmin=run<<32
        limmax=((run+1)<<32)-1
        print "Used Streams:"
        f = db.getFolder( "/TRIGGER/HLT/Menu" )
        chansel=cool.ChannelSelection.all()
        objs = f.browseObjects( limmin,limmax,chansel)
        streams = set()
        while objs.goToNext():
            obj=objs.currentRef()
            payload=obj.payload()
            streamsOfChain = payload['StreamInfo']
            for streamprescale in streamsOfChain.split(';'):
                streamname = streamprescale.split(',')[0]
                streams.add(streamname)
        for s in sorted(list(streams)):
            print s

    @staticmethod
    def writeXMLFiles(keys, verbosity):
        base = "%i_%i_%i" % (keys['SMK'],keys['HLTPSK'],keys['LVL1PSK'][0][0])
        cmd  = "TrigConf2XMLApp --trigdb TRIGGERDB"
        cmd += " --outputfile %s" % base
        cmd += " --configkey %i --prescalekeyhlt %i --prescalekeylvl1 %i" % (keys['SMK'],keys['HLTPSK'],keys['LVL1PSK'][0][0])
        if verbosity>0: print cmd
        FNULL = open('/dev/null', 'w')
        returncode = subprocess.call(cmd.split(),stdout=FNULL)
        if returncode==0:
            print "Wrote files L1Menu_%s.xml and HLTMenu_%s.xml" % (base,base)
            return ( "L1Menu_%s.xml" % base, "HLTMenu_%s.xml" % base )
        else:
            return ( None, None )

