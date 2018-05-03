# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from Logic import Logic


class Lvl1Condition(Logic):
    def __init__(self):
        super(Lvl1Condition, self).__init__(condition = self)



class ThrCondition(Lvl1Condition):
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

    def xml(self, ind, step=2):
        return ind * step * ' ' + '<TriggerCondition multi="%i" name="%s" triggerthreshold="%s"/>' % \
               (self.multiplicity, self.name(), self.threshold.name)



class Lvl1InternalTrigger(Lvl1Condition):
    def __init__(self, name):
        super(Lvl1InternalTrigger, self).__init__()
        self._name = name
        
    def __str__(self):
        return str(self._name)

    def name(self):
        return self._name

    def xml(self, ind, step=2):
        return ind * step * ' ' + '<InternalTrigger name="%s"/>' % self.name()
