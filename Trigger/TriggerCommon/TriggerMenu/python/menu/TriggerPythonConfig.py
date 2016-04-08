# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################
# User interface for the configuration, nothing beneath this class need to be studied
#
import re
import string
from collections import defaultdict
import xml.dom.minidom as minidom
import xml.etree.cElementTree as etree

from TriggerMenu.menu.HLTObjects import HLTSequence
from TriggerMenu.menu.LVL1Config import LVL1MenuItems,  LVL1Thresholds, CTPInfo, MuctpiInfo, Lvl1CaloInfo

from AthenaCommon.Logging        import logging
log = logging.getLogger( 'TriggerPythonConfig' )

# copied over from TrigConfigSvc
def extendLVL1config(triggerPythonConfig):
    """
    sets default bunchgroups for all menus, needed for simulation.
    """
    triggerPythonConfig.CTPInfo().setBunchGroupSet("MC")
    triggerPythonConfig.CTPInfo().addBunchGroup( 'BCRVeto',           0, [1] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'Filled',            1, [1] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'EmptyCalib',        2, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'Empty',             3, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'UnpairedBeam1',     4, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'UnpairedBeam2',     5, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'EmptyAfterFilled',  6, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'InTrain',           7, [1] )
    triggerPythonConfig.CTPInfo().setDeadtime('Commissioning', 4, 0, 0, 0, 0)
    triggerPythonConfig.CTPInfo().setRandom('rand01', 5, 0, 1, 1)
    triggerPythonConfig.CTPInfo().setPrescaledClock('psc01', 10, 100)
    triggerPythonConfig.Lvl1CaloInfo().setName('standard')


class TriggerPythonConfig:
    sCurrentTriggerConfig = None
    def currentTriggerConfig():
        return TriggerPythonConfig.sCurrentTriggerConfig
    currentTriggerConfig = staticmethod(currentTriggerConfig)

    def __init__(self, hltfile=None, l1file=None, signaturesOverwritten=False):
        self.menuName = 'TestMenu'
        self.__HLTFile = hltfile
        self.__L1File = l1file
        self.l1menuFromXML = None # l1menu read from XML file if any
        self.theLVL1Thresholds = LVL1Thresholds()
        self.theLVL1MenuItems  = LVL1MenuItems()
        self.theCTPInfo = CTPInfo()
        self.theMuctpiInfo = MuctpiInfo()
        self.theLvl1CaloInfo = Lvl1CaloInfo()
        self.allThresholds     = {}
        self.allItems          = {}
        self.allChains         = {}

        self.theHLTChains      = []
        self.theSeqLists       = []
        self.theSeqDict        = {} # dict by Seq output TE
        self.theTopoStartFrom    = []
        self.setMuctpiInfo(low_pt=1, high_pt=1, max_cand=13)
        self.Lvl1CaloInfo().setName('standard')
        self.Lvl1CaloInfo().setGlobalScale(1)
        self.signaturesOverwritten = signaturesOverwritten
        #
        TriggerPythonConfig.sCurrentTriggerConfig = self
        # self.defineInternalThresholds()
        extendLVL1config(self)


    def printIt(self):
        log.debug("self.theSeqLists: %s",  self.theSeqLists)
        log.debug("self.theSeqDict: %s", self.theSeqDict) 
        log.debug("self.theTopoStartFrom: %s", self.theTopoStartFrom) 
        


    # Work out smk and psk names to put inside xml files, for TT to use on upload

    # Please don't modify this function without consulting with the trigger configuration group
    def get_smk_psk_Name(self, menuName):
        smk_psk_Name = {}
        if "mc_prescale" in menuName:
            form = "(.*)_(.*)_mc_prescale"
            m = re.match(form, menuName)
            (smkName, pskName) = m.groups()
            pskName = pskName+"_mc"       
        elif "prescale" in menuName:
            #eg lumi1e31_simpleL1Calib_no_prescale
            form = "(.*)_(.*)_prescale"
            m = re.match(form,menuName)
            (smkName, pskName) = m.groups()
        else:
            #eg lumi1e31 ps set name can be "default"
            smkName = menuName
            pskName = "default"

        smk_psk_Name["smkName"] = str(smkName)
        smk_psk_Name["pskName"] = str(smkName+"_"+pskName+"_prescale")
        
        return smk_psk_Name

    # Please don't modify this function without consulting with the trigger configuration group
    @classmethod
    def getMenuNameFromHltpsName(cls,hltpsName):
        m = re.match( "(.*)_default_prescale", hltpsName)
        mmmm = re.match( "(.*)_mc_prescale", hltpsName)
        if m: return m.group(1)
        else: return hltpsName



    ##########################################################################
    # LVL1
    #
    def registerLvl1Threshold(self, name, type, mapping, 
                              slot='', connector='', active=1,
                              seed='', seed_ttype='', seed_multi=-1, bcdelay=-1,
                              ):
        """Add LVL1 thresholds by giving the type/mapping information.
        The cable input information will be calculated from them. And store it in
        the list of available thresholds
        Seed, seed_ttype, seed_multi and bcdelay are for the zero bias trigger only
        """

        thr = LVL1Thresholds.LVL1Threshold(name, type, seed_ttype)
        thr.active = active
        thr.mapping = mapping
        #for zero bias threshold type only:
        thr.seed = seed
        thr.seed_ttype = seed_ttype
        thr.seed_multi = seed_multi
        thr.bcdelay = bcdelay
        
        if type == 'ZB':
            (slot, connector, bitnum, range_begin, range_end) = self.cableInput(seed_ttype, mapping)
            #for zb threshold, begin and end is 30
            thr.setCableInput(bitnum, 30, 30, slot, connector)
        else:
            (slot, connector, bitnum, range_begin, range_end) = self.cableInput(type, mapping)
            thr.setCableInput(bitnum, range_begin, range_end, slot, connector)
        
        if name in self.allThresholds.keys():
            log.error('LVL1 threshold of name %s already exists' %
                           name)
            thr = self.allThresholds[name]
        else:
            self.allThresholds[name] = thr
        return thr
        
    def getLvl1Threshold(self, name):
        thr = None
        if name in self.allThresholds.keys():
            thr = self.allThresholds[name]
        return thr
    
    def registerLvl1Item(self, logical_name, item):
        """ Adds a LVL1 item to the set of items which are registered for further use"""
        if logical_name in self.allItems.keys():
            log.error('LVL1 item %s is already registered with ctpid=%d' % \
                           (logical_name, int(self.allItems[logical_name].ctpid)))
        else:
            self.allItems[logical_name] = item
            
    def getLvl1Item(self, name):
        item = None
        if name in self.allItems.keys():
            item = self.allItems[name]
        return item
        
    def addLvl1Threshold(self, name, type, mapping,
                         slot='', connector='', active=1):
        """Add LVL1 thresholds by giving the type/mapping information.
        The cable input information will be calculated from them."""
        thr0 = self.theLVL1Thresholds.thresholdOfTypeMapping(type, mapping)
        if thr0:
            log.warning('thr0 is name=%s' % thr0.name)
            s = 'Replacing LVL1 threshold of type=%s, mapping=%d name=%s' % \
                (type, mapping, thr0.name)
            s = '%s with name=%s' % (s, name)
            log.warning(s)
            self.theLVL1Thresholds.thresholds.remove(thr0)
        (slot, connector, bitnum, range_begin, range_end) = self.cableInput(type, mapping)
        thr = LVL1Thresholds.LVL1Threshold(name, type)
        thr.active = active
        thr.mapping = mapping
        thr.setCableInput(bitnum, range_begin, range_end, slot, connector)
        self.theLVL1Thresholds.thresholds.append(thr)
        return thr
        
    def addLvl1Threshold2(self, name, type, \
                         bitnum=0, range_begin=0, range_end=0, \
                         slot='', connector=''):
        """Add LVL1 thresholds by giving the input cable information"""
        thr = LVL1Thresholds.LVL1Threshold(name, type)
        thr.setCableInput(bitnum, range_begin, range_end, slot, connector)
        thr.mapping = LVL1Thresholds.cableMapping(type, range_begin)
        thr.active = 1
        self.theLVL1Thresholds.thresholds.append(thr)
        return thr
        
    def addLvl1Item(self, it):
        """Add LVL1 item of type LVL1MenuItem"""
        v = self.theLVL1MenuItems.findItemByName(it.name)
        if v:
            log.warning("Replacing LVL1 item (%s,%d) with (%s,%d)" % \
                             (v.name, int(v.ctpid), it.name, int(it.ctpid)))
        v = self.theLVL1MenuItems.findItemByCtpid(it.ctpid)
        if v:
            log.warning("Replacing LVL1 item (%s,%d) with (%s,%d)" % \
                             (v.name, int(v.ctpid), it.name, int(it.ctpid)))
        self.theLVL1MenuItems.items.append(it)



    def Lvl1ItemByTriggerType(self, triggertypebit, triggertypebitmask):
        """For a triggertypebit between 0 and 7, returns a list of names of
        those Lvl1 items that have that bit set in the triggertype"""
        if triggertypebit<0 or triggertypebit>0xFF:
            raise RuntimeError('TriggerPythonConfig.Lvl1ItemByTriggerType(triggertypebit,triggertypebitmask) needs to be called with 0<=triggertypebit<=0xFF, ' + \
                               + 'but is called with triggertypebit=%i' % triggertypebit)
        if triggertypebitmask<0 or triggertypebitmask>0xFF:
            raise RuntimeError('TriggerPythonConfig.Lvl1ItemByTriggerType(triggertypebit,triggertypebitmask) needs to be called with 0<=triggertypebitmask<=0xFF, ' + \
                               + 'but is called with triggertypebitmask=%i' % triggertypebitmask)

        #self.trigConfL1.menu.items) )

        itemsForMenu = [item for item in self.allItems.values() if item.ctpid != -1]
        
        if not itemsForMenu:
            log.warning('No item defined for the L1 Menu, perhaps TriggerPythonConfig.Lvl1ItemByTriggerType() ' + \
                             'is called too early (it needs to be called after Lvl1.generateMenu())')
        res = [item.name for item in itemsForMenu if (triggertypebitmask & item.trigger_type)==triggertypebit ]
        return res

    def Lvl1ItemByThreshold(self, thr_name):
        """Returns a list of L1 items using the specified threshold
(also searches for internal triggers). Original use case was to find all
items in BGRP1"""
        x = [ item.name for item in self.theLVL1MenuItems.items
              if thr_name in item.thresholdNames(True)]
        return x

    def thresholdOfName(self, name):
        """Return a threshold object of the given name"""
        return self.Lvl1Thresholds().thresholdOfName(name)
    def Lvl1Thresholds(self):
        """Return LVL1Thresholds object"""
        return self.theLVL1Thresholds
    def CTPInfo(self):
        """Return CTPInfo object"""
        return self.theCTPInfo
    def setMuctpiInfo(self, low_pt, high_pt, max_cand):
        """Return Muctpi object"""
        return self.theMuctpiInfo.set(low_pt, high_pt, max_cand)
    def Lvl1CaloInfo(self):
        """Return CaloInfo object"""
        return self.theLvl1CaloInfo
    def defineInternalThresholds(self):
        """Define thresholds for internal triggers"""
        self.addLvl1Threshold('RNDM0', 'RNDM', 0)
        self.addLvl1Threshold('RNDM1', 'RNDM', 1)
        self.addLvl1Threshold('PCLK0', 'PCLK', 0)
        self.addLvl1Threshold('PCLK1', 'PCLK', 1)
        self.addLvl1Threshold('BGRP0', 'BGRP', 0)
        self.addLvl1Threshold('BGRP1', 'BGRP', 1)
        self.addLvl1Threshold('BGRP2', 'BGRP', 2)
        self.addLvl1Threshold('BGRP3', 'BGRP', 3)
        self.addLvl1Threshold('BGRP4', 'BGRP', 4)
        self.addLvl1Threshold('BGRP5', 'BGRP', 5)
        self.addLvl1Threshold('BGRP6', 'BGRP', 6)
        self.addLvl1Threshold('BGRP7', 'BGRP', 7)
    def cableInput(self, type, mapping):
        """Calculate (slot, connector, bitnum, range_begin, range_end) of the input cable of
        the threshold to the CTP from its (type, mapping)"""
        bitnum = 0
        range_begin = 0
        range_end = 0
        slot = 0
        connector = 0
        bitnums = {
            'MUON': 3,
            'EM': 3,
            'TAU': 3,
            'JET': 3,
            'JE': 1,
            'JB': 2,
            'JF': 2,
            'TE': 1,
            'XE': 1,
            'XS': 1, 
            'MBTS': 3,
            'MBTSSI': 1,
            'NIM': 1,
            'ZDC': 1,
            'TRT': 1,
            'BCM': 1,
            'BCMCMB': 3,
            'LUCID': 1,
            'CALREQ': 1,
            }

        cables = { # type: [number of cables, slot, connector, start, stop ...]
            'MUON': [1, 8, 1, 1, 30], 
            'EM': [1, 7, 0, 0, 23],
            'TAU': [1, 7, 1, 0, 23],
            'JET': [1, 7, 2, 0, 23],
            'JE': [1, 7, 2, 24, 27],
            'JB': [1, 7, 3, 0, 7],
            'JF': [1, 7, 3, 8, 15],
            #'TE': [1, 8, 0, 0, 3], # With XS update, should be [ 1, 8, 0, 0, 7 ]
            #'XE': [1, 8, 0, 4, 7], # With XS update, should be [ 1, 8, 0, 8, 15 ]
            'TE': [ 1, 8, 0, 0, 7 ], 
            'XE': [ 1, 8, 0, 8, 15 ], 
            'XS': [ 1, 8, 0, 16, 23 ], # With XS update, should be [ 1, 8, 0, 16, 23 ]
            'MBTS': [2,
                     9, 0, 16, 18,
                     9, 1, 16, 18,
                     ],
            'MBTSSI': [2,
                       9, 0, 0, 15,
                       9, 1, 0, 15],
            'NIM': [3,
                    8, 2, 15, 27,
                    9, 0, 19, 30,
                    9, 1, 19, 30],
            'ZDC': [1, 8, 2, 17, 19],
            'TRT': [1, 8, 2, 22, 22],
            'BCM': [1, 8, 2, 0, 8],
            'BCMCMB': [1, 8, 2, 3, 5],
            'LUCID': [1, 8, 2, 9, 14],
            'CALREQ': [1, 8, 2, 28, 30],
            }
        
        bitnum = bitnums[type]

        if type=='EM' and mapping>=8: return self.cableInput('TAU', 15-mapping)
        
        for i in range(0,cables[type][0]):
            slot = cables[type][1+i*4]
            connector = cables[type][2+i*4]
            offset = mapping
            for j in range(0,i):
                delta = (cables[type][4+j*4]-cables[type][3+j*4])/bitnum+1
                log.debug('cable has room for %s thresholds.', delta)
                offset-=delta
            range_begin = cables[type][3+i*4]+offset*bitnum
            range_end = range_begin+bitnum-1
            if range_end <= cables[type][4+i*4]:
                break            
            else:
                if i == cables[type][0]:
                    log.warning("Untreated threshold type %s for cable assignment" % type)
        
        log.debug('%s threshold mapped at %s is set on SLOT: %s, connector: %s in range: %s - %s',
                  type, mappping, slot, connector, range_begin, range_end)

        return ('SLOT'+str(slot), 'CON'+str(connector), bitnum, range_begin, range_end)


    def addLVL1Threshold(self, th):
        """Obsolete - not obsolete!used in Lvl1.py in line 4710"""
        self.theLVL1Thresholds.thresholds.append(th)
        return self.theLVL1Thresholds.thresholds[-1]

#
# LVL1  END
####################################################################################################




####################################################################################################
# HLT  BEGIN
#

    def addHLTChain(self, chain):
        """ Adds HLT Trigger chain to the menu """
        if chain.isHLT():
            self.theHLTChains.append(chain)                        
        else:
            raise Exception( 'ERROR: Chain: ' + chain.chain_name + 'has incorrectly defined level')


    def checkChainUniqueness(self, teRenaming):
        """Check for chains defined multiple times but have differently"""
        def renameTE(te, teRemap):
            """returns teRemap[te] if existent, otherwise te itself"""
            if te in teRemap:
                return teRemap[te]
            return te
        sigs_wrongTEs = []
        chain_TEsigs = {} # [chain][TElist] -> list of signatures
        for sig, chains in self.allChains.iteritems():
            keys = chain_TEsigs.keys()
            for c in chains:
                #tes = string.join(c.getOutputTEs(), ',')
                tes  = string.join([ renameTE(te, teRenaming) for te in c.getOutputTEs() ], ',')                
                if c.chain_name in keys: # chain already defined before
                    TEsigs = chain_TEsigs[c.chain_name]
                    log.debug('Uniqueness check: TEs in sig=%s: %s', sig, tes)
                    log.debug(' TE list already found TEs: %s', TEsigs.keys())
                    if tes in TEsigs.keys():
                        TEsigs[tes].append(sig)
                    else:
                        TEsigs[tes] = [sig]
                else: # chain found for the first time
                    chain_TEsigs[c.chain_name] = { tes: [sig] }
        for c, TEsigs in chain_TEsigs.iteritems():
            if len(TEsigs) <= 1: continue
            n = sum(map(lambda x: len(x[1]), TEsigs.iteritems() ) )
            m = len(TEsigs)
            log.error('Chain %s defined %d times with %d variants' % (c, n, m))
            i = 0
            for TE, sigs in TEsigs.iteritems():
                log.warning('  [%d] TElist=%s : %s' % (i, TE, sigs))
                i += 1
        del(chain_TEsigs)
        pass

    
    def registerHLTChain(self, logical_name, chain):
        """ Adds chain to the set of chains which are registered for further use """
        if logical_name in self.allChains:
            self.allChains[logical_name].append( chain )
        else:
            self.allChains[logical_name] = [ chain ]
        
    def getHLTChain(self, name):
        """ Gets chain by name. None returned inf nothing found"""
        found=[]
        for  cl in self.allChains.values():
            for chain in cl:
                if chain.chain_name == name:
                    found.append(chain)
        if len(found)>1:
            idx=0
            for i in range(1,len(found)):
                if len(found[idx].siglist)<len(found[i].siglist):
                    idx=i
            return found[idx]
        if found:
            return found[0]
        return None

    def addSequence(self,  inTE, algolist, outTE, topo_start_from=None):
        """ Adds HLT sequences to the menu """

        if outTE in self.theSeqDict:
            log.error("There exists already sequence providing TE: "+outTE)
            log.error("Which is: "+str(self.theSeqDict[outTE]))
            log.error('Tried to produce it from '+inTE+' ('+str([a.name() for a in algolist]))
            
        if outTE in self.theLVL1Thresholds.thresholdNames():
            log.error("LVL1 theshold of name identical to output TE: "+outTE +" exists")
        seq = HLTSequence(inTE, algolist, outTE, topo_start_from)
        self.theSeqLists.append(seq)
        self.theSeqDict[outTE] = seq
        return outTE

    def addHLTSequence(self, theHLTSequence):
        """ Adds HLT sequences to TPC """
        outTE = theHLTSequence.output
        
        if outTE in self.theSeqDict:
            if not (str(self.theSeqDict[outTE]) == str(theHLTSequence)):
                log.error("Tried to add sequence %s but there exists already a DIFFERENT sequence with same TEout: %s " % (str(self.theSeqDict[outTE]),str(theHLTSequence)))
            else:
                log.debug("Tried to add sequence %s but there exists already the following sequence: %s ", self.theSeqDict[outTE], theHLTSequence)
            
        if outTE in self.theLVL1Thresholds.thresholdNames():
            log.error("LVL1 theshold of name identical to output TE: "+outTE +" exists")
    
        self.theSeqLists.append(theHLTSequence)
        self.theSeqDict[outTE] = theHLTSequence


    def seqDict(self):
        return self.theSeqDict
    def allSequences(self):
        return self.theSeqLists
    def allHLTChains(self):
        return self.theHLTChains

#
# HLT
####################################################################################################



    def checkMenuConsistency(self):
        """ Checks menu consistency


        Basic checks are done like chain continuity etc.
        Some of the checks are considered as a serious and therefore Exception is thrown.
        Other are just messaged as a warnings.
        """
        #print "self.checkChains()", self.checkChains()
        #print "self.checkSequences()", self.checkSequences()
        #print "self.checkL1()", self.checkL1()
        
        return self.checkChains() and self.checkSequences() and self.checkL1()


    def checkChainCounters(self, theChains):
        m_correct=True
        counters = map(lambda x: x.chain_counter, theChains) 
        for counter in counters: 
            if (int(counter) > 8191):
                log.error("IN CHAIN: Chain counter " +str(counter) + " is too high (13 bist = 8192 allowed for serialisation)")
                correct = False

        repcounters = filter(lambda x: counters.count(x) > 1, counters)
        for counter in repcounters:
            log.error("IN CHAIN: Chain counter "+ str(counter) + " used " + str(counters.count(counter)) + " times while can only once, will print them all")
            log.error( str(map( lambda x: x.chain_name,  filter(lambda x: x.chain_counter == counter, theChains))) )
            m_correct=False
        return m_correct



    def checkChains(self):
        log.info("TriggerPythonConfig: checkChains...")
        correct=True
        # repeating definitions
        ids = map(lambda x: x.chain_name, self.theHLTChains)      
        repid = filter(lambda x: ids.count(x) > 1, ids)
        for id in repid: 
            log.error(" IN CHAIN: Chain of chain_name "+ str(id) +" used " + str(ids.count(id))+ " times, while can only once") 
            correct=False                     
        
        correct = self.checkChainCounters(self.theHLTChains)

        # sufficiently defined ?
        for chain in self.theHLTChains:
            if len(chain.trigger_type_bits) == 0:
                log.debug("IN CHAIN: TRIGGERTYPE bit undefined for chain: %s will use chain_counter for it: %s",
                          chain.chain_name, chain.chain_counter)

                from string import atoi
                chain.trigger_type_bits = [atoi(chain.chain_counter)]
                
            if len(chain.stream_tag) == 0:                
                log.error( "IN CHAIN: STREAMTAG undefined for chain: %s", chain.chain_name)
                correct=False
                    
        # are TEs mentioned in sequences
        tesInSeq = set([seq.output for seq in self.theSeqLists])

        for chain in self.theHLTChains:
            for te in chain.getOutputTEs():
                if te not in tesInSeq:
                    log.error('IN CHAIN: TE: >' + te + '< used in the chain: ' + chain.chain_name + ' not found in the sequences definitions' )
                    log.debug('Available are: ', tesInSeq)
                    correct=False

        # Print list of available counters if check failed
        if not correct:

            all_counters = {}
            for chain in self.theHLTChains:
                all_counters[int(chain.chain_counter)] = "used"
                

            log.info("Number of used counters: %s", len(all_counters))
            if log.isEnabledFor(log.DEBUG):
                for used_id in all_counters:
                    log.debug("Used HLT counter: %s", used_id)
                
            for free_id in range(0, 1000):
                if free_id not in all_counters:
                    log.info("Available HLT counter: %s", free_id)
        
        return correct

    def checkSequences(self):
        """ Check sequences consistency """
        correct=True
        return correct


    def checkL1(self):
        """ Checks is Lvl1Menu is consistent """
        status = True
        for thr in self.theLVL1Thresholds.thresholds:
            if thr.active and len(thr.thresholdValues)==0:
                #zb threshold has no values
                if thr.ttype != 'ZB' and thr.name != 'ZB_4MBTS_A':
                    s = "LVL1 threshold %s is active," % thr.name
                    s += " but doesn't have any ThresholdValues"
                    log.error( s )
                    status = False
        return status

    def resolveDuplicateTEs(self):
        def renameTE(te, teRemap):
            """returns teRemap[te] if existent, otherwise te itself"""
            if (type(te)==list): te = te[0]
            if te in teRemap:
                return teRemap[te]
            return te
        def renameKey(key, teRemap):
            # print 'key=',key
            keycount = key.count("#")
            if (keycount == 2):
                tes, algos, tsf = key.split('#')
            else:
                tes, algos = key.split('#')

            tes = tes.split(',')
            key1 = ''
            for te in tes:
                key1 += '%s,' % renameTE(te, teRemap)
            if len(key1)>0: key1 = key1[:-1]
            if keycount == 1:
                return '%s#%s' % (key1, algos)
            elif keycount ==2:
                return '%s#%s#%s' % (key1, algos,tsf)

        def chooseName(v):
            """pick the name of the output TE from a list of output TE's (which have the same definition)
            v: list of TE names of length >= 2
            returned is the shortest TE name, or in case of ambiguity the first in the alphabet
            """
            goodname = v[0]
            for x in v[1:]:
                if len(x)<len(goodname) or ( len(x)==len(goodname) and x < goodname ): goodname = x
            return goodname
        def createKey(seq):
            """turns a sequence into a 'key' for use in a dictionary
            concatenates input TEs and algs
            """
            inTEs = ','.join([str(te) for te in seq.input])
            algs  = ','.join([str(a) for a in seq.algs])
            if (seq.topo_start_from):
                tsf = ','.join([str(a) for a in seq.topo_start_from])
                return '%s#%s#%s' % (inTEs, algs, tsf)
            else:
                return '%s#%s' % (inTEs, algs)

        seqs = self.allSequences()
        seq_list1 = set() # updated set of sequences after renaming TEs
        seq_list2 = set() # temporary set of sequences
        seq_to_outTEs = defaultdict(list)
        for seq in seqs:
            key = createKey(seq)
            outTE = seq.output
            seq_to_outTEs[key].append(outTE)
            seq_list1.add( (key, outTE) )
        
        # Update the renaming rules
        teRenaming = {}
        iterationCount = 0

        #---------------------------------------------------------------------
        # Change outTE names which use the same inputTEs and algorithms
        # Once the outTEs are changed, modify the inputTEs which use them
        # Do it recursively, until there's no more changes needed
        #---------------------------------------------------------------------
        while True:
            # First update the renaming rules based on the previous iteration
            for outTEs in seq_to_outTEs.itervalues():
                if len(outTEs) <= 1: continue                                       
                name = chooseName(outTEs)
                for x in outTEs:
                    teRenaming[x] = name

            # Changing the mapping defined earlier
            # A->B(old), B->C(new) should become A->C
            for a, b in teRenaming.iteritems():
                if b in teRenaming:
                    teRenaming[a] = teRenaming[b]
            
            # Update sequences (remove duplicates)
            n1 = len(seq_list1)
            seq_list2 = set()
            seq_to_outTEs = defaultdict(list)
            for key,outTE in seq_list1:
                key = renameKey(key, teRenaming)
                # This is actually slightly faster than dict.get(outTE,outTE)
                outTE2 = outTE if not outTE in teRenaming else teRenaming[outTE]

                seq_to_outTEs[key].append(outTE2)
                seq_list2.add( (key, outTE2) )
        
            seq_list1 = seq_list2
            iterationCount += 1
            if log.isEnabledFor(logging.DEBUG):
                log.debug('N TE remapping after iteration %d: %d' % iterationCount, len(teRenaming))
                
            if len(seq_list1) == n1: break

        # teRenaming still contains mappings x->x now, removing it
        teRenaming = dict( [(o,t) for o,t in teRenaming.items() if o!=t] )

        #--------------------------------------------
        # Apply the TE renaming to the real sequences
        #--------------------------------------------
        for seq in seqs:
            seq.input  = [ renameTE(inTE, teRenaming) for inTE in seq.input ]
            seq.output = renameTE(seq.output, teRenaming)
            
        #--------------------------------------------
        # Apply the TE renaming to the real chains
        #--------------------------------------------
        for chain in self.allHLTChains():
            for sig in chain.siglist:
                oldtes = sig.tes
                sig.tes  = [ renameTE(te, teRenaming) for te in sig.tes ]
                if oldtes != sig.tes:
                    log.debug('Remapping applied to chain %s and TEs %r to TEs %r', chain.chain_name, oldtes, sig.tes)

        if log.isEnabledFor(logging.DEBUG):
            for k in sorted(teRenaming.keys()):
                log.debug('  %-40s -> %s', k, teRenaming[k])
        
        return teRenaming
            

    def findSequencesRecursively(self, seqDict, te):
        """ finds recursively sequences
        
        seqDict is dictionary {sequence outout te: sequence obj}
        """
        seq = []
        if not seqDict.has_key(te):
            return seq
        this = seqDict[te]
        seq += [this]
        for i in this.input:
            if i != '':
                seq += self.findSequencesRecursively(seqDict, i)		
        return seq	

    def getChainTEs(self, seqDict, chain, alsorecursive=False):
        """ gets TEs used by chain (also recursively searching sequences)

        seqDict is dictionary {sequence outout te: sequence obj}
        """
        neededtes = reduce(lambda x,y: x+y, [s.tes for s in chain.siglist], [])
        if alsorecursive == True:
            seqs = []
            for te in neededtes:
                seqs += self.findSequencesRecursively(seqDict, te)
                for s in seqs:
                    # if s.used is set then it needs 
                    if (s.used() and s.used() == chain.level) or not s.used():
                        if s.output not in neededtes:
                            neededtes.append(s.output)    
        return neededtes


    def markUsedSequences(self):
        seqDict = {}
        seqDict.update([ (s.output, s) for s in self.theSeqLists])

        # mark sequences as the one which will be run at HLT
        for c in self.theHLTChains:
            tes = reduce(lambda x,y: x+y, [s.tes for s in c.siglist], [])
            for te in tes:
                seqs = self.findSequencesRecursively(seqDict, te)
                for seq in seqs:
                    if seq.used() == 'HLT':
                        continue
                    else:
                        seq.use('HLT')


 
    def getL1ConfigFile(self):
        return self.__L1File

    def getHLTConfigFile(self):
        return self.__HLTFile

    def writeHLTConfigFile(self,smk_psk_Name):
        """ Writes HLT config file"""
        if self.__HLTFile is None:
            return

        # HLT XML
        xHLTMenu = etree.Element("HLT_MENU",
                                 menu_name = smk_psk_Name["smkName"],
                                 prescale_set_name = smk_psk_Name["pskName"])

        # comments
        xHLTMenu.append(etree.Comment('File is auto-generated by python config class, may be overwritten'))
        xHLTMenu.append(etree.Comment('No. of HLT chains:' + str(len(self.theHLTChains))))

        self.markUsedSequences()
        
        # make xml sequences
        xSequenceList = etree.SubElement(xHLTMenu,'SEQUENCE_LIST')
        for s in self.theSeqLists:
            s.xml( xSequenceList )

        # make xml chains
        xChainList = etree.SubElement(xHLTMenu,'CHAIN_LIST')                        
        for c in self.theHLTChains:
            c.xml( xChainList )            

        # write out the xml as a file
        log.debug("Writing XML file for HLT")
        file = open( self.__HLTFile, mode="wt" )
        # etree does not support pretty printing, use minidom for this
        file.write(minidom.parseString(etree.tostring(xHLTMenu)).toprettyxml('  '))
        file.close()

                    
    def writeConfigFiles(self,level="all"):
        """ Writes all config files, and checks consistency"""

        teRenaming = self.resolveDuplicateTEs()
        self.checkChainUniqueness(teRenaming)
        
        #self.checkStreamConsistency()
        
        # disable python tracing
        from AthenaCommon.Include import excludeTracePattern
        excludeTracePattern.append("*/dom/*")

        smk_psk_Name = self.get_smk_psk_Name(self.menuName)

        # HLT XML
        if level=="all" or level=="hlt":
            self.writeHLTConfigFile(smk_psk_Name)

        # L1 XML no longer supported (now done by Lvl1Menu.py)
        if level=="l1":
            log.error("Lvl1 menu writing no longer supported here")

        if self.checkMenuConsistency() is False:
            raise Exception( "Trigger menu inconsistent, aborting...." )


    def dot(self, algs=True):
        file = open( self.__HLTFile[0:-4]+'_sequenceses.dot', mode="wt" )
        file.write( 'digraph Menu  {\n'\
                    +'\n'\
                    +'node [ shape=polygon, fontname=Helvetica ]\n'\
                    +'edge [ fontname=Helvetica ]\n' )
        for s in self.theSeqLists:
            file.write( s.dot(algs) )
        file.write( '}')
        file.close()

        
#
# EOF user interface
#########################################################################################





#########################################################################################
# internals
#

# little utility for xml printing attribute
def attr(name, value):
    return ' '+name+'="'+str(value)+'" '

# unique id generator
class idgen:
    a =  {}
    def get(self, kind):
        if kind not in self.a:
            self.a[kind] = 0
        self.a[kind] += 1
        return str(self.a[kind])

idgen = idgen()


def issubset(a, b):
    """ True check if all element in a also are in b """
    return len(filter( lambda x: x not in b, a)) == 0



