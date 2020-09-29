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
            msg = "LVL1 item %s is already in the menu, will not add it again" % item.name
            log.error(msg)
            raise RuntimeError(msg)
        if item.ctpid in [x.ctpid for x in self.items.values()]:
            msg = "LVL1 item %s with ctpid %i is already in the menu, will not add %s with the same ctpid" % (self.itemById(item.ctpid).name, item.ctpid, item.name)
            log.error(msg)
            raise RuntimeError(msg)
        self.items[ item.name ] = item
        return self
    
    def __contains__(self, itemname):
        return itemname in self.items

    def __len__(self):
        return len(self.items)

    def itemById(self,ctpid):
        itemById = {it.ctpid:it for it in self.items.values()}
        return itemById[ctpid] if ctpid in itemById else None

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
    
    __slots__ = ['name', 'group', 'ctpid', 'psCut', 'trigger_type', 'partition', 'logic', 'bunchGroups', 'legacy', 'monitorsLF', 'monitorsHF', 'verbose']
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
        self.legacy           = False

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
        if MenuItemsCollection.splitBunchGroups:
            (self.logic, self.bunchGroups) = logic.stripBunchGroups(logic)
        else:
            self.logic = logic
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

    def markLegacy(self,legacyThresholdsSet):
        self.legacy = bool ( legacyThresholdsSet.intersection(self.logic.thresholdNames()) )

    def binary_trigger_type(self, width=8):
        """Turns integer triggertype in a binary string of given width"""
        return binstr(self.trigger_type, width=width)

    def json(self):
        confObj = odict()
        confObj["name"] = self.name
        if self.legacy:
            confObj["legacy"] = self.legacy
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


class meta_d(type):
    def __getattr__(cls, attr):
        import traceback
        isTopo = any(filter(lambda x: attr.startswith(x), ["R2TOPO_", "TOPO_", "MUTOPO_", "MULTTOPO_"]))
        fs = traceback.extract_stack()[-2]
        expProdFile = "L1/Config/"
        if isTopo:
            if attr.startswith("R2TOPO_"):
                expProdFile += "TopoAlgoDefLegacy.py"
            elif attr.startswith("TOPO_"):
                expProdFile += "TopoAlgoDef.py"
            elif attr.startswith("MUTOPO_"):
                expProdFile += "TopoAlgoDefMuctpi.py"
            elif attr.startswith("MULTTOPO_"):
                expProdFile += "TopoMultiplicityAlgoDef.py"
        else:
            isLegacyThr = any(filter(lambda x: attr.startswith(x), ["EM", "TAU", "J", "XE", "TE", "XS"]))
            if isLegacyThr:
                expProdFile += "ThresholdDefLegacy.py"
            else:
                expProdFile += "ThresholdDef.py"

        msg = "Item definition issue in file %s, line %i. Threshold %s has not been defined in %s" % ('/'.join(fs.filename.rsplit('/',4)[1:]),fs.lineno, attr, expProdFile)
        log.error(msg)
        raise RuntimeError(msg)
