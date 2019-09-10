# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Base.MonCounters')

class MenuMonCountersCollection(object):

    def __init__(self):
        # list of monitoring counters
        self.counters = odict()

    def addCounter(self, c):
        if c.montype not in self.counters:
            self.counters[c.montype] = []
        self.counters[c.montype] += [c]

    def json(self):
        confObj = odict()
        for key,clist in self.counters.items():
            confObj[key] = odict()
            for c in clist:
                confObj[key][c.name] = c.json()
        return confObj


class MonCounter(object):

    def __init__(self, name, multiplicity, montype):
        self.name = name
        self.multiplicity = int(multiplicity)
        self.montype = montype
        pass

    def __cmp__(self, o):
        if(self.name!=o.name):
            return cmp(self.name,o.name)
        return cmp(self.multiplicity,o.multiplicity)

    def json(self):
        confObj = odict()
        confObj["thr"] = self.name
        confObj["multiplicity"] = self.multiplicity
        return confObj

    
class CtpinCounter(MonCounter):
    """
    These monitor the CTP Item counts
    """
    def __init__(self, name, multiplicity):
        super(CtpinCounter, self).__init__(name, multiplicity, 'ctpin')

class CtpmonCounter(MonCounter):
    """
    These monitor the CTPInput signal counts
    """
    def __init__(self, name, multiplicity):
        super(CtpmonCounter, self).__init__(name, multiplicity, 'ctpmon')

