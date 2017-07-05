# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file TriggerConfig.py
## @brief Configuration object to be used in python ARA
## @author Joerg Stelzer <stelzer@cern.ch>
## $Id: TriggerConfigARA.py,v 1.12 2009-03-11 15:13:27 stelzer Exp $

import sys
import copy
import ROOT
import cppyy
from PyCool import coral
import AthenaROOTAccess.transientTree

def _iter(self) :
    sequential = self.begin()
    end        = self.end()
    while sequential != end :
        yield sequential.__deref__()
        sequential.__preinc__()
    raise StopIteration

def _chPrint(self):
    s  = "%s (%s %i), ps %g   [streams: " % (self.chain_name(), self.level(), self.chain_counter(), self.prescale())
    if len(self.streamTagList())>0:
        for stag in self.streamTagList():
            s += stag.stream() + "|"
        s = s.rstrip('|')
    else:
        s += "-"
    s+=']'
    return s

def _elemFilter(elem):
    if elem.key().startswith('/TRIGGER/'):
        return elem
    return None


class _TrigConfHolder:
    """helper class to hold all configuration data in one place"""
    def __init__(self):
        self.SMK = -1
        self.HltPSK = -1
        self.L1PSK = -1
        self.L1PS = []
        self.L1Items = {}
        self.HLTChains = {}
        self.ConfigSrc = ""

    def __str__(self):
        s =  "SMK: %i, HLTpsk: %i, L1psk: %i\n" % (self.SMK, self.HltPSK, self.L1PSK)
        s += "Config source: %s\n" % str(self.ConfigSrc)
        s += "# L1 Items   : %i\n" % len(self.L1Items)
        s += "# HLT Chains : %i" % len(self.HLTChains)
        return s


class TriggerConfigARA(object):
    """class which the user should instantiate to get access to the trigger configuration in PyROOT"""

    __cnv = ROOT.TrigConf.TrigConfigARA()
    __keysInUpdateOrder = ['L1K','HLTK','HLTPSK','L1M','HLTM','L1PS']
    __folderName = {
        "L1M"    : "_TRIGGER_LVL1_Menu",
        "HLTM"   : "_TRIGGER_HLT_Menu",
        "L1K"    : "_TRIGGER_LVL1_Lvl1ConfigKey",
        "HLTK"   : "_TRIGGER_HLT_HltConfigKeys",
        "HLTPSK" : "_TRIGGER_HLT_PrescaleKey",
        "L1PS"   : "_TRIGGER_LVL1_Prescales"
        }


    def __init__(self, poolfile, **args):
        """An AOD file pointer (not a string) must be given as argument"""

        self.__dict__.__setitem__("_TriggerConfigARA__curConf", _TrigConfHolder())
        self.__currentIOV = {}
        self.verbose = False
        if 'verbose' in args: self.verbose = args['verbose']

        plcClass = cppyy.makeClass("IOVPayloadContainer")
        plcClass.iter  = _iter

        tiClass = cppyy.makeClass("TrigConf::TriggerItem")
        tiClass.__str__  = lambda x: "%s (ctpid %i), ps %g" % (x.name(), x.ctpId(), x.ps)

        chClass = cppyy.makeClass("TrigConf::HLTChain")
        chClass.__str__  = _chPrint

        ROOT.IOVTime.__str__ = lambda x: "%i/%i" % (x.run(),x.event())
        ROOT.IOVTime.__lt__ = lambda s,o: s.run()<o.run() or s.run()==o.run() and s.event()<o.event()
        ROOT.IOVTime.__eq__ = lambda s,o: s.run()==o.run() and s.event()==o.event()
        ROOT.IOVTime.__le__ = lambda s,o: s<o or s==o
        ROOT.IOVRange.__str__ = lambda x: "%s - %s" % (x.start(),x.stop())
        ROOT.IOVRange.isInRange = lambda x,e: x.start()<=e and e<x.stop()
        
        print "Opening file(s) and creating transient metadata ..."
        if type(poolfile) == list:
            self._collection = ROOT.AthenaROOTAccess.TChainROOTAccess('MetaData')
            for file in poolfile:
                self._collection.Add(file)
            self.mdt = AthenaROOTAccess.transientTree.makeTree(self._collection,
                                                               persTreeName = 'MetaData',
                                                               dhTreeName = 'MetaDataHdr',
                                                               dhfilt = _elemFilter )
        else:
            treeNames = [k.GetName() for k in poolfile.GetListOfKeys()]
            if "MetaDataHdrDataHeader" in treeNames:
                dhTreeName = 'MetaDataHdrDataHeader' # old style
            else:
                dhTreeName = 'MetaDataHdr'
            self.mdt = AthenaROOTAccess.transientTree.makeTree(poolfile,
                                                               persTreeName = 'MetaData',
                                                               dhTreeName = dhTreeName,
                                                               dhfilt = _elemFilter )

        for key in self.__class__.__keysInUpdateOrder:
            if not hasattr(self.mdt,self.__class__.__folderName[key]):
                print "No key %s in file %s, use checkFile.py to verify" % (self.__class__.__folderName[key],poolfile)
                sys.exit(0)





    def printfnc(self,chaindetails=False):
        print self
        if not chaindetails: return
        for ef in self.HLTChains.values():
            if ef.level()!='EF': continue
            if ef.prescale()<0: continue;
            fc = self.printFullChain(ef.chain_name())

    def printFullChain(self, name):
        fc = self.getFullChain(name)
        efstr = "-"
        l2str = "-"
        l1str = "-"
        ststr = ""
        if fc[2]:
            efstr = "%s (%1.2f)" % (fc[2].chain_name(), fc[2].prescale())
            if len(fc[2].streamTagList())>0:
                for stag in fc[2].streamTagList():
                    ststr += stag.stream() + "|"
                ststr = ststr.rstrip('|')
            else:
                ststr += "-"
            ststr+=']'
        if fc[1]: l2str = "%s (%1.2f)" % (fc[1].chain_name(), fc[1].prescale())
        if fc[0]:
            l1str = ""
            if type(fc[0]) == type([]):
                for i in fc[0]:
                    l1str += "%s (%i) " % (i.name(), self.L1PS[i.ctpId()])
            else:
                l1str = "%s (%i)"    % (fc[0].name(), self.L1PS[fc[0].ctpId()])
        print "EF: %s, L2: %s, L1: %s [streams: %s" % (efstr, l2str, l1str, ststr )
        

    def __str__(self):
        s =  "SMK: %i, HLTpsk: %i, L1psk: %i\n" % (self.SMK, self.HltPSK, self.L1PSK)
        s += "Config source: %s\n" % str(self.ConfigSrc)
        s += "L1 Items   : %i\n" % len(self.L1Items)
        s += "HLT Chains : %i" % len(self.HLTChains)
        return s

    def __getattr__(self, key):
        """Forwards getting of unknown attributes to the configuration holder"""
        if key in self.__dict__["_TriggerConfigARA__curConf"].__dict__:
            return self.__dict__["_TriggerConfigARA__curConf"].__dict__.__getitem__(key)
        raise AttributeError, "No configuration parameter %s" % key

    def __setattr__(self, key, val):
        """Forwards setting of unknown attributes to the configuration holder"""
        if key in self.__dict__["_TriggerConfigARA__curConf"].__dict__:
            self.__dict__["_TriggerConfigARA__curConf"].__dict__.__setitem__(key,val)
        else:
            self.__dict__.__setitem__(key, val)

    def printFileSummary(self, chaindetails=False):
        br = self.mdt.GetBranch(self.__class__.__folderName['HLTK'])
        iovs = []
        for i in xrange(self.mdt.GetEntries()):
            br.GetEntry(i)
            metaData = getattr(self.mdt, br.GetName())
            plc = metaData.payloadContainer()
            for payload in plc.iter():
                for j in xrange(payload.size()):
                    chanNum = int(payload.chanNum(j))
                    iovr = payload.iovRange(chanNum)
                    iovs += [iovr]

        print "-----------------------------------------------------------------------"
        for iov in iovs:
            end = iov.stop().event()-1
            if end==-1: end="MAX"
            else: end="%3i" % end
            print "Run: %6i  LBs: %3i - %s" % (iov.start().run(),iov.start().event(),end)
            print "---------------------------"
            currentEvent = ROOT.IOVTime(iov.start().run(), iov.start().event())
            for key in self.__class__.__keysInUpdateOrder:
                success = self.__loadData(key, currentEvent)
            self.printfnc(chaindetails)
            print "-----------------------------------------------------------------------"
        self.__dict__.__setitem__("_TriggerConfigARA__curConf", _TrigConfHolder())
        self.__currentIOV = {}

    def update(self, run, lb):
        """This is the function the user must call to trigger reloading of meta-data from the Tree.
        Data is only reloaded if the IOV goes out of range. The valid IOVs are kept for each folder individually"""
        updated = False
        overallsucc = True
        currentEvent = ROOT.IOVTime(run, lb)
        # check if all the currently loaded folders are still covering the current run/lb
        for key in self.__class__.__keysInUpdateOrder:
            if (key in self.__currentIOV) and self.__currentIOV[key] and self.__currentIOV[key].isInRange(currentEvent):
                continue
            success = self.__loadData(key, currentEvent)
            if not success:
                overallsucc = False
                print "Did not find valid IOV for %s" % self.__class__.__folderName[key]
            updated = True
        if updated:
            if overallsucc:
                print "Loaded new trigger configuration for run/lb = %i/%i" % (run,lb)
            else:
                print "ERROR: Loading of new trigger configuration for run/lb = %i/%i failed" % (run,lb)
        if self.verbose:
            print self
        return updated

    def isConfigured(self, name):
        if name in self.L1Items: return True
        if name in self.HLTChains: return True
        return False

    def getConfiguredChainNames(self):
        nameList = []
        nameList += self.L1Items.keys()
        nameList += self.HLTChains.keys()
        return nameList

    def getFullChain(self, name):
        if not self.isConfigured(name): return None

        fullchain = [None,None,None] # L1,L2,EF

        if name in self.L1Items:
            fullchain[0] = self.L1Items[name]
            return fullchain

        hltchain = self.HLTChains[name]
        if hltchain.level()=='EF':
            fullchain[2] = hltchain
            l2name = hltchain.lower_chain_name()
            if l2name!="":
                if not self.isConfigured(l2name):
                    raise RuntimeError, "Lower chain %s as seed of %s not configured" % (l2name,name)
                fullchain[1] = self.HLTChains[l2name]
        else:
            fullchain[1] = hltchain

        if fullchain[1]:
            l2name = fullchain[1].chain_name()
            l1name = fullchain[1].lower_chain_name()
            if l1name=="":
                pass
            elif ',' in l1name: # multiseeded L2 chain
                fullchain[0] = []
                itemlist = l1name.replace(' ','').split(',')
                for item in itemlist:
                    if not self.isConfigured(item):
                        raise RuntimeError, "L1 item %s as seed of %s not configured" % (str(item),l2name)
                    fullchain[0].append(self.L1Items[item])
            else:
                if not self.isConfigured(l1name):
                    raise RuntimeError, "L1 item %s as seed of %s not configured" % (str(l1name),l2name)
                fullchain[0] = self.L1Items[l1name]

        return fullchain


    def getFullChainPrescaleFactor(self, name):
        if not self.isConfigured(name): return 0
        # if name is a L1Item
        if name in self.L1Items:
            return self.L1PS[self.L1Items[lowname].ctpId()]

        hltchain = self.HLTChains[name]
        prescale = hltchain.prescale()
        lowname  = hltchain.lower_chain_name()
        if lowname=="": return prescale # unseeded
        if not self.isConfigured(lowname):
            raise RuntimeError, "Lower chain %s of %s not configured" % (lowname,name)

        if hltchain.level()=='EF':
            hltchain = self.HLTChains[lowname]
            prescale *= hltchain.prescale()
            lowname  = hltchain.lower_chain_name()
            if lowname=="": return prescale # unseeded
            if not self.isConfigured(lowname):
                raise RuntimeError, "Lower item %s of %s not configured" % (lowname,name)

        # hltchain should be of L2
        prescale *= self.L1PS[self.L1Items[lowname].ctpId()]
        return prescale

    def getL1Items(self):
        return self.L1Items

    def getHLTChains(self):
        return self.HLTChains

    def getConfigKeys(self):
        return (self.SMK, self.L1PSK, self.HltPSK)

    def __loadData(self, key, currentEvent):
        br = self.mdt.GetBranch(self.__class__.__folderName[key])
        validIOV = getattr(self,"_load%s" % key)(br, currentEvent)
        if self.verbose:
            print "Loaded %s with iov %s" % (key, validIOV)
        self.__currentIOV[key] = validIOV
        return validIOV != None

    def _loadHLTK(self, br, currentEvent):
        validIOV = None
        self.SMK = -1
        self.HltPSK = -1
        self.ConfigSrc = ""
        for i in xrange(self.mdt.GetEntries()):
            br.GetEntry(i)
            metaData = getattr(self.mdt, br.GetName())
            plc = metaData.payloadContainer()
            for payload in plc.iter():
                #payload.dump()
                for i in xrange(payload.size()):
                    chanNum = int(payload.chanNum(i))
                    # first check the iov
                    iovr = payload.iovRange(chanNum)
                    if not iovr.isInRange(currentEvent): continue
                    if not validIOV: validIOV = ROOT.IOVRange(iovr)
                    al = payload.attributeList(chanNum)
                    smk = ROOT.Long(0)
                    hltpsk = ROOT.Long(0)
                    configsrc = ROOT.std.string("")
                    self.__class__.__cnv.readHltConfigKeysARA(al,smk,hltpsk,configsrc)
                    self.SMK = smk
                    self.HltPSK = hltpsk
                    self.ConfigSrc = configsrc
        return validIOV


    def _loadHLTPSK(self, br, currentEvent):
        validIOV = None
        self.HltPSK = -1
        for i in xrange(self.mdt.GetEntries()):
            br.GetEntry(i)
            metaData = getattr(self.mdt, br.GetName())
            plc = metaData.payloadContainer()
            for payload in plc.iter():
                #payload.dump()
                for i in xrange(payload.size()):
                    chanNum = int(payload.chanNum(i))
                    # first check the iov
                    iovr = payload.iovRange(chanNum)
                    if not iovr.isInRange(currentEvent): continue
                    if not validIOV: validIOV = ROOT.IOVRange(iovr)
                    al = payload.attributeList(chanNum)
                    hltpsk = ROOT.Long(0)
                    self.HltPSK = self.__class__.__cnv.readHltPrescaleKeyARA(al,hltpsk)
                    self.HltPSK = hltpsk
        return validIOV



    def _loadL1K(self, br, currentEvent):
        foundAnyIOV = False
        self.L1PSK = -1
        payloadList = []
        for i in xrange(self.mdt.GetEntries()):
            br.GetEntry(i)
            metaData = getattr(self.mdt, br.GetName())
            plc = metaData.payloadContainer()
            for payload in plc.iter():
                for i in xrange(payload.size()):
                    chanNum = int(payload.chanNum(i))
                    iovr = payload.iovRange(chanNum)
                    payloadList += [[iovr,payload.attributeList(chanNum)]]
                    foundAnyIOV = True
        if not foundAnyIOV: return None
        # sort the payload by IOV and remove the overlap
        payloadList.sort(lambda x,y: cmp(x[0].start(),y[0].start()))
        for i in xrange(len(payloadList)-1):
            payloadList[i][0] = ROOT.IOVRange(payloadList[i][0].start(),payloadList[i+1][0].start())
        # get the content
        validIOV =  None
        for pl in payloadList:
            if pl[0].isInRange(currentEvent):
                if not validIOV: validIOV = pl[0]
                self.L1PSK = pl[1]['Lvl1PrescaleConfigurationKey']
        return validIOV


    def _loadL1PS(self, br, currentEvent):
        foundAnyIOV = False
        self.L1PS = 256*[0]
        payloadList = [[] for x in xrange(256)]
        for i in xrange(self.mdt.GetEntries()):
            br.GetEntry(i)
            metaData = getattr(self.mdt, br.GetName())
            plc = metaData.payloadContainer()
            for payload in plc.iter():
                for i in xrange(payload.size()):
                    chanNum = int(payload.chanNum(i))
                    iovr = payload.iovRange(chanNum)
                    payloadList[chanNum] += [[iovr,payload.attributeList(chanNum)]]
                    foundAnyIOV = True
        if not foundAnyIOV: return None
        # sort the payload by IOV and remove the overlap
        for ch in xrange(256):
            x = payloadList[ch]
            x.sort(lambda x,y: cmp(x[0].start(),y[0].start()))
            for i in xrange(len(x)-1):
                x[i][0] = ROOT.IOVRange(x[i][0].start(),x[i+1][0].start())
        # read the payload
        psset = 256 * [0]
        validIOV =  None
        for ch in xrange(256):
            for pl in payloadList[ch]:
                if pl[0].isInRange(currentEvent):
                    if not validIOV: validIOV = pl[0]
                    psset[ch] = pl[1]['Lvl1Prescale']
                    
        for item in self.L1Items.values():
            item.ps = psset[item.ctpId()]
        self.L1PS = psset
        return validIOV

    def _loadL1M(self, br, currentEvent):
        validIOV = None
        items = {}
        for i in xrange(self.mdt.GetEntries()):
            br.GetEntry(i)
            metaData = getattr(self.mdt, br.GetName())
            plc = metaData.payloadContainer()
            for payload in plc.iter():
                #payload.dump()
                for i in xrange(payload.size()):
                    chanNum = int(payload.chanNum(i))
                    # first check the iov
                    iovr = payload.iovRange(chanNum)
                    if not iovr.isInRange(currentEvent): continue
                    if not validIOV: validIOV = ROOT.IOVRange(iovr)
                    al = payload.attributeList(chanNum)
                    l1item = self.__class__.__cnv.createLvl1TriggerItemARA(al)
                    l1item.setCtpId(chanNum)
                    items[l1item.name()] = l1item
        self.L1Items = items
        return validIOV


    def _loadHLTM(self, br, currentEvent):
        validIOV = None
        chains = {}
        for i in xrange(self.mdt.GetEntries()):
            br.GetEntry(i)
            metaData = getattr(self.mdt, br.GetName())
            plc = metaData.payloadContainer()
            for payload in plc.iter():
                #payload.dump()
                for i in xrange(payload.size()):
                    chanNum = int(payload.chanNum(i))
                    # first check the iov
                    iovr = payload.iovRange(chanNum)
                    if not iovr.isInRange(currentEvent): continue
                    if not validIOV: validIOV = ROOT.IOVRange(iovr)
                    al = payload.attributeList(chanNum)
                    chain = self.__class__.__cnv.createHLTChainARA(al)
                    chains[chain.chain_name()] = chain
        self.HLTChains = chains
        return validIOV
