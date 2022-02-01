# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict
from functools import total_ordering

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

class MenuMonCountersCollection(object):

    def __init__(self):
        # list of monitoring counters
        self.counters = odict()
        self.counters['ctpmon'] = []
        self.counters['ctpin'] = []

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


@total_ordering
class MonCounter(object):

    def __init__(self, threshold, multiplicity, montype):
        self.name = "%i%s" % (multiplicity, threshold)
        self.threshold = threshold
        self.multiplicity = int(multiplicity)
        self.montype = montype
        pass

    def __lt__(self, o):
        if(self.threshold!=o.threshold):
            return self.threshold < o.threshold
        return self.multiplicity < o.multiplicity

    def __eq__(self, o):
        return self.name == o.name

    def json(self):
        confObj = odict()
        confObj["thr"] = self.threshold
        confObj["multiplicity"] = self.multiplicity
        return confObj

    
class CtpinCounter(MonCounter):
    """
    These monitor the CTP Item counts
    """
    def __init__(self, threshold, multiplicity):
        super(CtpinCounter, self).__init__(threshold, multiplicity, 'ctpin')

class CtpmonCounter(MonCounter):
    """
    These monitor the CTPInput signal counts
    """
    def __init__(self, threshold, multiplicity):
        super(CtpmonCounter, self).__init__(threshold, multiplicity, 'ctpmon')

