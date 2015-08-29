# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################
# User interface for the configuration, nothing beneath this class need to be studied
#
import os
import time
from sys import settrace
import xml.dom.minidom
from AthenaCommon.Logging import logging  # loads logger
from TrigConfigSvc.TrigLVL1configExtented import extendLVL1config

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
        self.theSeqLists       = []
        self.setMuctpiInfo(low_pt=1, high_pt=1, max_cand=13)
        self.Lvl1CaloInfo().setName('standard')
        self.Lvl1CaloInfo().setGlobalScale(1)
        #
        TriggerPythonConfig.sCurrentTriggerConfig = self
        # self.defineInternalThresholds()
        extendLVL1config(self)
        

    ##########################################################################
    # LVL1
    #
    def registerLvl1Threshold(self, name, type, mapping,
                              slot='', connector='', active=1):
        """Add LVL1 thresholds by giving the type/mapping information.
        The cable input information will be calculated from them. And store it in
        the list of available thresholds"""
        (slot, connector, bitnum, range_begin, range_end) = self.cableInput(type, mapping)
        thr = LVL1Thresholds.LVL1Threshold(name, type)
        thr.active = active
        thr.mapping = mapping
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
                           (logical_name, self.allItems[logical_name].ctpid))
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

        itemsForMenu = [item for item in self.allItems.values() if item.ctpid != -1]
        
        if not itemsForMenu:
            logger().warning('No item defined for the L1 Menu, perhaps TriggerPythonConfig.Lvl1ItemByTriggerType() ' + \
                             'is called too early (it needs to be called after Lvl1.generateMenu())')
        res = [item.name for item in itemsForMenu if (triggertypebitmask & item.trigger_type)==triggertypebit ]
        return res


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
            'TE': [1, 8, 0, 0, 3],
            'XE': [1, 8, 0, 4, 7],
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

        for i in range(0,cables[type][0]):
            slot = cables[type][1+i*4]
            connector = cables[type][2+i*4]
            offset = mapping
            for j in range(0,i):
                delta = (cables[type][4+j*4]-cables[type][3+j*4])/bitnum+1
                print 'cable has room for '+str(delta)+' thresholds.'
                offset-=delta
            range_begin = cables[type][3+i*4]+offset*bitnum
            range_end = range_begin+bitnum-1
            if range_end <= cables[type][4+i*4]:
                break            
            else:
                if i == cables[type][0]:
                    print "Untreated threshold type %s for cable assignment" % type

        print type+' threshold mapped at '+str(mapping)+' is set on SLOT: '+str(slot)+', connector: '+str(connector)+' in range: '+str(range_begin)+' - '+str(range_end)

        return ('SLOT'+str(slot), 'CON'+str(connector), bitnum, range_begin, range_end)
    #---------------------------------------------------------------------
    # Below are obsolete functions
    def addEMThreshold(self, name, value,
                       phimin='0', phimax='64', etamin='-49', etamax='49',
                       emiso='999', hadiso='999', hadveto='999',
                       cable=None, range_begin=None, range_end=None):
        """Obsolete, use addLvl1Threshold(...).addEMThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'EM')
        thr.setCableInput(3, range_begin, range_end,slot='',connector='')
        thr.addEMThresholdValue(value, etamin, etamax, phimin, phimax, \
                                emiso, hadiso, hadveto)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addTauThreshold(self, name, value,
                        phimin='0', phimax='64', etamin='-49', etamax='49',
                        emiso='999', hadiso='999', hadveto='999',
                        cable=None, range_begin=None, range_end=None):
        """Obsolete, use addLvl1Threshold(...).addTauThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'TAU')
        thr.setCableInput(3, range_begin, range_end,slot='',connector='')
        thr.addTauThresholdValue(value, etamin, etamax, phimin, phimax, \
                                 emiso, hadiso, hadveto)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addMuonThreshold(self, name, value,
                         phimin='0', phimax='64', etamin='-49', etamax='49',
                         cable=None, range_begin=None, range_end=None):
        """Obsolete, use addLvl1Threshold(...).addMuonThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'MUON')
        thr.setCableInput(3, range_begin, range_end,slot='',connector='')
        thr.addMuonThresholdValue(value, etamin, etamax, phimin, phimax)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addJetThreshold(self, name, value,
                        phimin='0', phimax='64', etamin='-49', etamax='49',
                        window='8',
                        cable=None, range_begin=None, range_end=None):
        """Obsolete, use addLvl1Threshold(...).addJetThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'JET')
        thr.setCableInput(3, range_begin, range_end,slot='',connector='')
        thr.addJetThresholdValue(value, etamin, etamax, phimin, phimax, window)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addFwdJetThreshold(self, name, value,
                           phimin='0', phimax='64', etamin='-49', etamax='49',
                           window='8',
                           cable=None, range_begin=None, range_end=None):
        """Obsolete, use addLvl1Threshold(...).addJetThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'FJET')
        thr.setCableInput(3, range_begin, range_end,slot='',connector='')
        thr.addJetThresholdValue(value, etamin, etamax, phimin, phimax, window)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addFJLThreshold(self, name, value,
                        phimin='0', phimax='64', etamin='-49', etamax='49',
                        window='8',
                        cable=None, range_begin=None, range_end=None):
        """Obsolete, use addLvl1Threshold(...).addJetThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'JB')
        thr.setCableInput(3, range_begin, range_end,slot='',connector='')
        thr.addJetThresholdValue(value, etamin, etamax, phimin, phimax, window)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addFJRThreshold(self, name, value,
                        phimin='0', phimax='64', etamin='-49', etamax='49',
                        window='8',
                        cable=None, range_begin=None, range_end=None):
        """Obsolete, use addLvl1Threshold(...).addJetThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'JF')
        thr.setCableInput(3, range_begin, range_end,slot='',connector='')
        thr.addJetThresholdValue(value, etamin, etamax, phimin, phimax, window)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addEtMissThreshold(self, name, value,
                           cable=None, range_begin=None, range_end=None):
        """Obsolete, use addLvl1Threshold(...).addEnergyThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'XE')
        thr.setCableInput(1, range_begin, range_end,slot='',connector='')
        thr.addEnergyThresholdValue(value, etamin=-49, etamax=49, \
                                    phimin=0, phimax=64)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addEtSumThreshold(self, name, value,
                          cable=None, range_begin=None, range_end=None):
        """Obsolete, use addLvl1Threshold(...).addEnergyThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'TE')
        thr.setCableInput(1, range_begin, range_end,slot='',connector='')
        thr.addEnergyThresholdValue(value, etamin=-49, etamax=49, \
                                    phimin=0, phimax=64)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addJetSumThreshold(self, name, value):
        """Obsolete, use addLvl1Threshold(...).addEnergyThresholdValue(...)"""
        thr = LVL1Thresholds.LVL1Threshold(name, 'JE')
        thr.setCableInput(1, range_begin, range_end,slot='',connector='')
        thr.addEnergyThresholdValue(value, etamin=-49, etamax=49,\
                                    phimin=0, phimax=64)
        self.theLVL1Thresholds.thresholds.append(thr)
        return self.theLVL1Thresholds.thresholds[-1]

    def addThresholdScale(self, value=None):
        self.theLVL1Thresholds.thresholdScale = LVL1Thresholds.ThresholdScale(value)

    def addLVL1Threshold(self, th):
        """Obsolete"""
        self.theLVL1Thresholds.thresholds.append(th)
        return self.theLVL1Thresholds.thresholds[-1]

    def addThreshold(self, name, value, ttype, bitnum,
                     etamin='-49', etamax='49', phimin='0', phimax='64',
                     em_isolation='999', had_isolation='999', had_veto='999', window='8'):
        """Obsolete, use addLvl1Threshold"""
        self.theLVL1Thresholds.thresholds.append(
            LVL1Thresholds.LVL1Threshold(name, value, ttype, bitnum,
                                         etamin, etamax, phimin, phimax,
                                         em_isolation, had_isolation, had_veto, window))
        return self.theLVL1Thresholds.thresholds[-1]

    def createLVL1Item(self, name, ctpid, group, prescale, priority):
        """Obsolete, use addLvl1Item()"""
        item = LVL1MenuItem(name, ctpid, group, prescale, priority)
        self.addLvl1Item(item)
        return item
    def addLVL1Item(self, it):
        """Obsolete, use addLvl1Item()"""
        self.addLvl1Item(it);
    def addItem(self, name, ctpid, group='1', prescale='1', priority='HIGH'):
        """ Obsolete, use addLvl1Item(), (Adds LVL1 Trigger items)"""
        if name in self.theLVL1MenuItems.itemNames():
            raise Exception('LVL1 Item: '+name+' already exist')
        self.theLVL1MenuItems.items.append( LVL1MenuItem(name, ctpid, group, prescale, priority))

#
# LVL1
####################################################################################################

####################################################################################################
# HLT
#

    def addHLTChain(self, chain):
        """ Adds HLT Trigger chain to the menu """
        if chain.isL2():
            self.theL2HLTChains.append(chain)
        elif chain.isEF():
            self.theEFHLTChains.append(chain)
        else:
            raise Exception( 'ERROR: Chain: ' + chain.chain_name + 'has incorrectly defined level')

    def registerHLTChain(self, logical_name, chain):
        """ Adds chain to the set of chains which are registered for further use """
        if logical_name in self.allChains.keys():
            self.allChains[logical_name].append( chain )
        else:
            self.allChains[logical_name] = [ chain ]
        
    def getHLTChain(self, name):
        """ Gets chain by name. None returned inf nothing found"""
        for  cl in self.allChains.values():
            for chain in cl:
                if chain.chain_name == name:
                    return chain
        return None

    def addSequence(self,  inTE, algolist, outTE, topo_starts_from=None):
        """ Adds HLT sequences to the menu """
        if outTE in [ s.output for s in self.theSeqLists ]:
            logger().error("There exists already sequence providing TE: "+outTE)
            logger().error("Which is: "+str([str(s) for s in self.theSeqLists if s.output == outTE]))
            logger().error('Tried to produce it from '+inTE+' ('+str([a.name() for a in algolist]))
        if outTE in self.theLVL1Thresholds.thresholdNames():
            logger().error("LVL1 theshold of name identical to output TE: "+outTE +"exists")

        seq = HLTSequence(inTE, algolist, outTE, topo_starts_from)
        self.theSeqLists.append(seq)
        return outTE
#
# HLT
####################################################################################################



    def checkMenuConsistency(self):
        """ Checks menu consistency


        Basic checks are done like chain continuity etc.
        Some of the checks are considered as a serious and therefore Exception is thrown.
        Other are just messaged as a warnings.
        """
        return self.checkChains() and self.checkSequences() and self.checkL1()

    def checkChains(self):
        correct=True
        # repeating definitions
        ids = map(lambda x: x.chain_name, self.theL2HLTChains + self.theEFHLTChains)
        repid = filter(lambda x: ids.count(x) > 1, ids)
        for id in repid:
            logger().error(" IN CHAIN: Chain of chain_name "+ str(id) +" used " + str(ids.count(id))+ " times, while can only once")
            correct=False


        counters = map(lambda x: x.chain_counter, self.theL2HLTChains)
        repcounters = filter(lambda x: counters.count(x) > 1, counters)
        for counter in repcounters:
            logger().error("IN CHAIN: L2 Chain counter "+ str(counter) + " used " + str(counters.count(counter)) + " times while can only once, will print them all")
            logger().debug( str(map( lambda x: x.chain_name,  filter(lambda x: x.chain_counter == counter, self.theL2HLTChains))) )
            correct=False

        counters = map(lambda x: x.chain_counter, self.theEFHLTChains)
        repcounters = filter(lambda x: counters.count(x) > 1, counters)
        for counter in repcounters:
            logger().error("IN CHAIN: EF Chain counter "+ str(counter) + " used " + str(counters.count(counter)) + " times while can only once, will print them all")
            logger().debug( str(map( lambda x: x.chain_name, filter(lambda x: x.chain_counter == counter, self.theEFHLTChains))) )
            correct=False

        # sufficiently defined ?
        for chain in self.theL2HLTChains + self.theEFHLTChains:
            if len(chain.trigger_type_bits) == 0:
                logger().debug( "IN CHAIN: TRIGGERTYPE bit undefined for chain: " + str(chain.chain_name)+" will use chain_counter for it: "+str(chain.chain_counter) )
                from string import atoi
                chain.trigger_type_bits = [atoi(chain.chain_counter)]

            if len(chain.stream_tag) == 0:
                logger().error( "IN CHAIN: STREAMTAG undefined for chain: " + str(chain.chain_name) )
                correct=False

        #         # is L2 matching L1
        #         if self.__L1File is not None:
        #             for chain in self.theL2HLTChains:
        #                 if chain.lower_chain_name != "":
        #                     if chain.lower_chain_name not in self.theLVL1MenuItems.itemNames():
        #                         logger().error( 'IN CHAIN: L2 chain '+chain.chain_name+' with has no matching LVL1 item between:'+\
        #                                         str(self.theLVL1MenuItems.itemNames())+\
        #                                         'while looking for ' +  str(chain.lower_chain_name) )
        #                         correct=False
        # are chain continuous
        # is L2 matching L1
        if self.__L1File is not None:
            for chain in self.theL2HLTChains:
                if chain.lower_chain_name != "":
                    lower_chains = [ x.strip(" ") for x in  chain.lower_chain_name.split(",") ]
                    #                   if chain.lower_chain_name not in self.theLVL1MenuItems.itemNames():
                    for lc in lower_chains:
                        if lc not in self.theLVL1MenuItems.itemNames():
                            logger().error( 'IN CHAIN: L2 chain '+chain.chain_name+' with has no matching LVL1 item '+ lc +   chain.lower_chain_name + '   between:'+\
                                            str(self.theLVL1MenuItems.itemNames()))
                            correct=False

        # is EF matching L2
        l2ChainNames = map ( lambda x: x.chain_name, self.theL2HLTChains )
        for chain in self.theEFHLTChains:
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
        for chain in self.theL2HLTChains + self.theEFHLTChains:
            for te in chain.getOutputTEs():
                if te not in tesInSeq:
                    logger().error( 'IN CHAIN: TE: ' + te + ' used in the chain: ' + chain.chain_name + ' not found in the sequences definitions' )
                    logger().info('Available are: '+ str(tesInSeq))
                    correct=False

        # check if all TEs needed for EF chains are produced by corresponding L2 chain
        # this cehck is ported to C++ version of the checker 

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

                                        
        
#         def activateSequencesRecursively(output):
#             if not seqDict.has_key(output):
#                 return
#             s = seqDict[output]
#             if s.used():
#                 return
#             s.use()
#             for i in s.input:
#                 activateSequencesRecursively(i)
#             return

#         for c in self.theL2HLTChains:
#             tes = []
#             for sig in c.siglist[0:]:
#                 tes.extend(sig.tes)
#             for te in tes:
#                 activateSequencesRecursively(te)

#         for c in self.theEFHLTChains:
#             tes = []
#             for sig in c.siglist[0:]:
#                 tes.extend(sig.tes)
#             for te in tes:
#                 activateSequencesRecursively(te)                
                    
    def writeConfigFiles(self):
        """ Writes all config files, but first checking consistency"""


        self.alignChains(self.theL2HLTChains)
        self.alignChains(self.theEFHLTChains)



        # disable python tracing
        from AthenaCommon.Include import excludeTracePattern
        excludeTracePattern.append("*/dom/*")
        
        # HLT XML
        xHLTdoc = xml.dom.minidom.Document()
        #        xtype = xml.dom.minidom.DocumentType("HLT_MENU")
        #        xtype.systemId = "hlt_menu.dtd"
        #        xHLTdoc.appendChild(xtype)

        # comments

        xc = xHLTdoc.createComment('File is auto-generated by python config class, may be overwritten')
        xHLTdoc.appendChild(xc)

        xc = xHLTdoc.createComment('No. of L2 HLT chains:' + str(len(self.theL2HLTChains)) )
        xHLTdoc.appendChild(xc)

        xc = xHLTdoc.createComment('No. of EF HLT chains:' + str(len(self.theEFHLTChains)))
        xHLTdoc.appendChild(xc)

        xHLTMenu = xHLTdoc.createElement('HLT_MENU')
        xHLTdoc.appendChild(xHLTMenu)

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
        xHLTMenu.appendChild(xChainList)

        # write out the xml as a file
        file = open( self.__HLTFile, mode="wt" )
        logger().debug("Writing XML file for HLT")

        file.write( xHLTdoc.toprettyxml() )
        file.flush()
        file.close()
        xHLTdoc.unlink()

        if self.__L1File is not None:

            # LVL1 XML
            xlvl1doc = xml.dom.minidom.Document()
            xlvl1config = xHLTdoc.createElement('LVL1Config')
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

            
            self.theLVL1MenuItems.xml(xlvl1config)
            self.theLVL1Thresholds.xml(xlvl1config)
            self.theCTPInfo.xml(xlvl1config)
            self.theMuctpiInfo.xml(xlvl1config)
            self.theLvl1CaloInfo.xml(xlvl1config)

            
            file = open( self.__L1File, mode="wt" )
            
            logger().debug("Writing XML file for LVL1")
            file.write( xlvl1doc.toprettyxml('  ') )
            file.close()
            xlvl1doc.unlink()


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


#########################################################################################
# LVL1

class Logic:
    NONE = ''
    AND = '&'
    OR = '|'
    NOT = '!'

    def symbolToString(x):
        if x==Logic.AND:
            return 'AND'
        elif x==Logic.OR:
            return 'OR'
        elif x==Logic.NOT:
            return 'NOT'
        else:
            return ''
    symbolToString = staticmethod(symbolToString)

    def __init__(self, condition=None):
        self.subLogic = Logic.NONE
        self.condition = '' # hold a Lvl1Condition instance
        self.subConditions = [] # holds Logic instances
        if condition!=None and condition!='':
            self.condition = condition

    def __or__(self, x):
        a = Logic()
        if self.subLogic == Logic.OR:
            a = self
        else:
            a = Logic()
            a.subLogic = Logic.OR
            a.subConditions.append(self)
        a.subConditions.append(x)
        return a
    def __and__(self, x):
        a = Logic()
        if self.subLogic == Logic.AND:
            a = self
        else:
            a = Logic()
            a.subLogic = Logic.AND
            a.subConditions.append(self)
        a.subConditions.append(x)
        return a
    def __not__(self, x):
        logger().debug('checking NOT')
        a = self
        if a.subLogic==Logic.NONE:
            a.subLogic = Logic.NOT
        if a.subLogic==Logic.NOT:
            if len(a.subConditions) == 1:
                logger().debug('not is a unary operator, ignore it')
            else:
                a.subConditions.append(x)
        return a

    def __str__(self):
        s = ''
        if self.subLogic == Logic.NONE:
            if len(self.subConditions)==1:
                s += str(self.subConditions[0])
            elif len(self.subConditions)>=1:
                logger().error('Logic NONE has more than one element')
                s += ''
            elif len(self.subConditions)==0 and self.condition!='':
                s += str(self.condition)
        elif self.subLogic == Logic.NOT:
            if len(self.subConditions)==1:
                s += '!'+str(self.subConditions[0])
            elif len(self.subConditions)>=1:
                logger().error('Logic NOT has more than one element')
                s += ''
        elif self.subLogic == Logic.AND or self.subLogic == Logic.OR:
            if len(self.subConditions)==1:
                s += str(self.subConditions[0])
            else:
                s += '('
                for (i, a) in enumerate(self.subConditions):
                    if i > 0: s += self.subLogic
                    s += str(a)
                s +=')'
        return s
    def thresholdNames(self):
        names = []
        if self.subLogic == Logic.NONE or \
               (self.condition!=None and self.condition!=''):
            if self.condition!=None and \
                   not isinstance(self.condition,LVL1MenuItem.Lvl1InternalTrigger):
                    
                if self.condition.threshold!=None and \
                       self.condition.threshold.name not in names:
                    names.append(self.condition.threshold.name)
        else:
            for a in self.subConditions:
                for b in a.thresholdNames():
                    if b not in names: names.append(b)
        return names
    def normalize(self):
        if self.subLogic in (Logic.AND, Logic.OR):
            mylogic = self.subLogic
            newconditions = []
            for c in self.subConditions:
                if c.subLogic == mylogic: # X&(A&B) or X|(A|B) 
                    # expand it to X&A&B or X|A|B
                    c.normalize()
                    newconditions.extend(c.subConditions)
                else:
                    newconditions.append(c)
            self.subConditions = newconditions
        else:
            pass
        pass
    def xml(self, node):
        if self.subLogic==Logic.NONE and \
               isinstance(self.condition, LVL1MenuItem.Lvl1InternalTrigger):
            e = xml.dom.minidom.Document().createElement('InternalTrigger')
            e.setAttribute('name', self.condition.name)
            node.appendChild(e)
        elif self.subLogic==Logic.NONE and \
             isinstance(self.condition, LVL1MenuItem.Lvl1Condition):
            e = xml.dom.minidom.Document().createElement('TriggerCondition')
            e.setAttribute('name', self.condition.threshold.name+'_x'+\
                           str(self.condition.multiplicity))
            e.setAttribute('triggerthreshold', self.condition.threshold.name)
            e.setAttribute('multi', str(self.condition.multiplicity))
            node.appendChild(e)
        elif self.subLogic==Logic.AND:
            e = xml.dom.minidom.Document().createElement('AND')
            node.appendChild(e)
            for s in self.subConditions:
                s.xml(e)
        elif self.subLogic==Logic.OR:
            e = xml.dom.minidom.Document().createElement('OR')
            node.appendChild(e)
            for s in self.subConditions:
                s.xml(e)
        elif self.subLogic==Logic.NOT:
            e = xml.dom.minidom.Document().createElement('NOT')
            node.appendChild(e)
            for s in self.subConditions:
                s.xml(e)
        else:
            logger().error('Unknown node in LVL1 item logic')
            logger().error('  ==> sublogic = ', self.subLogic)
            logger().error('  ==> # subConditions = ', len(self.subConditions))

    def printIt(self):
        for a in self.subConditions:
            if a.subLogic==a.NONE and a.condition!='':
                logger().info('subCondition :', str(a.condition))
            else:
                logger().info('subCondition :', a.printIt())
        return ''

    def dumpXML(self, indent=''):
        if self.subLogic==Logic.NONE:
            if len(self.subConditions)==0:
                logger().info(indent + '<TriggerCondition ' + str(self.condition) + '>')
        else:
            logger().info(indent + '<' + Logic.symbolToString(self.subLogic) + '>')
            for a in self.subConditions:
                a.dumpXML(indent+'  ')
            logger().info(indent + '</' + Logic.symbolToString(self.subLogic) + '>')
        return ''

    def Not(x):
        a = Logic()
        a.subLogic = Logic.NOT
        a.subConditions.append(x)
        return a
    Not = staticmethod(Not)
def Not(x):
    return Logic.Not(x)

class LVL1MenuItem:
    class Lvl1Condition:
        def __init__(self, threshold, multi):
            self.threshold = threshold
            self.multiplicity = multi
        def __str__(self):
            return str(self.threshold) + '[x' + str(self.multiplicity)+']'

    class Lvl1InternalTrigger(Lvl1Condition):
        def __init__(self, name):
            self.name = name
        def xml(self, x):
            internaltrig = xml.dom.minidom.Element('InternalTrigger')
            internaltrig.setAttribute('name', self.name)
            x.appendChild(internaltrig)
        def __str__(self):
            return str(self.name)

    def __init__(self, name, ctpid, group='1', prescale='1', priority='HIGH'):
        self.name      = name
        self.group     = group
        self.ctpid     = str(ctpid)
        self.prescale  = str(prescale)
        self.priority  = str(priority)
        self.logic = None
        self.trigger_type = 0
        
    def setLogic(self, x):
        self.logic = x
        return self

    def setCtpid(self, x):
        self.ctpid = str(x)
        return self

    def thresholdNames(self):
        if self.logic!=None:
            return self.logic.thresholdNames()
        else:
            return []

    def setTriggerType(self, n):
        ttype = int(n)
        ttype = ttype & 0xff
        self.trigger_type = ttype
        return self
    
    def addCondition(self, name, multi, threshold=None):
        """Obsolete method, use setLogic"""
        return self.addAndedCondition(name, multi, threshold)

    def addOredCondition(self, name, multi, threshold=None):
        """Obsolete method, use setLogic"""
        tm = TriggerPythonConfig.currentTriggerConfig()
        ok = False
        if tm != None:
            thr = tm.theLVL1Thresholds.thresholdOfName(name)
            if thr != None:
                if self.logic != None:
                    if self.logic.subLogic == Logic.OR or \
                           self.logic.subLogic == Logic.NONE:
                        self.logic = self.logic|Logic(thr.lvl1Condition(multi))
                        ok = True
                else:
                    self.setLogic(Logic(thr.lvl1Condition(multi)))
                    ok = True
        if not ok:
            logger().error('Error while trying to add ORed Condition')
            if self.logic==None:
                logger().error('Logic is not defined for this item')
            else:
                logger().error('Already setup logic: ')
                self.logic.printIt()
            logger().error('Tried to add ORed condition of ' + threshold + \
                  " multi=" + str(multi))
        return self

    def addAndedCondition(self, name, multi, threshold=None):
        """Obsolete method, use setLogic"""
        tm = TriggerPythonConfig.currentTriggerConfig()
        ok = False
        if tm != None:
            thr = tm.theLVL1Thresholds.thresholdOfName(name)
            if thr != None:
                if self.logic != None:
                    if self.logic.subLogic == Logic.AND or \
                           self.logic.subLogic == Logic.NONE:
                        self.logic = self.logic|Logic(thr.lvl1Condition(multi))
                        ok = True
                else:
                    self.setLogic(Logic(thr.lvl1Condition(multi)))
                    ok = True
        if not ok:
            logger().error('Error while trying to add ANDed Condition')
            if self.logic==None:
                logger().error('Logic is not defined for this item')
            else:
                logger().error('Already setup logic: ')
                self.logic.printIt()
            logger().error('Tried to add ANDed condition of %s multi=%s' % (threshold, multi))
        return self

    def binary_trigger_type(self, x):
        TT=''
        for i in range(0,8):
            if(x%2==1):
                TT='1'+TT
            else:
                TT='0'+TT
            x=x/2
        return TT

    def xml(self, xmenu):
        xtriggeritem = xml.dom.minidom.Document().createElement('TriggerItem')
        xtriggeritem.setAttribute('name', self.name)
        xtriggeritem.setAttribute('ctpid', self.ctpid)
        xtriggeritem.setAttribute('version', '1')
        xtriggeritem.setAttribute('definition', '(1)')
        xtriggeritem.setAttribute('trigger_type', self.binary_trigger_type(self.trigger_type))
        xtriggeritem.setAttribute('comment','test comment')
        xtriggeritem.setAttribute('group','0')
        xmenu.appendChild(xtriggeritem);
        if self.logic!=None:
            self.logic.normalize()
            self.logic.xml(xtriggeritem)

class LVL1MenuItems:
    def __init__(self):
        self.items = []

    def itemNames(self):
        return map( lambda x: x.name, self.items )

    def findItemByName(self, name):
        items = filter(lambda x: x.name==name, self.items)
        if len(items)==0: return None
        else: return items[0]

    def findItemByCtpid(self, ctpid):
        items = filter(lambda x: x.ctpid==ctpid, self.items)
        if len(items)==0: return None
        else: return items[0]

    def addItem(self, itemname, mult='1', prescale='1', mask='on', \
                threshold=None):
        self.items.append( LVL1MenuItem(itemname, mult, prescale, mask, threshold) )

    def getNMDPS(self, PS):
        # floatPS is the raw float as entered into the python. This returns n | m | d where
        # n is a 24bit positive number INTEGER PRESCALE
        # m,d are 4bit positive integer used for fractional prescale.
        # The fractional prescale is calculated as: f = n * (m+d+1)/(m+1)
        # List with all possible prescaleValues between 1 and 2. Used to calculate the
        # N,M,D combination out of a float prescale.
        L1PSNumberAuxValues = [[15,1],[14,1],[13,1],[12,1],[11,1],[10,1],[9,1],[8,1],[15,2],
                               [14,2],[13,2],[12,2],[11,2],[10,2],[15,3],[14,3],[13,3],[8,2],
                               [12, 3],[15, 4],[14, 4],[10, 3],[13, 4],[9, 3],[12, 4],[15, 5],
                               [14, 5],[13, 5],[10, 4],[15, 6],[12, 5],[14, 6],[11, 5],[13, 6],
                               [15, 7],[8, 4],[10, 5],[12, 6],[14, 7],[15, 8],[14, 8],[12, 7],
                               [10, 6],[8, 5],[15, 9],[13, 8],[11, 7],[14, 9],[12, 8],[15, 10],
                               [10, 7],[13, 9],[14, 10],[15, 11],[12, 9],[9, 7],[13, 10],[10, 8],
                               [14, 11],[15, 12],[12, 10],[8, 7],[13, 11],[14, 12],[15, 13],[10, 9],
                               [11, 10],[12, 11],[13, 12],[14, 13],[15, 14],[8, 8],[9, 9],[10, 10],
                               [11, 11],[12, 12],[13, 13],[14, 14],[15, 15],
                              ]

        floatPS = float(PS)
        # Only negative prescale allowed is -1 from Python
        if (floatPS < 0.0):
            n = -1
            m = 0
            d = 0
            f = -1
            return n,m,d,f

        # Now find n, m, d and allowed float ps for any float provided
        floatPScorr = 1.0
        tmpN = int(floatPS)
        tmpM = 0
        tmpD = 0
        diff = floatPS - tmpN
        psIdx = -1
        i=-1
        if (floatPS - tmpN != 0):
            if (floatPS <= 1):
                tmpN = 1
            else:
                for entry in L1PSNumberAuxValues: #pair is mm, dd
                    i += 1
                    floatPScorr = ( float(entry[0] + entry[1] + 1.0)) / (float(entry[0] + 1))
                    tmpF = tmpN * floatPScorr
                    if (tmpF - tmpN < 1.0):
                        tmpDiff = abs(floatPS - tmpF)
                        if (tmpDiff < diff):
                            diff = tmpDiff
                            psIdx = i
                        else:
                            break
                if psIdx >= 0:
                    tmpM = (L1PSNumberAuxValues[psIdx])[0]
                    tmpD = (L1PSNumberAuxValues[psIdx])[1]
                    #floatPScorr = tmpN * (float(tmpM + tmpD + 1.0)) / (float(tmpM + 1))
                    floatPScorr = float(tmpM + tmpD + 1.0) / (float(tmpM + 1.0))
                else:
                    floatPScorr = 1

        return tmpN, tmpM, tmpD, tmpN*float(tmpM+tmpD+1.0)/(float(tmpM+1.0))

    def xml(self, xconfig):
        xtriggermenu = xml.dom.minidom.Document().createElement('TriggerMenu')
        xtriggermenu.setAttribute('id', idgen.get('TriggerMenu'))
        xtriggermenu.setAttribute('name','lumi01')
        xtriggermenu.setAttribute('version','1')
        xtriggermenu.setAttribute('phase','lumi')
        xconfig.appendChild(xtriggermenu)

        for i in self.items:
            i.xml(xtriggermenu)
        # write prescale array
        prescaleSet = map(lambda x: '1', range(256)) #1, 256))
        for i in map( lambda x: (x.ctpid, x.prescale), self.items ):
            prescaleSet[int(i[0])] = i[1]
        xprescaleset = xml.dom.minidom.Element('PrescaleSet')
        xprescaleset.setAttribute('name','standard')
        xprescaleset.setAttribute('version','1')
        for i in range(256):
            prescale = xml.dom.minidom.Element('Prescale')
            # get float ps and convert to n,m,d and corrected float
            floatPS = prescaleSet[i]
            n,m,d,floatPScorr = self.getNMDPS(floatPS)
            prescale.setAttribute('ctpid', str(i))
            prescale.setAttribute('n', str(n))
            prescale.setAttribute('m', str(m))
            prescale.setAttribute('d', str(d))
            #
            xtext = xml.dom.minidom.Text()
            xtext.replaceWholeText(str(floatPScorr))
            prescale.appendChild(xtext)
            xprescaleset.appendChild(prescale)
        xconfig.appendChild(xprescaleset)

        # write priority array
        prioritySet = map(lambda x: 'LOW', range(256)) # 1, 256))
        for i in map( lambda x: (x.ctpid, x.priority), self.items ):
            prioritySet[int(i[0])] = i[1]
        xpriorityset = xml.dom.minidom.Element('PrioritySet')
        xpriorityset.setAttribute('name', 'standard')
        xpriorityset.setAttribute('version', '1')
        for i in range(256):
            priority = xml.dom.minidom.Element('Priority')
            priority.setAttribute('ctpid', str(i))
            xtext = xml.dom.minidom.Text()
            xtext.replaceWholeText(str(prioritySet[i]))
            priority.appendChild(xtext)
            xpriorityset.appendChild(priority)
        xconfig.appendChild(xpriorityset)

##############################################################################
# LVL1 Threshold classes
# Maximum values for calo thresholds to disable that threshold
#
ClusterOff   = 255
IsolationOff = 63
JetOff       = 1023
EtSumOff     = 2047
EtMissOff    = 2895
JetEtOff     = 13286
# InternalTriggers
bgrp0 = LVL1MenuItem.Lvl1InternalTrigger('BGRP0')
bgrp1 = LVL1MenuItem.Lvl1InternalTrigger('BGRP1')
bgrp2 = LVL1MenuItem.Lvl1InternalTrigger('BGRP2')
bgrp3 = LVL1MenuItem.Lvl1InternalTrigger('BGRP3')
bgrp4 = LVL1MenuItem.Lvl1InternalTrigger('BGRP4')
bgrp5 = LVL1MenuItem.Lvl1InternalTrigger('BGRP5')
bgrp6 = LVL1MenuItem.Lvl1InternalTrigger('BGRP6')
bgrp7 = LVL1MenuItem.Lvl1InternalTrigger('BGRP7')
rndm0 = LVL1MenuItem.Lvl1InternalTrigger('RNDM0')
rndm1 = LVL1MenuItem.Lvl1InternalTrigger('RNDM1')
pclk0 = LVL1MenuItem.Lvl1InternalTrigger('PCLK0')
pclk1 = LVL1MenuItem.Lvl1InternalTrigger('PCLK1')

class LVL1Thresholds:
    def cableMapping(type, range_begin):
        """Calculate the mapping of the input cable of the threshold to the
        CTP from its (type, range_begin)"""
        mapping = -1
        if type == 'MUON':
            mapping = range_begin/3
        elif type=='EM':
            mapping = range_begin/3
        elif type=='TAU':
            mapping = range_begin/3
        elif type=='JET':
            mapping = range_begin/3
        elif type=='JE':
            mapping = range_begin-24
        elif type=='JB' or type=='FJL':
            mapping = range_begin/2
        elif type=='JF' or type=='FJR':
            mapping = (range_begin-8)/2
        elif type=='TE':
            mapping = range_begin
        elif type=='XE':
            mapping = range_begin-4
        return mapping
    cableMapping = staticmethod(cableMapping)
    
    class ThresholdScale:
        def __init__(self, value=None):
            self.value = '1.0'
            if value is not None: self.value = value

        def xml(self):
            line='<ThresholdScale caloscale="'
            line+=self.value
            line+='"/>'
            return line


    class ThresholdValue:
        #
        def __init__(self, type, value, etamin, etamax, phimin, phimax, \
                     em_isolation, had_isolation, had_veto, \
                     window, priority, name=''):
            self.name = name
            self.type = type
            self.value = value
            self.etamin = etamin
            self.etamax = etamax
            self.phimin = phimin
            self.phimax = phimax
            self.em_isolation = em_isolation
            self.had_isolation = had_isolation
            self.had_veto = had_veto
            self.window = window
            self.priority = priority

        def xml(self, xlist):
            e = xml.dom.minidom.Element('TriggerThresholdValue')
            e.setAttribute('name', self.name)
            e.setAttribute('type', self.type)
            e.setAttribute('thresholdval', str(self.value))
            e.setAttribute('etamin', str(self.etamin))
            e.setAttribute('etamax', str(self.etamax))
            e.setAttribute('phimin', str(self.phimin))
            e.setAttribute('phimax', str(self.phimax))
            e.setAttribute('em_isolation', str(self.em_isolation))
            e.setAttribute('had_isolation', str(self.had_isolation))
            e.setAttribute('had_veto', str(self.had_veto))
            e.setAttribute('window', str(self.window))
            e.setAttribute('priority', str(self.priority))
            xlist.appendChild(e)

    class LVL1Threshold:
        #
        def __init__(self, name, ttype):
            self.name   = name
            self.ttype  = ttype
            self.thresholdValues = []
            self.bitnum = 0
            self.active = 1
            self.mapping = -1
            self.cable = ''
            self.range_begin = -1
            self.range_end = -1
            self.slot = ''
            self.connector = ''
            self.type2cable    = {'MUON': 'MU',
                                  'EM': 'CP1',
                                  'TAU': 'CP2',
                                  'JET':'JEP1',
                                  'JE':'JEP1',
                                  'FJET':'JEP2',
                                  'JB':'JEP2',
                                  'JF':'JEP2',
                                  'XE':'JEP3',
                                  'TE':'JEP3',
                                  'LUCID': 'NIM',
                                  'TRT': 'NIM',
                                  'ZDC': 'NIM',
                                  'BCM': 'NIM',
                                  'BCMCMB': 'NIM',
                                  'MBTSSI': 'NIM',
                                  'MBTS': 'NIM',
                                  'CALREQ': 'NIM',
                                  'NIM': 'NIM'}

            if self.cable is None or self.cable=='':
                if ttype in self.type2cable.keys():
                    cable = self.type2cable[ttype]
                    self.cable = cable

        def __str__(self):
            return self.name

        def lvl1Condition(self, multi):
            return LVL1MenuItem.Lvl1Condition(self, multi)

        def condition(self, multi=1):
            return Logic(self.lvl1Condition(multi))

        def setCableInput(self, bitnum, range_begin, range_end, \
                          slot, connector):
            self.bitnum = int(bitnum)
            self.range_begin = int(range_begin)
            self.range_end = int(range_end)
            self.slot = slot
            self.connector = connector
            return self

        def addMuonThresholdValue(self, value, \
                                  etamin, etamax, phimin, phimax, \
                                  priority=0):
            thrv = LVL1Thresholds.ThresholdValue(self.ttype, value, \
                                                 etamin, etamax, phimin, phimax, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 0, priority, self.name+'full')
            self.thresholdValues.append(thrv)
            return self

        def addCPThresholdValue(self, value, etamin, etamax, phimin, phimax, \
                                em_isolation, had_isolation, had_veto,
                                priority=1):
            thrv = LVL1Thresholds.ThresholdValue(self.ttype, value, \
                                                 etamin, etamax, phimin, phimax, \
                                                 em_isolation, had_isolation, had_veto, \
                                                 0, priority, self.name+'full')
            self.thresholdValues.append(thrv)
            return self

        def addEMThresholdValue(self, value, etamin, etamax, phimin, phimax, \
                                em_isolation, had_isolation, had_veto,
                                priority=1):
            return self.addCPThresholdValue(value, etamin, etamax, \
                                            phimin, phimax,
                                            em_isolation, had_isolation,
                                            had_veto, priority)

        def addTauThresholdValue(self, value, etamin, etamax, phimin, phimax, \
                                 em_isolation, had_isolation, had_veto,
                                 priority=1):
            return self.addCPThresholdValue(value, etamin, etamax, \
                                            phimin, phimax,
                                            em_isolation, had_isolation,
                                            had_veto, priority)

        def addJetThresholdValue(self, value, etamin, etamax, phimin, phimax, \
                                 window=8, priority=0):
            thrv = LVL1Thresholds.ThresholdValue(self.ttype, value, \
                                                 etamin, etamax, phimin, phimax, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 window, priority, self.name+'full')
            self.thresholdValues.append(thrv)
            return self

        def addEnergyThresholdValue(self, value, \
                                etamin, etamax, phimin, phimax, \
                                priority=0):
            thrv = LVL1Thresholds.ThresholdValue(self.ttype, value, \
                                                 etamin, etamax, phimin, phimax, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 0, priority, self.name+'full')
            self.thresholdValues.append(thrv)
            return self

        def thresholdInGeV(self):
            if len(self.thresholdValues)==0:
                return 0
            else:
                return float(self.thresholdValues[0].value)

        def xml(self, xlist):
            """ Returns XML representation of the LVL1 Threshold """
            sThresholdTypes = ('MUON',
                               'EM', 'TAU',
                               'JET', 'JE',
                               'JB', 'JF', 
                               'TE', 'XE',
                               'LUCID', 'TRT', 'ZDC', 'BCM', 'BCMCMB', 'MBTS', 'MBTSSI', 'CALREQ','NIM',
                               )
            sInternalThresholdTypes = ('RNDM', 'BGRP', 'PCLK')
            if self.ttype in sThresholdTypes or \
                   self.ttype in sInternalThresholdTypes:
                xtth = xml.dom.minidom.Element('TriggerThreshold')
                xtth.setAttribute('id', idgen.get('TriggerThreshold'))
                xtth.setAttribute('version', '1')
                xtth.setAttribute('name', self.name)
                xtth.setAttribute('type', self.ttype)
                xtth.setAttribute('active', str(self.active))
                xtth.setAttribute('mapping', str(self.mapping))
                xtth.setAttribute('bitnum', str(self.bitnum))
                for thrv in self.thresholdValues:
                    thrv.xml(xtth)
                xcable = xml.dom.minidom.Element('Cable')
                xcable.setAttribute('name', self.cable)
                xcable.setAttribute('ctpin', self.slot)
                xcable.setAttribute('connector', self.connector)
                xtth.appendChild(xcable)

                xsignal = xml.dom.minidom.Element('Signal')
                xsignal.setAttribute('range_begin', str(self.range_begin))
                xsignal.setAttribute('range_end',  str(self.range_end))
                xcable.appendChild(xsignal)
                xlist.appendChild(xtth)
            else:
                logger().error('Trigger type not defined: ', self.ttype)

    def compThreshold(thr1, thr2):
        mapping1, mapping2 = thr1.mapping, thr2.mapping
        # First sort by mapping if it's not -1
        if mapping1>=0 and mapping2>0:
            if mapping1 < mapping2: return -1
            if mapping1 > mapping2: return 1
            else: return 0
        elif mapping1>=0: return -1
        elif mapping2>=0: return 1
        else:
            # Second sort by (threshold value and then threshold name)
            import re
            thrv1, thrv2 = 0, 0
            re_thrv = re.compile('(\d+)')
            mg = re_thrv.search(thr1.name)
            if mg: thrv1 = int(mg.group(1))
            mg = re_thrv.search(thr2.name)
            if mg: thrv2 = int(mg.group(1))
            #
            if thrv1 < thrv2: return -1
            elif thrv1 > thrv2: return 1
            else:
                if thr1.name < thr2.name: return -1
                elif thr1.name > thr2.name: return 1
                else: return 0
    compThreshold = staticmethod(compThreshold)
    
    def __init__(self):
        self.thresholds = []
        self.thresholdScale = LVL1Thresholds.ThresholdScale()
    def thresholdNames(self):
        return map(lambda x: x.name, self.thresholds)

    def allThresholdsOf(self, type):
        # thr_types = ('MUON', 'EM', 'TAU', 'JET', 'FJET', 'JE', 'TE', 'XE')
        thr_types = ('MUON', 'EM', 'TAU', 'JET', 'JB','JF', 'JE', 'TE', 'XE',
                     'FJL','FJR')
        thrs = []
        if type in thr_types:
            for thr in self.thresholds:
                if thr.ttype == type: thrs.append(thr)
        return thrs

    def thresholdOfName(self, name):
        thr = filter(lambda x: x.name==name, self.thresholds)
        if len(thr)==1:
            return thr[0]
        else:
            return None
    def thresholdOfTypeMapping(self, type, mapping):
        thrs = filter(lambda x: (x.ttype==type and x.mapping==mapping), \
                      self.thresholds)
        if len(thrs)==1:
            return thrs[0]
        elif len(thrs)>1:
            s = 'More than one (%d) LVL1 thresholds of type %s with' % \
                (len(thrs), type)
            s = '%s mapping %d found' % (s, mapping)
            logger().warning(s)
            return thrs[0]
        else:
            return None
            

    def xml(self, xconfig):
        xttlist = xml.dom.minidom.Element('TriggerThresholdList')
        self.thresholds.sort(LVL1Thresholds.compThreshold)
        type_map = []
        for th in self.thresholds:
            mapping = th.mapping
            if mapping==-1:
                mapping = LVL1Thresholds.cableMapping(th.ttype, th.range_begin)
            tm = (th.ttype, mapping)
            if tm not in type_map:
                type_map.append(tm)
                th.xml(xttlist)
            else:
                s = 'Ignoring LVL1 threshold %s of type=%s, mapping=%d' % \
                    (th.name, th.ttype, mapping)
                s = '%s since this position was already occupied' % s
                logger().warning(s)
                print tm 
        xconfig.appendChild(xttlist)

class CTPInfo:
    class Random:
        def __init__(self):
            self.name = ''
            self.rate1 = 0
            self.rate2 = 0
        def set(self, name, rate1, rate2, seed1, seed2):
            self.name = name
            self.rate1 = rate1
            self.rate2 = rate2
            self.seed1 = seed1
            self.seed2 = seed2
        def xml(self, x):
            xrandom = xml.dom.minidom.Element('Random')
            xrandom.setAttribute('name', self.name)
            xrandom.setAttribute('rate1', str(self.rate1))
            xrandom.setAttribute('rate2', str(self.rate2))
            xrandom.setAttribute('seed1', str(self.seed1))
            xrandom.setAttribute('seed2', str(self.seed2))
            x.appendChild(xrandom)
    class PrescaledClock:
        def __init__(self):
            self.name = ''
            self.clock1 = 0
            self.clock2 = 0
        def set(self, name, clock1, clock2):
            self.name = name
            self.clock1 = clock1
            self.clock2 = clock2
        def xml(self, x):
            xpc = xml.dom.minidom.Element('PrescaledClock')
            xpc.setAttribute('name',   self.name)
            xpc.setAttribute('version', '1')
            xpc.setAttribute('clock1', str(self.clock1))
            xpc.setAttribute('clock2', str(self.clock2))
            x.appendChild(xpc)
    class Deadtime:
        def __init__(self):
            self.name = ''
            self.simple = 0
            self.complex1_rate = 0
            self.complex1_level = 0
            self.complex2_rate = 0
            self.complex2_level = 0
        def set(self, name, simple, complex1_rate, complex1_level, \
                complex2_rate, complex2_level):
            self.name = name
            self.simple = simple
            self.complex1_rate = complex1_rate
            self.complex1_level = complex1_level
            self.complex2_rate = complex2_rate
            self.complex2_level = complex2_level
        def xml(self, x):
            xdt = xml.dom.minidom.Element('Deadtime')
            xdt.setAttribute('name', self.name)
            xdt.setAttribute('simple', str(self.simple))
            xdt.setAttribute('complex1_rate', str(self.complex1_rate))
            xdt.setAttribute('complex1_level', str(self.complex1_level))
            xdt.setAttribute('complex2_rate', str(self.complex2_rate))
            xdt.setAttribute('complex2_level', str(self.complex2_level))
            xdt.setAttribute('version','1')
            x.appendChild(xdt)
    class BunchGroupSet:
        class BunchGroup:
            def __init__(self):
                self.name = ''
                self.internalNumber = 0
                self.bunches = []
            def set(self, name, internalNumber, bunches):
                self.name = name
                self.internalNumber = internalNumber
                self.bunches = bunches
            def xml(self, x):
                xbg = xml.dom.minidom.Element('BunchGroup')
                xbg.setAttribute('name', self.name)
                xbg.setAttribute('version', '1')
                xbg.setAttribute('internalNumber', str(self.internalNumber))
                for b in self.bunches:
                    xb = xml.dom.minidom.Element('Bunch')
                    xb.setAttribute('bunchNumber', str(b))
                    xbg.appendChild(xb)
                x.appendChild(xbg)
        def __init__(self):
            self.name= ''
            self.bunchGroups = []
        def set(self, name):
            self.name = name
        def addBunchGroup(self, name, internalNumber, bunches):
            bg = CTPInfo.BunchGroupSet.BunchGroup()
            bg.set(name, internalNumber, bunches)
            self.bunchGroups.append(bg)
            return self
        def xml(self, x):
            xbgs = xml.dom.minidom.Element('BunchGroupSet')
            xbgs.setAttribute('name', self.name)
            xbgs.setAttribute('version','1')
            for bg in self.bunchGroups:
                bg.xml(xbgs)
            x.appendChild(xbgs)
    class TriggerType:
        def __init__(self):
            self.bits = [ 0, 0, 0, 0, 0, 0, 0, 0]
        def setBit(self, bit, x):
            if bit in range(0, 8):
                self.bits[bit] = x
            else:
                logger().error('Warning: tried to access bit '+str(bit)+\
                               " in LVL1 trigger type")
        def xml(self, x):
            xtt = xml.dom.minidom.Element('TriggerType')
            text = 'TriggerType bits: each bit mapped to TriggerItem bit[255 <-> 0]'
            c = xml.dom.minidom.Comment(text)
            xtt.appendChild(c)
            for bit in range(0, 8):
                text = 'bit'+str(bit)
                xbit = xml.dom.minidom.Element(text)
                text = xml.dom.minidom.Text()
                s0 = hex(self.bits[bit])
                if s0.startswith('0x'): s0 = s0[2:]
                if s0.endswith('L'): s0 = s0[:-1]
                n = len(s0)
                if n > 64:
                    logger().error('TriggerType bit has more than 256 bits, '+len(s0))
                    logger().error('Stripping off higher bits and save only least 256 bits')
                    s0 = s0[n-64:]
                elif n < 64:
                    s1 = '0000000000000000000000000000000000000000000000000000000000000000'
                    s1 = s1[n:]
                    s0 = s1 + s0
                s = ''
                for i in range(0, 8):
                    s += s0[i*8:(i+1)*8]
                    if i != 7: s += ' '
                text.replaceWholeText(s)
                xbit.appendChild(text)
                xtt.appendChild(xbit)
            x.appendChild(xtt)
    # Members of CTPInfo
    def __init__(self):
        self.random = CTPInfo.Random()
        self.prescaledClock = CTPInfo.PrescaledClock()
        self.deadtime = CTPInfo.Deadtime()
        self.bunchGroupSet = CTPInfo.BunchGroupSet()
        self.triggerType = CTPInfo.TriggerType()
    def setRandom(self, name, rate1, rate2, seed1, seed2):
        self.random.set(name, rate1, rate2, seed1, seed2)
    def setPrescaledClock(self, name, clock1, clock2):
        self.prescaledClock.set(name, clock1, clock2)
    def setDeadtime(self, name, simple, complex1_rate, \
                    complex1_level, complex2_rate, complex2_level):
        self.deadtime.set(name, simple, complex1_rate, complex1_level, \
                          complex2_rate, complex2_level)
    def setBunchGroupSet(self, name):
        self.bunchGroupSet.set(name)
        return self.bunchGroupSet
    def addBunchGroup(self, name, internalNumber, bunches):
        self.bunchGroupSet.addBunchGroup(name, internalNumber, bunches)
    def setTriggerType(self, bit, value):
        self.triggerType.setBit(bit, value)
    def xml(self, x):
        self.random.xml(x)
        self.deadtime.xml(x)
        self.bunchGroupSet.xml(x)
        self.prescaledClock.xml(x)
        # self.triggerType.xml(x)

class MuctpiInfo:
    def __init__(self):
        self.low_pt = 0
        self.high_pt = 0
        self.max_cand = 0
    def set(self, low_pt, high_pt, max_cand):
        self.low_pt = low_pt
        self.high_pt = high_pt
        self.max_cand = max_cand
    def xml(self, xconfig):
        xmuctpi = xml.dom.minidom.Element('MuctpiInfo')
        low_pt = xml.dom.minidom.Element('low_pt')
        text = xml.dom.minidom.Text()
        text.replaceWholeText(str(self.low_pt))
        low_pt.appendChild(text)
        high_pt = xml.dom.minidom.Element('high_pt')
        text = xml.dom.minidom.Text()
        text.replaceWholeText(str(self.high_pt))
        high_pt.appendChild(text)
        max_cand = xml.dom.minidom.Element('max_cand')
        text = xml.dom.minidom.Text()
        text.replaceWholeText(str(self.max_cand))
        max_cand.appendChild(text)
        xmuctpi.setAttribute('name', 'muctpi')
        xmuctpi.setAttribute('version', '1')
        xmuctpi.appendChild(low_pt)
        xmuctpi.appendChild(high_pt)
        xmuctpi.appendChild(max_cand)
        xconfig.appendChild(xmuctpi)

class Lvl1CaloInfo:
    AverageOfThr = 1
    LowerThr = 2
    HigherThr = 3
    def __init__(self):
        self.name = ''
        self.globalScale = 1
        self.jetWeights = [0]*12
    def setName(self, name):
        self.name = name
    def setGlobalScale(self, x):
        self.globalScale = x
    def setJetWeights(self, weights):
        self.jetWeights = weights
    def calcJetWeights(lvl1_thresholds, option=AverageOfThr):
        ws = []
        thrvs = []
        thrs = lvl1_thresholds.allThresholdsOf('JET')
        for t in thrs:
            thrvs.append(t.thresholdInGeV())
        if option==Lvl1CaloInfo.AverageOfThr:
            ws = thrvs
        elif option==Lvl1CaloInfo.LowerThr:
            ws = thrvs
        elif option==Lvl1CaloInfo.HigherThr:
            ws = thrvs
        return ws
    calcJetWeights = staticmethod(calcJetWeights)
    def xml(self, x):
        xcal = xml.dom.minidom.Element('CaloInfo')
        xcal.setAttribute('global_scale', str(self.globalScale))
        xcal.setAttribute('name', 'CaloInfo')
        xcal.setAttribute('version', '1')
        for (i, w) in enumerate(self.jetWeights):
            jw = xml.dom.minidom.Element('JetWeight')
            jw.setAttribute('num', str(i+1))
            text = xml.dom.minidom.Text()
            text.replaceWholeText(str(w))
            jw.appendChild(text)
            xcal.appendChild(jw)
        x.appendChild(xcal)
#
# EOF LVL1 classes
#########################################################################################



#########################################################################################
# HLT Chain classes
#
class HLTChain:
    """ the HLT Chain definition class """
    class HLTSignature:
        """ HLT Signature, internal class of HLT Chain """
        def __init__(self, telist, sigcounter, logic='1'):
            self.sigcounter = sigcounter
            if type(telist) != type([]):
                self.tes    = [ telist ]
            else:
                self.tes    = telist
            self.logic      = logic


        def __eq__(self, other):
            if self.tes == other.tes:
                return True
            return False

        def multiplicity(self):
            return len(self.tes)

        def uniqueTEs(self):
            return len(self.tes)

        def xml(self, xlist):
            if len(self.tes) != 0:
                xSignature = xml.dom.minidom.Document().createElement('SIGNATURE')
                xlist.appendChild(xSignature)
                xSignature.setAttribute('logic','1')
                xSignature.setAttribute('signature_counter', str(self.sigcounter))

                for te in self.tes:
                    if type(te) != type(''): # check if this is a string
                        raise Exception("The trigger element: " + str(te) + " in the signature: " + self.sigcaounter + "is not a plain string" )
                    xTriggerElement = xml.dom.minidom.Document().createElement('TRIGGERELEMENT')
                    xTriggerElement.setAttribute('te_name', te)
                    xSignature.appendChild(xTriggerElement)
    # construction
    def __init__(self, chain_name, chain_counter,
                 lower_chain_name, level, prescale='1', pass_through='0', rerun_prescale='-1'):
        self.chain_name       = chain_name
        self.chain_counter  = chain_counter
        self.level          = level
        self.siglist        = []
        self.prescale       = prescale
        self.rerun_prescale = rerun_prescale
        self.pass_through   = pass_through
        self.lower_chain_name = lower_chain_name
        self.stream_tag     = []
        self.trigger_type_bits = []
        self.groups           = []
        self.chainBindings = []
        self.chains_to_merge   = []
        self.sigs_n_before_merge   = 0
        
    def addHLTSignature(self, telist, sigcounter=None, logic='1'):
        if sigcounter is None:       # figure out sigcounter if not specified
            if len(self.siglist) == 0:
                sigcounter = 1
            else:
                sigcounter =  self.siglist[-1].sigcounter+1
        else:
            if  len(self.siglist) != 0:
                if sigcounter >= self.siglist[-1].sigcounter:
                    sigcounter =  self.siglist[-1].sigcounter+1
                    
        self.siglist.append( HLTChain.HLTSignature(telist, sigcounter, logic) )
        return self

    def getHLTSignatureOfStep(self, step):
        """ Returns signature at given step

        If no signature at given step is present then method recursively searches back.
        If step 0 is reached and no signature is found then empty one is returned.
        """
        for sig in self.siglist:
            if sig.sigcounter == step:
                return sig
        if step == 0:
            return HLTChain.HLTSignature([], step)
        else:
            return self.getHLTSignatureOfStep(step -1)

    def mergeAndAppendChains(self, chains_list):
        """ Helps definig complex chains made out of other chains.

        All chains from the chains_list are scanned and thier signatures are merged and appended to the list of existing signatures.
        """
        self.chains_to_merge = chains_list
        self.sigs_n_before_merge = len(self.siglist)

    def doChainsMerging(self):
        if type(self.chains_to_merge) != type([]):
            logger().error( "Not a chains list given to mergeAndAppendChains for chain: "+ self.chain_name+ " it is: "+ str(type(self.chains_to_merge)) )
            raise Exception("Incorrect usage of: mergeAndAppendChains")

        logger().debug("merging chains:"+str([ c.chain_name for c in self.chains_to_merge]))

        copysigs = self.siglist
        self.siglist = copysigs[0:self.sigs_n_before_merge]

        # find out counter of last signature
        last_counter = 1
        if len(self.siglist) != 0:
            last_counter = self.siglist[-1].sigcounter
            logger().debug( "The chain: " + self.chain_name + " contains already signatures. Will align sinatures to it." )


        max_counter = 0;
        for chain in self.chains_to_merge:
            if len(chain.siglist) != 0:
                max_counter = max(chain.siglist[-1].sigcounter, max_counter)
                logger().debug("Max counter in merged chains "+str(max_counter))
            else:
                logger().debug("Chain: "+ chain.chain_name + " is having empty signature list wil lbe ignored in merging")
                del self.chains_to_merge[self.chains_to_merge.index(chain)]


        # make dictionary with TE sigantures
        for i in range(1, max_counter+1):
            telist = []
            [ telist.extend(x) for x in [chain.getHLTSignatureOfStep(i).tes for chain in self.chains_to_merge]] # repack TEs from other chain signatures
            self.addHLTSignature(telist, sigcounter=i)

        # now append all signatures which were added after merging
        for sig in copysigs[self.sigs_n_before_merge:]:
            max_counter = max_counter+1
            self.addHLTSignature(sig.tes, sigcounter=max_counter)

    def continueOn(self, chain):
        self.siglist = chain.siglist
        return self

    def evaluateSignatureAfter(self, mySig, otherChain, otherChainSig):
        """ Adds chain binding. Note that at this point action is only scheduled. Real work is done be alignChain method """
        if type(otherChain) == type(''):
            self.chainBindings.append( (mySig, otherChain, otherChainSig) )
        else:
            self.chainBindings.append( (mySig, otherChain.chain_name, otherChainSig) )
#        self.alignChain(mySig, otherChain, otherChainSig)

    def alignChain(self, mySig, otherChain, otherChainSigName ):
        """ Changes internals of this chains and makes sure that selected signatures of other chain are evaluated first """
        # check if any binding is required
        if len(self.chainBindings) == 0:
            return False

        # find this chain signature
        thisChainSig = filter(lambda x: x.tes == HLTChain.HLTSignature(mySig, 0).tes, self.siglist)
        if len(thisChainSig) == 0:
            logger().debug( "Chains binding ineffective (1), can't find signature: " + mySig + " in chain " + self.chain_name )
            return False
        thisChainSig = thisChainSig[0] # unpack it from list

        # find sigcounter of the otherChainSig
        otherChainSig = filter(lambda x: x.tes == HLTChain.HLTSignature(otherChainSigName, 0).tes, otherChain.siglist)
        if len(otherChainSig) == 0:
            logger().debug( "Chains binding ineffective (2), can't find signature: " + str(otherChainSigName) + " in chain " + otherChain.chain_name )
            return False

        otherChainSig = otherChainSig[0]

        # fianlly we have both, check if we need to do anything
        if thisChainSig.sigcounter > otherChainSig.sigcounter:
            logger().debug( "Chains binding ineffective (3), signatures steps already ordered" )
            return False

        # and real work to be done here
        counterShift = otherChainSig.sigcounter - thisChainSig.sigcounter + 1
        for i in range (self.siglist.index(thisChainSig), len(self.siglist)):
            self.siglist[i].sigcounter = self.siglist[i].sigcounter + counterShift
            logger().debug( "Chains binding effective: signature: " + str(self.siglist[i].tes) + " will be shifted to step: "+ str(self.siglist[i].sigcounter) )
        # true means that there was something to do
        return True

    def getOutputTEs(self):
        tes = []
        for sig in self.siglist[1:]:
            tes.extend(sig.tes)
        return tes

    def isL2(self):
        if self.level == "L2":
            return True
        else:
            return False

    def isEF(self):
        if self.level == "EF":
            return True
        else:
            return False

    def addTriggerTypeBit(self, bit):
        self.trigger_type_bits.append(bit)

    def addStreamTag(self, stream, type="physics", obeyLB="yes", prescale='1'):
        self.stream_tag.append( (stream, type, obeyLB, prescale) )

    def addGroup(self, name ):
        if type(name) == type(''):
            self.groups += [name]
        if type(name) == type([]):
            self.groups += name

    def xml(self, xChainList):
        """ Returns XML representation of this chain """
        xChain = xml.dom.minidom.Document().createElement('CHAIN')
        xChainList.appendChild(xChain)

        xChain.setAttribute('chain_name', self.chain_name)
        xChain.setAttribute('chain_counter', self.chain_counter)
        xChain.setAttribute('lower_chain_name', self.lower_chain_name)
        xChain.setAttribute('level', self.level)
        xChain.setAttribute('prescale', self.prescale)
        xChain.setAttribute('pass_through', self.pass_through)
        xChain.setAttribute('rerun_prescale', self.rerun_prescale)


        xTriggerTypeList = xml.dom.minidom.Document().createElement('TRIGGERTYPE_LIST')
        xChain.appendChild(xTriggerTypeList)


        for bit in self.trigger_type_bits:
            xTriggerType = xml.dom.minidom.Document().createElement('TRIGGERTYPE')
            xTriggerType.setAttribute('bit', str(bit))
            xTriggerTypeList.appendChild(xTriggerType)

        xStreamTagList =  xml.dom.minidom.Document().createElement('STREAMTAG_LIST')
        xChain.appendChild(xStreamTagList)

        for stream in self.stream_tag:
            xStreamTag = xml.dom.minidom.Document().createElement('STREAMTAG')
            xStreamTag.setAttribute('stream', stream[0])
            xStreamTag.setAttribute('type', stream[1])
            xStreamTag.setAttribute('obeyLB', stream[2])
            xStreamTag.setAttribute('prescale', str(stream[3]))
            xStreamTagList.appendChild(xStreamTag)


        xGroupList = xml.dom.minidom.Document().createElement('GROUP_LIST')
        xChain.appendChild(xGroupList)                                             
        for g in self.groups:
            xGroup = xml.dom.minidom.Document().createElement('GROUP')
            xGroup.setAttribute('name', g)
            xGroupList.appendChild(xGroup)
            
        xSignatureList = xml.dom.minidom.Document().createElement('SIGNATURE_LIST')
        xChain.appendChild(xSignatureList)
        for sig in self.siglist:
            sig.xml(xSignatureList)



#
# EOF Chain classes
#########################################################################################


#########################################################################################
# HLT Sequence
#
class HLTSequence:
    def __init__(self, inputTEs, algos, outputTE, topo_starts_from):
        """HLT Steering Sequence class constructor


        inputTEs argument is a one or list of names TEs for the sequence,
        if empty string is passed then sequence is considered as UNSEEDED
        algos is one or list of algo instances

        """
        if type(inputTEs) == type(""):
            self.input  = [ inputTEs ]
        else:
            self.input  = inputTEs

        # make algos always to be alist of ... something ...
        if type(algos) != type([]):
            algos = [ algos ]
        from AthenaCommon.Configurable import ConfigurableAlgorithm
        self.algs = []
        for alg in algos:
            if type(alg) == type(""): # plain string
                self.algs.append( alg );
            elif issubclass(type(alg), ConfigurableAlgorithm): # configurable given to us
                self.algs.append(alg.getFullName())
        self.output = outputTE
        self.topo_starts_from = topo_starts_from

        self.__used = None

    def used(self):
        return self.__used

    def use(self, level):
        if level == 'L2' or level == 'EF':
            self.__used = level
        else:
            raise Exception('setting for the sequence level to incorrect value: '+str(level))

    def __str__(self):
        return str(self.input)+" -> "+ str(self.algs)+" -> "+self.output

    def xml(self, xlist):
        """Generates piece of XML used to configure HLT Steering"""
        if not self.used():
            return
        
        #logger().info('doing seq: ' + self.output)
        #print 'doing seq: ' + self.output + str(self.input)
        xSequence = xml.dom.minidom.Document().createElement('SEQUENCE')
        xSequence.setAttribute('input', reduce(lambda x,y: x+' '+y, self.input))
        xSequence.setAttribute('output', self.output)
        xSequence.setAttribute('algorithm', reduce(lambda x,y: x+' '+y, self.algs))
        if self.topo_starts_from!= None:
            xSequence.setAttribute('topo_start_from', self.topo_starts_from)  
        xlist.appendChild(xSequence)

    def dot(self, algs=True):
        if not self.used():
            return ''
        #example EM10_1 -> em10t2calo_1  [label="T2Calo,\n T2Hypo10"]        
        line = ''
        for i in self.input:
            if i == '':
                line += '"UNSEEDED"  [style=filled, fillcolor=blue]\n'
                line += 'UNSEEDED'
            else:
                line += '"'+i+'"'
            line += ' -> ' + '"' + self.output + '"'
            if algs:
                line += ' [label="' + reduce(lambda x,y: x+'\\n '+y, self.algs) + '"]'
            line += '\n'
        return line
    
#
# EOF Sequences
#########################################################################################
