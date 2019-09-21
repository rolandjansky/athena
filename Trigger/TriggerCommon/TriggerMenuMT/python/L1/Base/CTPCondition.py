# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger("Menu.L1.Base.CTPCondition") 

from .Logic import Logic

class Condition(Logic):
    def __init__(self):
        super(Condition, self).__init__(condition = self)


class ThrCondition(Condition):
    def __init__(self, threshold, multiplicity=1):
        super(ThrCondition, self).__init__()
        self.threshold = threshold
        self.multiplicity = multiplicity

    def __str__(self):
        return str(self.threshold) + '[x' + str(self.multiplicity)+']'

    def x(self, multiplicity):
        return ThrCondition(self.threshold, multiplicity) # provide a copy

    def name(self):
        return "%s_x%i" % (self.threshold.name, self.multiplicity)



class InternalTrigger(Condition):
    def __init__(self, name):
        super(InternalTrigger, self).__init__()
        self._name = name
        
    def __str__(self):
        return str(self._name)

    def name(self):
        return self._name
