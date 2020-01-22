# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging

from ..Config.MonitorDef import MonitorDef
from .PrescaleHelper import getCutFromPrescale, getPrescaleFromCut
from .MenuUtils import binstr

from past.builtins import cmp


log = logging.getLogger("Menu.L1.Base.Items")


class MenuItemsCollection(object):

    splitBunchGroups = False

    def __init__(self):
        self.menuName = ''
        self.pssName = ''
        self.pssType = 'Physics'
        self.items = odict()

    def __iter__(self):
        return iter(self.items.values())

    def __getitem__(self, key):
        return self.items[key]

    def __setitem__(self, name, item):
        if item is None:
            return self
        if name in self.items:
            log.warning("LVL1 item %s is already in the menu, will not add it again", name)
            return self
        if item.ctpid in [x.ctpid for x in self.items]:
            log.warning("LVL1 item with ctpid %i is already in the menu, will not add %s", item.ctpid, name)
            return self
        self.items[ name ] = item
        return self

    def __iadd__(self, item):
        if item is None:
            return self
        if item.name in self.items:
            log.warning("LVL1 item %s is already in the menu, will not add it again", item.name)
            return self
        if item.ctpid in [x.ctpid for x in self.items.values()]:
            log.warning("LVL1 item with ctpid %i is already in the menu, will not add %s", item.ctpid, item.name)
            return self
        self.items[ item.name ] = item
        return self
    
    def __contains__(self, itemname):
        return itemname in self.items

    def __len__(self):
        return len(self.items)

    def itemNames(self):
        return self.items.keys()

    def json(self):
        confObj = odict()
        for item in self:
            confObj[item.name] = item.json()
        return confObj


class MenuItem(object):

    # global variable for item registration, if set, new items will be registered with l1configForRegistration (type TriggerConfigL1)
    l1configForRegistration = None

    @staticmethod
    def setMenuConfig(mc):
        MenuItem.l1configForRegistration = mc

    # New items will be assigned to this partition
    currentPartition = 0
    
    __slots__ = ['name', 'group', 'ctpid', 'psCut', 'trigger_type', 'partition', 'logic', 'bunchGroups', 'monitorsLF', 'monitorsHF', 'verbose']
    def __init__(self, name, ctpid=-1, group='1', prescale=1, psCut=None, verbose=False):
        self.name             = name
        self.group            = group
        self.ctpid            = int(ctpid)
        self.psCut            = psCut if psCut is not None else getCutFromPrescale(prescale)
        self.trigger_type     = 0
        self.partition        = MenuItem.currentPartition
        self.logic            = None
        self.monitorsLF       = 0
        self.monitorsHF       = 0
        self.verbose          = verbose
        self.bunchGroups      = None

        if MenuItem.l1configForRegistration:
            MenuItem.l1configForRegistration.registerItem(self.name, self)


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

    def addMonitor(self, flag, frequency):
        if frequency == MonitorDef.LOW_FREQ:
            self.monitorsLF |= flag
        if frequency == MonitorDef.HIGH_FREQ:
            self.monitorsHF |= flag
    
    def setLogic(self, logic):
        self.logic = logic
        if self.verbose:
            log.debug('%s', self)

        if MenuItemsCollection.splitBunchGroups:
            bgLogic = self.logic.stripBunchGroups()
            self.bunchGroups = bgLogic.thresholdNames(include_bgrp=True)
        return self




    def setCtpid(self, x):
        self.ctpid = int(x)
        return self

    def thresholdNames(self, include_bgrp=False):
        if self.logic is not None:
            return self.logic.thresholdNames(include_bgrp)
        else:
            return []

    def conditions(self, include_internal=False):
        if self.logic is not None:
            return self.logic.conditions(include_internal)
        else:
            return []

    def setTriggerType(self, ttype):
        self.trigger_type = int(ttype) & 0xff
        return self

    def binary_trigger_type(self, width=8):
        """Turns integer triggertype in a binary string of given width"""
        return binstr(self.trigger_type, width=width)

    def json(self):
        confObj = odict()
        confObj["name"] = self.name
        confObj["ctpid"] = self.ctpid
        confObj["definition"] = str(self.logic)
        if self.bunchGroups:
            confObj["bunchgroups"] = self.bunchGroups
        confObj["triggerType"] = self.binary_trigger_type(8 if self.partition==1 else 4)
        confObj["partition"] = self.partition
        confObj["monitor"] = 'LF:{0:03b}|HF:{1:03b}'.format(self.monitorsLF,self.monitorsHF)
        return confObj




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

