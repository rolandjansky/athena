# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PrescaleHelper import getCutFromPrescale, getPrescaleFromCut
from Lvl1MenuUtil import oldStyle

from AthenaCommon.Logging import logging
log = logging.getLogger("LVL1MenuItem")


class LVL1MenuItem(object):

    # global variable for item registration, if set, new items will be registered with l1configForRegistration (type TriggerConfigL1)
    l1configForRegistration = None

    # New items will be assigned to this partition
    currentPartition = 0
    
    __slots__ = ['name', 'group', 'ctpid', 'psCut', 'complex_deadtime', 'trigger_type', 'partition', 'logic', 'monitorsLF', 'monitorsHF', 'verbose']
    def __init__(self, name, ctpid=-1, group='1', prescale=1, complex_deadtime=0, psCut=None, verbose=False):
        self.name             = name
        self.group            = group
        self.ctpid            = int(ctpid)
        self.psCut            = psCut if psCut!=None else getCutFromPrescale(prescale)
        self.complex_deadtime = complex_deadtime
        self.trigger_type     = 0
        self.partition        = LVL1MenuItem.currentPartition
        self.logic            = None
        self.monitorsLF       = 0
        self.monitorsHF       = 0
        self.verbose          = verbose

        if self.verbose:
            print "Created",name

        if LVL1MenuItem.l1configForRegistration:
            LVL1MenuItem.l1configForRegistration.registerItem(self.name, self)


    def __str__(self):
        s = "Item %s (ctp id=%i) [%s]" % (self.name, self.ctpid, self.logic)
        return s
    
    @property
    def prescale(self):
        return getPrescaleFromCut(self.psCut)

    @prescale.setter
    def prescale(self, prescale):
        self.psCut = getCutFromPrescale(prescale)
        return self

    @property
    def priority(self):
        print "Item priority not used anymore, replaced by complex_deadtime"
        return self.complex_deadtime

    @priority.setter
    def priority(self,priority):
        print "Setter: item priority not used anymore, replaced by complex_deadtime"
        

    def addMonitor(self, flag, frequency):
        from TriggerMenu.l1menu.MonitorDef import MonitorDef
        if frequency == MonitorDef.LOW_FREQ:
            self.monitorsLF |= flag
        if frequency == MonitorDef.HIGH_FREQ:
            self.monitorsHF |= flag
    
    def setLogic(self, logic):
        self.logic = logic
        if self.verbose:
            print self
        return self

    def setCtpid(self, x):
        self.ctpid = int(x)
        return self

    def thresholdNames(self, include_bgrp=False):
        if self.logic!=None:
            return self.logic.thresholdNames(include_bgrp)
        else:
            return []

    def conditions(self, include_internal=False):
        if self.logic!=None:
            return self.logic.conditions(include_internal)
        else:
            return []

    def setTriggerType(self, ttype):
        self.trigger_type = int(ttype) & 0xff
        return self


    def binary_trigger_type(self, width=8):
        from Lvl1MenuUtil import binstr
        return binstr(self.trigger_type, width=width)


    def xml(self, ind, step=2):
        monitor= ' monitor="LF:{0:03b}|HF:{1:03b}"'.format(self.monitorsLF,self.monitorsHF)
        s = ind * step * ' ' + '<TriggerItem ctpid="%i" partition="%i" name="%s" complex_deadtime="%i" definition="(%s)" trigger_type="%s"%s>\n' % \
            (self.ctpid, self.partition, self.name, self.complex_deadtime, str(self.logic).replace('&','&amp;'), self.binary_trigger_type(8 if self.partition==1 else 4), monitor)
        if self.logic:
            self.logic.normalize()
            s += self.logic.xml(ind+1, step) + "\n"
        s += ind * step * ' ' + '</TriggerItem>'
        return s


class LVL1MenuItems:
    def __init__(self):
        self.menuName = ''
        self.pssName = ''
        self.pssType = 'Physics'
        self.items = []

    def __iter__(self):
        return self.items.__iter__()

    def itemNames(self):
        return [x.name for x in self.items]

    def findItemByName(self, name):
        return ([x for x in self.items if x.name==name] + [None])[0]

    def __iadd__(self, item):
        if item is None: return self
        if item.name in [x.name for x in self.items]:
            log.warning("LVL1 item %s is already in the menu, will not add it again" % item.name)
            return self
        if item.ctpid in [x.ctpid for x in self.items]:
            log.warning("LVL1 item with ctpid %i is already in the menu, will not add %s" % (item.ctpid, item.name) )
            return self
        self.items += [ item ]
        return self
    
    def __contains__(self, itemname):
        return itemname in [x.name for x in self.items] 

    def __len__(self):
        return len(self.items)

    def xml(self, ind=1, step=2):

        self.items.sort( lambda x,y: cmp(x.ctpid,y.ctpid) )

        s = ind * step * ' ' + '<TriggerMenu name="%s" phase="lumi">\n' % self.menuName
        for i in self.items:
            s += i.xml(ind+1,step)+"\n"
        s += ind * step * ' '+'</TriggerMenu>\n'

        pshandler = PrescaleHandler(self.pssName, self.pssType, self.items)
        s += pshandler.xml()

        # write priorities
        if oldStyle():
            s += ind * step * ' ' + '<PrioritySet name="standard" version="1">\n'
            for item in self.items:
                s += (ind+1) * step * ' ' + '<Priority ctpid="%i" priority="%s"/>\n'  % (item.ctpid, "HIGH" if item.complex_deadtime==0 else "LOW")
            s += ind * step * ' ' + '</PrioritySet>\n'

        return s


class PrescaleHandler(object):
    def __init__(self, name, psType, menuitems):
        self.name = name
        self.psType = psType
        self.items = menuitems
        self.itemsByPartition = {}

        for item in menuitems:
            self.itemsByPartition.setdefault(item.partition,[]).append(item)

        for itemList in self.itemsByPartition.values():
            itemList.sort(lambda x,y: cmp(x.ctpid,y.ctpid))

    def xml(self, ind=1, step=2):
        # write prescales (only prescales for which an item is defined)
        # write one set per defined partition
        s = ind * step * ' ' + '<PrescaleSet name="%s" type="%s" menuPartition="0">\n' % (self.name, self.psType)

        from Limits import Limits
        cuts = [ getCutFromPrescale(-1) ] * Limits.MaxTrigItems
        
        for item in self.items:
            cuts[item.ctpid] = item.psCut

        for ctpid, pscut in enumerate(cuts):
            psvalue = getPrescaleFromCut(pscut)
            if oldStyle():
                s += (ind+1) * step * ' ' + '<Prescale ctpid="%i" ps="%g"/>\n'  % (ctpid, psvalue)
            else:
                s += (ind+1) * step * ' ' + '<Prescale ctpid="%i" cut="%s%06x" value="%g"/>\n'  % (ctpid, '-' if pscut<0 else '', abs(pscut), psvalue)
        s += ind * step * ' ' + '</PrescaleSet>\n'
        return s
