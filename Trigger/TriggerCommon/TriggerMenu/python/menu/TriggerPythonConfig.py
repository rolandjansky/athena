# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################
# User interface for the configuration, nothing beneath this class need to be studied
#
import os
import time
import re
import string
from sys import settrace
import xml.dom.minidom
from AthenaCommon.Logging        import logging  # loads logger
from TriggerMenu.menu.HLTObjects import HLTSequence

from TriggerMenu.menu.LVL1Config import LVL1MenuItems,  LVL1Thresholds, CTPInfo, MuctpiInfo, Lvl1CaloInfo

from AthenaCommon.Logging import logging
TPClog = logging.getLogger( 'TriggerMenu.TriggerPythonConfig' )


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

    def __init__(self, hltfile=None, l1file=None):
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

        self.theL2HLTChains    = []
        self.theEFHLTChains    = []
        self.theHLTChains      = []
        self.theSeqLists       = []
        self.theSeqDict        = {} # dict by Seq output TE
        self.setMuctpiInfo(low_pt=1, high_pt=1, max_cand=13)
        self.Lvl1CaloInfo().setName('standard')
        self.Lvl1CaloInfo().setGlobalScale(1)
        #
        TriggerPythonConfig.sCurrentTriggerConfig = self
        # self.defineInternalThresholds()
        extendLVL1config(self)


    def printIt(self):
        TPClog.debug("self.theL2HLTChains: %s " % self.theL2HLTChains)
        TPClog.debug("self.theEFHLTChains: %s" % self.theEFHLTChains)
        TPClog.debug("self.theSeqLists: %s" % self.theSeqLists)
        TPClog.debug("self.theSeqDict: %s" % self.theSeqDict) 
        


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
            logger().error('LVL1 threshold of name %s already exists' %
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
            logger().error('LVL1 item %s is already registered with ctpid=%d' % \
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
            logger().warning('thr0 is name=%s' % thr0.name)
            s = 'Replacing LVL1 threshold of type=%s, mapping=%d name=%s' % \
                (type, mapping, thr0.name)
            s = '%s with name=%s' % (s, name)
            logger().warning(s)
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
            logger().warning("Replacing LVL1 item (%s,%d) with (%s,%d)" % \
                             (v.name, int(v.ctpid), it.name, int(it.ctpid)))
        v = self.theLVL1MenuItems.findItemByCtpid(it.ctpid)
        if v:
            logger().warning("Replacing LVL1 item (%s,%d) with (%s,%d)" % \
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
            logger().warning('No item defined for the L1 Menu, perhaps TriggerPythonConfig.Lvl1ItemByTriggerType() ' + \
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
                logger().debug('cable has room for '+str(delta)+' thresholds.')
                offset-=delta
            range_begin = cables[type][3+i*4]+offset*bitnum
            range_end = range_begin+bitnum-1
            if range_end <= cables[type][4+i*4]:
                break            
            else:
                if i == cables[type][0]:
                    logger().warning("Untreated threshold type %s for cable assignment" % type)

        logger().debug(type+' threshold mapped at '+str(mapping)+' is set on SLOT: '+str(slot)+
                      ', connector: '+str(connector)+' in range: '+str(range_begin)+' - '+str(range_end))

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
        if chain.isL2():
            self.theL2HLTChains.append(chain)
        elif chain.isEF():
            self.theEFHLTChains.append(chain)
        elif chain.isHLT():
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
                    logger().debug('Uniqueness check: TEs in sig=%s: %s' % (sig, tes))
                    logger().debug(' TE list already found TEs: %s' % str(TEsigs.keys()))
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
            logger().error('Chain %s defined %d times with %d variants' % (c, n, m))
            i = 0
            for TE, sigs in TEsigs.iteritems():
                logger().warning('  [%d] TElist=%s : %s' % (i, TE, sigs))
                i += 1
        del(chain_TEsigs)
        pass

    
    def registerHLTChain(self, logical_name, chain):
        """ Adds chain to the set of chains which are registered for further use """
        if logical_name in self.allChains.keys():
            # check that an EF name is only defined once
            if(chain.level == 'EF'):
                for ch in self.allChains[logical_name]:
                    if(ch.level == 'EF'):
                        logger().error("Trying to define EF item more than once "+chain.chain_name)
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

    def addSequence(self,  inTE, algolist, outTE, topo_starts_from=None):
        """ Adds HLT sequences to the menu """

        if outTE in self.theSeqDict.iterkeys():
            logger().error("There exists already sequence providing TE: "+outTE)
            logger().error("Which is: "+str(self.theSeqDict[outTE]))
            logger().error('Tried to produce it from '+inTE+' ('+str([a.name() for a in algolist]))
            
        if outTE in self.theLVL1Thresholds.thresholdNames():
            logger().error("LVL1 theshold of name identical to output TE: "+outTE +" exists")
        seq = HLTSequence(inTE, algolist, outTE, topo_starts_from)
        self.theSeqLists.append(seq)
        self.theSeqDict[outTE] = seq
        return outTE

    def addHLTSequence(self, theHLTSequence):
        """ Adds HLT sequences to TPC """
        outTE = theHLTSequence.output
        inTE = theHLTSequence.input
        topo_starts_from = theHLTSequence.topo_starts_from
        algolist = theHLTSequence.algs
        
        if outTE in self.theSeqDict.iterkeys():
            if not (str(self.theSeqDict[outTE]) == str(theHLTSequence)):
                logger().error("Tried to add sequence %s but there exists already a DIFFERENT sequence with same TEout: %s " % (str(self.theSeqDict[outTE]),str(theHLTSequence)))
            else:
                logger().debug("Tried to add sequence %s but there exists already the following sequence: %s " % (str(self.theSeqDict[outTE]),str(theHLTSequence)))
            
        if outTE in self.theLVL1Thresholds.thresholdNames():
            logger().error("LVL1 theshold of name identical to output TE: "+outTE +" exists")
    
        self.theSeqLists.append(theHLTSequence)
        self.theSeqDict[outTE] = theHLTSequence


    def seqDict(self):
        return self.theSeqDict
    def allSequences(self):
        return self.theSeqLists
    def allL2Chains(self):
        return self.theL2HLTChains
    def allEFChains(self):
        return self.theEFHLTChains
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
                logger().error("IN CHAIN: L2 Chain counter " +str(counter) + " is too high (13 bist = 8192 allowed for serialisation)")
                correct = False

        repcounters = filter(lambda x: counters.count(x) > 1, counters)
        for counter in repcounters:
            logger().error("IN CHAIN: Chain counter "+ str(counter) + " used " + str(counters.count(counter)) + " times while can only once, will print them all")
            logger().error( str(map( lambda x: x.chain_name,  filter(lambda x: x.chain_counter == counter, theChains))) )
            m_correct=False
        return m_correct



    def checkChains(self):
        logger().info("TriggerPythonConfig: checkChains...")
        correct=True
        # repeating definitions
        ids = map(lambda x: x.chain_name, self.theL2HLTChains + self.theEFHLTChains + self.theHLTChains)      
        repid = filter(lambda x: ids.count(x) > 1, ids)
        for id in repid: 
            logger().error(" IN CHAIN: Chain of chain_name "+ str(id) +" used " + str(ids.count(id))+ " times, while can only once") 
            correct=False         
        

        #HLTids = map(lambda x:x.chain_name, self.theHLTChains)
        #print 'self.theHLTChains', self.theHLTChains
        #

        #chainlist = []
        #if repid:
        #    for chain in (self.theHLTChains+self.theL2HLTChains+self.theEFHLTChains):
        #        newHLTids = map(lambda x: x.chain_name, self.theL2HLTChains+self.theEFHLTChains + self.theHLTChains)      
        #        newrepid = filter(lambda x: newHLTids.count(x) > 1, newHLTids)
        #        if (newrepid) and (chain.chain_name == newrepid[0]):
        #            #print 'Repeated chain: chain.chain_name', chain.chain_name
        #            if 'HLT' in chain.chain_name:
        #                logger().warning("Removing duplicated HLT chain: "+str(chain.chain_name))                                            
        #                self.theHLTChains.remove(chain)
        #            elif 'L2' in chain.chain_name:
        #                logger().warning("Removing duplicated L2 chain: "+str(chain.chain_name))                                            
        #                self.theL2HLTChains.remove(chain)
        #            elif 'EF' in chain.chain_name:
        #                logger().warning("Removing duplicated EF chain: "+str(chain.chain_name))                                            
        #                self.theEFHLTChains.remove(chain)
        #                
        #testHLTids = map(lambda x: x.chain_name, self.theHLTChains+self.theL2HLTChains+self.theEFHLTChains)      
        #testrepid = filter(lambda x: testHLTids.count(x) > 1, testHLTids)
        #
        #for id in testrepid:
        #    logger().error("IN CHAIN: Chain of chain_name "+ str(id) +" used " + str(testHLTids.count(id))+ " times, while can only once")           
        #    #logger().warning(" IN CHAIN: Chain of chain_name "+ str(id) +" used " + str(testHLTids.count(id))+ " times, while can only once")


        
        correct = self.checkChainCounters(self.theL2HLTChains)
        correct = self.checkChainCounters(self.theEFHLTChains)
        correct = self.checkChainCounters(self.theHLTChains)

        # sufficiently defined ?
        for chain in self.theL2HLTChains + self.theEFHLTChains + self.theHLTChains:
            if len(chain.trigger_type_bits) == 0:
                logger().debug( "IN CHAIN: TRIGGERTYPE bit undefined for chain: " 
                                +  str(chain.chain_name)+" will use chain_counter for it: "+str(chain.chain_counter) )

                from string import atoi
                #print chain.chain_counter
                chain.trigger_type_bits = [atoi(chain.chain_counter)]

                
            if len(chain.stream_tag) == 0:                
                logger().error( "IN CHAIN: STREAMTAG undefined for chain: " + str(chain.chain_name) )
                correct=False

        if self.__L1File is not None:
            for chain in self.theL2HLTChains:
                if chain.lower_chain_name != "":
                    lower_chains = [ x.strip(" ") for x in  chain.lower_chain_name.split(",") ]
                    #                   if chain.lower_chain_name not in self.theLVL1MenuItems.itemNames():
                    #print "lower_chains ", lower_chains
                    #print "self.theLVL1MenuItems.itemNames():", self.theLVL1MenuItems.itemNames()
                    for lc in lower_chains:
                        if lc not in self.theLVL1MenuItems.itemNames():
                            logger().error( 'IN CHAIN: L2 chain '+chain.chain_name+' with has no matching LVL1 item '
                                            + lc +   chain.lower_chain_name + '   between:'+\
                                            str(self.theLVL1MenuItems.itemNames()))
                            correct=False

        # is EF matching L2
        l2ChainNames = map ( lambda x: x.chain_name, self.theL2HLTChains )
        for chain in self.theEFHLTChains:
            #print "JJJJJJJJL2", l2ChainNames
            #print "JJJJJJJJlower",  chain.lower_chain_name
            if chain.lower_chain_name != "":
                if chain.lower_chain_name not in l2ChainNames:
                    logger().error( 'ERROR IN CHAIN: EF chain: '+str(chain.chain_name)+' has no matching L2 chain between:'+\
                        str(l2ChainNames) )                    
                    correct=False

                    
        # are TEs mentioned in sequences
        tesInSeq = []
        for seq in self.theSeqLists:
            tesInSeq.append(seq.output)
            #print tesInSeq
        for chain in self.theL2HLTChains + self.theEFHLTChains + self.theHLTChains:
            for te in chain.getOutputTEs():
                if te not in tesInSeq:
                    logger().error( 'IN CHAIN: TE: >' + te + '< used in the chain: ' + chain.chain_name + ' not found in the sequences definitions' )
                    logger().debug('Available are: '+ str(tesInSeq))
                    correct=False

        # check if all TEs needed for EF chains are produced by corresponding L2 chain
        # this cehck is ported to C++ version of the checker
        # Print list of available counters if check failed
        if not correct:

            all_counters = {}
            for chain in self.theL2HLTChains + self.theEFHLTChains  + self.theHLTChains:
                all_counters[int(chain.chain_counter)] = "used"
                

            logger().info("Number of used counters: "+str(len(all_counters)))
            for used_id in all_counters.iterkeys():
                logger().debug("Used HLT counter: "+str(used_id))
                
            for free_id in range(0, 1000):
                if free_id not in all_counters:
                    logger().info("Available HLT counter: "+str(free_id))
        
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
                    logger().error( s )
                    status = False
        return status


    def alignChains(self, chainsList):
        work = True
        # operations must repeat until all chains are OK
        while work:
            logger().debug(" Chains alignment round starting")
            work = False
            for chain in chainsList:
                for binding in chain.chainBindings:
                    # find chain of name
                    peerChain = filter(lambda x: x.chain_name == binding[1],  chainsList)
                    if len(peerChain) == 1:
                        work = work or chain.alignChain(binding[0], peerChain[0], binding[2])
                    else:
                        logger().debug( "binding ineffective peer chain: " + str(binding[1]) \
                                        + " does not exist in between: "+str(map(lambda x: x.chain_name,  chainsList))\
                                        + " or there is more than 1" )
                        
        # support composition of chain out of other chains
        for chain in chainsList:
            if len(chain.chains_to_merge) != 0:
                chain.doChainsMerging()

    def resolveDuplicateTEs(self):
        def renameTE(te, teRemap):
            """returns teRemap[te] if existent, otherwise te itself"""
            if (type(te)==list): te = te[0]
            if te in teRemap:
                return teRemap[te]
            return te
        def renameKey(key, teRemap):
            # print 'key=',key
            tes, algos = key.split('#')
            tes = tes.split(',')
            key1 = ''
            for te in tes:
                key1 += '%s,' % renameTE(te, teRemap)
            if len(key1)>0: key1 = key1[:-1]
            return '%s#%s' % (key1, algos)
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
            return '%s#%s' % (inTEs, algs)

        seqs = self.allSequences()
        seq_list0 = [] # original sequences
        seq_list1 = [] # updated list of sequences after renaming TEs
        seq_list2 = [] # temporary list of sequences
        seq_to_outTEs = {}
        for seq in seqs:
            key = createKey(seq)
            outTE = seq.output
            if key in seq_to_outTEs:
                seq_to_outTEs[key] += [outTE]
            else:
                seq_to_outTEs[key]  = [outTE]
            seq_list0.append( (key, outTE))
            seq_list1.append( (key, outTE))

        # Update the renaming rules
        teRenaming = {}
        iterationCount = 0
        nn = 0

        #---------------------------------------------------------------------
        # Change outTE names which use the same inputTEs and algorithms
        # Once the outTEs are changed, modify the inputTEs which use them
        # Do it recursively, until there's no more changes needed
        #---------------------------------------------------------------------
        while True:
            # First update the renaming rules based on the previous iteration
            for seq, outTEs in seq_to_outTEs.iteritems():
                if len(outTEs) <= 1: continue
                name = chooseName(outTEs)
                for x in outTEs:
                    teRenaming[x] = name
                    # Changing the mapping defined earlier
                    # A->B(old), B->C(new) should become A->C
                    for a, b in teRenaming.iteritems():
                        if b == x: teRenaming[a] = name

            # Update sequences (remove duplicates)
            n1 = len(seq_list1)
            seq_list2 = []
            seq_to_outTEs = {}
            for x in seq_list1:
                key = x[0]
                outTE = x[1]
                key = renameKey(key, teRenaming)
                outTE2 = outTE
                if outTE in teRenaming:
                    outTE2 = teRenaming[outTE]
                #
                if key in seq_to_outTEs:
                    seq_to_outTEs[key].append(outTE2)
                else:
                    seq_to_outTEs[key] = [outTE2]
                if seq_list2.count( (key, outTE2)) == 0:
                    seq_list2.append( (key, outTE2))
            seq_list1 = seq_list2
            iterationCount += 1
            logger().debug('N TE remapping after iteration %d: %d' % \
                     (iterationCount, len(teRenaming)) )
            nn += n1 - len(seq_list1)
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
        # chains = self.allL2Chains() + self.allEFChains()
        for chain in (self.allL2Chains() + self.allEFChains() + self.allHLTChains()):
            for sig in chain.siglist:
                oldtes = sig.tes
                sig.tes  = [ renameTE(te, teRenaming) for te in sig.tes ]
                if oldtes != sig.tes:
                    logger().debug('Remapping applied to chain %s and TEs %r to TEs %r' % (chain.chain_name, oldtes, sig.tes))
                    
        for k in sorted(teRenaming.keys()):
            logger().debug('  %-40s -> %s' % (k, teRenaming[k]))
        return teRenaming
            
    def resolveDuplicateTEsOrig(self):
        seqs = self.allSequences()
        # logger().info('N chains %d' % len(chains))
        seq_to_outTE = {}
        algo2inTE2outTE = {} # algo -> inTE[] -> outTE[]
        seq2outTE = {} # seq: algo:inTEs
        for seq in seqs:
            inTEs = list(seq.input)
            algos = list(seq.algs)
            #inTEs.sort() # this should not be sorted
            #algos.sort() # order of algorithms is important
            key1, key2 = '', ''
            for x in inTEs:
                key1 += '%s,' % x
            if len(key1)>0: key1 = key1[:-1] # remove last comma
            for x in algos: key2 += x
            key = '%s:%s' % (key1, key2)
            if key2 in algo2inTE2outTE.keys():
                in2out = algo2inTE2outTE[key2]
                if key1 in in2out.keys():
                    in2out[key1].append(seq)
                else:
                    in2out[key1] = [seq]
            else:
                algo2inTE2outTE[key2] = { key1: [seq] }
            if key in seq_to_outTE.keys():
                seq_to_outTE[key].append(seq)
            else:
                seq_to_outTE[key] = [ seq ]
        for k in algo2inTE2outTE.keys():
            if len(algo2inTE2outTE[k]) <= 1: algo2inTE2outTE.pop(k, None)
        logger().debug('%s seq.algo types' % len(algo2inTE2outTE))
        teRemap = {} # Holds which TEs are remapped
        # Replace the TE names recursively starting from earlier sequences
        while True:
            nnn = 0
            logger().debug('%d TE remaps defined' % len(teRemap))
            for v in teRemap.iteritems():
                logger().debug('TE remapping: %s -> %s' % v)
            # Remap inputTE names based on the current knowledge
            for k1 in algo2inTE2outTE.keys():
                in2out = algo2inTE2outTE[k1]
                in_tes = []
                y = {}
                for k2 in in2out.keys():
                    if k2 in teRemap.keys():
                        x = teRemap[k2]
                    else:
                        x = k2
                    if x in y.keys():
                        logger().debug('Reducing inputTE %s' % x)
                        y[x].extend(in2out[k2])
                    else:
                        y[x] = list(in2out[k2])
                algo2inTE2outTE[k1] = y
            # Check for newly identified remapping rules
            for k1 in algo2inTE2outTE.keys():
                in2out = algo2inTE2outTE[k1]
                if len(in2out) <= 1: continue
                for k2 in in2out.keys():
                    outs = in2out[k2]
                    te0, i0, n0 = '', -1, 1000
                    for i, seq in enumerate(outs):
                        te = seq.output
                        n = len(te)
                        if n < n0 or i0<0:
                            i0, n0, te0 = i, n, te
                    for seq in outs:
                        te = seq.output
                        if te != te0:
                            nnn += 1
                            teRemap[te] = te0
                    in2out[k2] = outs[0:1]
                    in2out[k2][0].output = te
            logger().debug('Reduced %d TE names' % nnn)
            if nnn == 0: break
            pass
        logger().debug('%d TE remapping defined' % len(teRemap))
        # Use a common outputTE name for same sequences. Choose the TE name
        # with the shorted length as the unique TE for that sequence.
        # Places to change are
        # - Input TE
        # - Output TE
        # - TE name used in signatures
        seqs_new=[]
        for iseq in range(len(seqs)):
            seq = seqs[iseq]
            if seq.output not in teRemap.keys():
                for i in range(len(seq.input)):
                    if seq.input[i] in teRemap.keys():
                        #logger().info('check %s against %s' % (seq.input[i], teRemap))
                        seq.input[i] = teRemap[seq.input[i]]
                seqs_new.append(seq)
            else:
                # Drop the sequence
                #logger().info('Chaning output %s->%s' % (seq.output, teRemap[seq.output]))
                seq.output = teRemap[seq.output]
                for i in range(len(seq.input)):
                    if seq.input[i] in teRemap.keys():
                        #logger().info('check %s against %s' % (seq.input[i], teRemap))
                        seq.input[i] = teRemap[seq.input[i]]
                seqs_new.append(seq)
                pass
        chains = self.allL2Chains() + self.allEFChains() + self.allHLTChains()
        for ichain in range(len(chains)):
            chain = chains[ichain]
            # logger().info('Remaping sig.tes in %s (%d sigs)' % (chain.chain_name, len(chain.siglist)))
            for isig in range(len(chain.siglist)):
                sig = chain.siglist[isig]
                for i in range(len(sig.tes)):
                    if sig.tes[i] in teRemap.keys():
                        sig.tes[i] = teRemap[sig.tes[i]]
                # logger().info('sig.tes: %s' % sig.tes)
        self.theSeqLists = seqs_new

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

        # mark sequences as the one which will be run at L2
        for c in self.theL2HLTChains:
            tes = reduce(lambda x,y: x+y, [s.tes for s in c.siglist], [])
            for te in tes:
                seqs = self.findSequencesRecursively(seqDict, te)
                for seq in seqs:
                    if seq.used() == 'L2':
                        continue
                    else:
                        seq.use('L2')
                        
        # mark sequences as the one which will be run at EF
        for c in self.theEFHLTChains:
            tes = reduce(lambda x,y: x+y, [s.tes for s in c.siglist], [])
            for te in tes:
                seqs = self.findSequencesRecursively(seqDict, te)
                for seq in seqs:
                    if seq.used() == 'L2':
                        continue
                    else:
                        seq.use('EF')

        # mark sequences as the one which will be run at EF
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

    def writeL1ConfigFile(self,smk_psk_Name):
        """ Writes L1 config file"""
        if self.__L1File is None:
            return

        # LVL1 XML
        xlvl1doc = xml.dom.minidom.Document()

        #config element contains all
        xlvl1config = xlvl1doc.createElement('LVL1Config')
        xlvl1config.setAttribute('name', self.menuName)
        xlvl1config.setAttribute('id',idgen.get('LVL1Config'))
        xlvl1doc.appendChild(xlvl1config)

        # comments
        xc = xlvl1doc.createComment('File is auto-generated by python config class, may be overwritten')
        xlvl1config.appendChild(xc)
        xc = xlvl1doc.createComment('No. L1 thresholds defined: ' + str(len(self.theLVL1Thresholds.thresholds)) )
        xlvl1config.appendChild(xc)
        xc = xlvl1doc.createComment('No. L1 items defined: ' + str(len(self.theLVL1MenuItems.items)) )
        xlvl1config.appendChild(xc)

        #pass menu and psk names 
        self.theLVL1MenuItems.xml(xlvl1config, smk_psk_Name["smkName"],smk_psk_Name["pskName"] )
        self.theLVL1Thresholds.xml(xlvl1config)
        self.theCTPInfo.xml(xlvl1config)
        self.theMuctpiInfo.xml(xlvl1config)
        self.theLvl1CaloInfo.xml(xlvl1config)

        file = open( self.__L1File, mode="wt" )

        logger().debug("Writing XML file for LVL1")
        file.write( xlvl1doc.toprettyxml('  ') )
        file.close()
        xlvl1doc.unlink()



    def writeHLTConfigFile(self,smk_psk_Name):
        """ Writes HLT config file"""
        if self.__HLTFile is None:
            return

        # HLT XML
        xHLTdoc = xml.dom.minidom.Document()

        # comments
        xc = xHLTdoc.createComment('File is auto-generated by python config class, may be overwritten')
        xHLTdoc.appendChild(xc)
        xc = xHLTdoc.createComment('No. of L2 HLT chains:' + str(len(self.theL2HLTChains)) )
        xHLTdoc.appendChild(xc)
        xc = xHLTdoc.createComment('No. of EF HLT chains:' + str(len(self.theEFHLTChains)))
        xHLTdoc.appendChild(xc)
        xc = xHLTdoc.createComment('No. of HLT chains:' + str(len(self.theHLTChains)))
        xHLTdoc.appendChild(xc)

        # menu with name
        xHLTMenu = xHLTdoc.createElement('HLT_MENU')
        xHLTdoc.appendChild(xHLTMenu)

        xHLTMenu.setAttribute('menu_name', smk_psk_Name["smkName"])
        xHLTMenu.setAttribute('prescale_set_name', smk_psk_Name["pskName"])
        self.markUsedSequences()
        
        # make xml sequences
        xSequenceList = xHLTdoc.createElement('SEQUENCE_LIST')
        for s in self.theSeqLists:
            s.xml( xSequenceList )

        xHLTMenu.appendChild(xSequenceList)

        # make xml chains
        xChainList = xHLTdoc.createElement('CHAIN_LIST')

        for c in self.theL2HLTChains:
            c.xml( xChainList )
            

        for c in self.theEFHLTChains:
            c.xml( xChainList )


        for c in self.theHLTChains:
            c.xml( xChainList )

        xHLTMenu.appendChild(xChainList)

        # write out the xml as a file
        file = open( self.__HLTFile, mode="wt" )
        logger().debug("Writing XML file for HLT")

        file.write( xHLTdoc.toprettyxml() )
        file.flush()
        file.close()
        xHLTdoc.unlink()

                    
    def writeConfigFiles(self,level="all"):
        """ Writes all config files, and checks consistency"""

        #self.alignChains(self.theL2HLTChains)
        #self.alignChains(self.theEFHLTChains)


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

        # L1 XML
        if level=="all" or level=="l1":
            self.writeL1ConfigFile(smk_psk_Name)

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

def logger():
    return logging.getLogger( "TriggerPythonConfig" )


