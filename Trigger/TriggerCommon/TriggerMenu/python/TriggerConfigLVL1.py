# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import re

from .TriggerConfigL1Topo import TriggerConfigL1Topo
from l1.Lvl1Menu import Lvl1Menu
from l1.Lvl1Flags import Lvl1Flags


from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerConfigLVL1")

class TriggerConfigLVL1:

    current = None
    def __init__(self, outputFile = None , inputFile = None , menuName = None , topoMenu = "MATCH" ):
        """
        menuName: if None taken from TF.triggerMenuSetup, otherwise assume externally defined menu
        topoMenu: MATCH means that the L1Topo menu matches the L1 menu
        """
        TriggerConfigLVL1.current = self

        from TriggerJobOpts.TriggerFlags import TriggerFlags

        self.menuName = TriggerFlags.triggerMenuSetup() if menuName==None else menuName

        self.inputFile     = inputFile
        self.outputFile    = outputFile
        self.topoMenu      = topoMenu
        self.l1menuFromXML = None # flag if l1menu is read from XML file

        # all registered items
        self.registeredItems = {}
        
        # all registered thresholds
        self.registeredThresholds = {}

        # get L1Topo trigger line connections
        if topoMenu=="MATCH": topoMenu = self.menuName # topo menu name should match CTP menu for correct connection
        if topoMenu!=None:
            self.topotriggers = self.getL1TopoTriggerLines(topoMenu)
            self.registerAllTopoTriggersAsThresholds()


        # menu
        self.menu = Lvl1Menu(self.menuName)

        if self.inputFile != None:
            """Read menu from XML"""
            self.l1menuFromXML = True
            self.menu.readMenuFromXML(self.inputFile)
        elif menuName==None:
            """Build menu from menu name"""
            # defines the menu (item and threshold names)
            TriggerConfigLVL1.defineMenu(self.menuName)

            # registers all items ever defined
            self.registerMenu()


    ## L1 Topo connection
    def getL1TopoTriggerLines(self, menu):
        if menu == None:
            return None

        if menu.endswith(".xml"):
            raise RuntimeError("Can't read the topo trigger lines from xml yet")

        else:
            triggerLines = None
            try:
                tpcl1 = TriggerConfigL1Topo()
                tpcl1.generateMenu()
                triggerLines = tpcl1.menu.getTriggerLines()

            except Exception, ex:
                print "Topo menu generation inside L1 menu failed, but will be ignored for the time being",ex 

            return triggerLines

            
    ## Thresholds
    def registerThr(self, name, type, mapping=-1, active=1,
                    seed='', seed_ttype='', seed_multi=0, bcdelay=0 ):
        """
        Add LVL1 thresholds with the given type and mapping and store
        it in the list of available thresholds.

        The cable input information will be calculated from type and
        mapping.

        Seed, seed_ttype, seed_multi and bcdelay are for the zero bias
        trigger only.
        """

        if name in self.registeredThresholds:
            log.error("LVL1 threshold of name '%s' already registered, will ignore this new registration request" % name)
            return self.registeredThresholds[name]

        from l1.Lvl1Thresholds import LVL1Threshold
        thr = LVL1Threshold( name, type,
                             mapping = mapping, active = active,
                             seed_type = seed_ttype, seed = seed, seed_multi = seed_multi, bcdelay = bcdelay
                             )

        self.registeredThresholds[name] = thr
        return thr


    def registerAllTopoTriggersAsThresholds(self):
        """
        Add all L1Topo triggers as allowed input to the menu
        """
        if not self.topotriggers:
            return
        
        from l1.Lvl1Thresholds import LVL1TopoInput
        from collections import defaultdict

        multibitTopoTriggers = defaultdict(list)
        multibitPattern = re.compile("(?P<line>.*)\[(?P<bit>\d+)\]")
        for triggerline in self.topotriggers:
            m = multibitPattern.match(triggerline.trigger) # tries to match "trigger[bit]"
            if m:
                multibitTopoTriggers[m.groupdict()['line']] += [triggerline]  # multibit triggerlines are temporarilty stored in multibitTopoTriggers
            else:
                thr = LVL1TopoInput( triggerline )
                thr.setCableInput()
                self.registeredThresholds[thr.name] = thr

        # create thresholds from topo-multibit 
        for multibitTriggerlines in multibitTopoTriggers.values():
            thr = LVL1TopoInput( multibitTriggerlines )
            thr.setCableInput()
            self.registeredThresholds[thr.name] = thr



            
    def getRegisteredThreshold(self, name):
        if name in self.registeredThresholds:
            return self.registeredThresholds[name]
        return None
    

    ## Items
    def registerItem(self, name, item):
        """ Adds a LVL1 item to the set of items which are registered for further use"""
        if name in self.registeredItems:
            log.error('LVL1 item %s is already registered with ctpid=%d' % \
                           (name, int(self.registeredItems[name].ctpid)))
        else:
            self.registeredItems[name] = item

    def getRegisteredItem(self, name):
        if name in self.registeredItems:
            return self.registeredItems[name]
        return None


    def CTPInfo(self):
        return self.menu.CTPInfo


    def Lvl1CaloInfo(self):
        return self.menu.CaloInfo


    def writeXML(self):
        """
        Writes L1 XML file
        returns the file name
        """
        if self.outputFile is None:
            log.warning("Can't write xml file since no name was provided")
            return

        from l1.Lvl1MenuUtil import idgen
        idgen.reset()

        FH = open( self.outputFile, mode="wt" )
        FH.write( self.menu.xml() )
        FH.close()
        from l1.Lvl1MenuUtil import oldStyle
        log.info("Wrote %s in %s" % (self.outputFile, "run 1 style" if oldStyle() else "run 2 style"))
        return self.outputFile



    @staticmethod
    def defineMenu(menuName = None ):
        """
        Defines the list if item and threshold names that will be in the menu

        Calls defineMenu() of the correct module 'Menu_<menuname>.py'

        Menu.defineMenu() defines the menu via Lvl1Flags
        """

        from TriggerJobOpts.TriggerFlags import TriggerFlags
        if not menuName:
            menuName = TriggerFlags.triggerMenuSetup()

        menuName=TriggerConfigL1Topo.getMenuBaseName(menuName)
        menumodule = __import__('l1menu.Menu_%s' % menuName, globals(), locals(), ['defineMenu'], -1)
        menumodule.defineMenu()
        log.info("menu %s contains %i items and %i thresholds" % ( menuName, len(Lvl1Flags.items()), len(Lvl1Flags.thresholds()) ) )



    def registerMenu(self):
        """
        Registers the list if items and thresholds that could be used in the menu of Run1

        Calls registerItem() of the correct module 'ItemDef.py' or 'ItemDefRun1.py'

        Has to run AFTER defineMenu
        """

        run1 = Lvl1Flags.CTPVersion()<=3

        itemdefmodule = __import__('l1menu.ItemDef%s' % ('Run1' if run1 else ''), globals(), locals(), ['ItemDef'], -1)

        itemdefmodule.ItemDef.registerItems(self)
        log.info("registered %i items and %i thresholds (%s)" % ( len(self.registeredItems), len(self.registeredThresholds), ('Run 1' if run1 else 'Run 2') ) )



    def generateMenu(self):
        """
        Generates the menu structure from the list of item and threshold names in the Lvl1Flags

        To be called after defineMenu()
        """

        if self.l1menuFromXML:
            log.info("Menu was read from input file '%s', generateMenu() will not run" % self.inputFile)
            return

        # build list of items for the menu from the list of requested names
        itemsForMenu = []

        for itemName in Lvl1Flags.items():
            registeredItem = self.getRegisteredItem(itemName)
            if registeredItem == None:
                log.fatal("LVL1 item '%s' has not been registered in l1menu/ItemDef.py" % itemName)
                raise RuntimeError("LVL1 item %s has not been registered in l1menu/ItemDef.py" % itemName)

            if itemName in Lvl1Flags.CtpIdMap():
                newCTPID = Lvl1Flags.CtpIdMap()[itemName]
                registeredItem.setCtpid(newCTPID)

            itemsForMenu += [ registeredItem ]

        # pre-assigned ctpIDs (as integer values)
        assigned_ctpids = [item.ctpid for item in itemsForMenu]

        # CTP IDs available for assignment
        from l1.Limits import Limits
        available_ctpids = sorted( list( set(range(Limits.MaxTrigItems)) - set(assigned_ctpids) ) )
        available_ctpids.reverse()

        # add the items to the menu
        from TriggerMenu.l1.TriggerTypeDef import TT
        for item in itemsForMenu:
            # set the physics bit
            if not item.name.startswith('L1_CALREQ'):
                item.setTriggerType( item.trigger_type | TT.phys )
            # assign ctp IDs to items that don't have one
            if item.ctpid == -1:
                log.error('CTPID is not assigned for LVL1 item %s , please assign it in %s' % (item.name,self.menu.menuName))
            # add the items into the menu
            self.menu.addItem( item )

        # printout
        for item in self.menu.items:
            if item.verbose:
                log.info(str(item))

        # add the thresholds to the menu
        undefined_thr = False
        list_of_undefined_thresholds = []
        for index, thresholdName in enumerate(Lvl1Flags.thresholds()):
                        
            if thresholdName in self.menu.thresholds:
                continue
            threshold = self.getRegisteredThreshold(thresholdName)
            if threshold is None and not thresholdName=="":
                log.error('Threshold %s is listed in menu but not defined' % thresholdName )
                undefined_thr = True
                list_of_undefined_thresholds += [ thresholdName ]
            else:
                self.menu.addThreshold( threshold )
        if undefined_thr:
            raise RuntimeError("Found undefined threshold in menu %s, please add these thresholds to l1menu/ThresholdDef.py: %s" % (self.menu.menuName, ', '.join(list_of_undefined_thresholds)) )
                
        # threshold mapping
        self.mapThresholds()

        # ZB thresholds on the right connectors
        self.assignZeroBiasConnectors()

        # add the counters to the menu
        self.menu.addCounters()

        # update the prescales that are not 1
        self.updateItemPrescales()

        # other menu information
        self.setCTPInfo()
        self.setCaloInfo()

        # final consistency check
        self.menu.checkL1()



    def mapThresholds(self):
        """
        Set the correct mapping of thresholds according to the
        order it was given in Lvl1Flags.thresholds list. That list
        is usually defined in the setupMenu function of each menu

        NIM and CALREQ types are not remapped !!
        """
        existingMappings = {}
        for thr in self.menu.thresholds():
            if not thr.ttype in existingMappings:
                existingMappings[thr.ttype] = set()
            if thr.mapping<0: continue
            existingMappings[thr.ttype].add(thr.mapping)
                
        nextFreeMapping = {}
        for k in  existingMappings:
            nextFreeMapping[k] = 0

        for thr in self.menu.thresholds():
            if thr.mapping < 0:
                while nextFreeMapping[thr.ttype] in existingMappings[thr.ttype]:
                    nextFreeMapping[thr.ttype] += 1
                log.debug('Setting mapping of threshold %s as %i' % (thr, nextFreeMapping[thr.ttype]) )
                thr.mapping = nextFreeMapping[thr.ttype]
                nextFreeMapping[thr.ttype] += 1

            thr.setCableInput()


    def assignZeroBiasConnectors(self):
        from collections import Counter
        from copy import copy
        c = Counter()
        for thr in self.menu.thresholds:
            if thr.ttype=="ZB":
                if not thr.seed in self.menu.thresholds:
                    raise RuntimeError("Zero bias threshold '%s' based on non-existing threshold '%s'" % (thr,thr.seed) )
                seed = self.menu.thresholds.thresholdOfName(thr.seed) # the ZB seed
                thr.cableinfo = copy(seed.cableinfo)
                thr.cableinfo.bitnum      = 1
                thr.cableinfo.range_begin = 30
                thr.cableinfo.range_end   = 30
                c += Counter( ((thr.cableinfo.slot,thr.cableinfo.connector),) ) 
        
        if len(c)>0 and sorted(c.values())[-1]>1:
            for k,v in c.items():
                if v>1:
                    log.error("Slot %i, connector %i has more than one ZB threshold" % k)
            raise RuntimeError("Multiple zero bias thresholds on single connector")
            

            
    def updateItemPrescales(self):
        for (it_name, ps) in Lvl1Flags.prescales().items():
            item = self.menu.getItem(it_name)
            if item:
                item.prescale = ps
            else:
                log.warning('Cannot find item %s to set the prescale' % it_name )


    def setCTPInfo(self):
        self.menu.CTPInfo.bunchGroupSet.setDefaultBunchGroupDefinition()

        
    def setCaloInfo(self):
        from l1menu.CaloDef import CaloDef
        CaloDef.defineGlobalSettings(self)
