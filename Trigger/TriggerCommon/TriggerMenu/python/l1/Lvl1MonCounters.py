# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



class Lvl1MonCounters(object):

    def __init__(self):

        # list of counters
        self.counters = []


    def addCounter(self, c):
        self.counters += [c]


    def xml(self, ind=1, step=2):
        s  = ind * step * ' ' + '<TriggerCounterList>\n'
        for c in sorted(self.counters):
            s  += c.xml(ind+1,step)
        s += ind * step * ' ' + '</TriggerCounterList>\n'
        return s


class Lvl1MonCounter(object):

    def __init__(self, name, multiplicity, montype):
        self.name = name
        self.multiplicity = int(multiplicity)
        self.montype = montype
        pass

    def __cmp__(self, o):
        if(self.name!=o.name):
            return cmp(self.name,o.name)
        return cmp(self.multiplicity,o.multiplicity)
    
class Lvl1CtpinCounter(Lvl1MonCounter):

    def __init__(self, name, multiplicity):
        super(Lvl1CtpinCounter, self).__init__(name, multiplicity, 'CTPIN')
        #print 'Initialize ctpin counter name="%i%s" triggerthreshold="%s" multi="%i"' % (self.multiplicity, self.name, self.name, self.multiplicity)

    def xml(self, ind=1, step=2):
        s  = ind * step * ' ' + '<TriggerCounter name="%i%s" type="CTPIN">\n' % (self.multiplicity, self.name)
        s += ind * step * ' ' + '  <TriggerCondition name="%i%s" triggerthreshold="%s" multi="%i"/>\n' % (self.multiplicity, self.name, self.name, self.multiplicity)
        s += ind * step * ' ' + '</TriggerCounter>\n'
        return s



class Lvl1CtpmonCounter(Lvl1MonCounter):

    def __init__(self, name, multiplicity):
        super(Lvl1CtpmonCounter, self).__init__(name, multiplicity, 'CTPMON')
        #print 'Initialize ctpmon counter name="%i%s" triggerthreshold="%s" multi="%i"' % (self.multiplicity, self.name, self.name, self.multiplicity)

    def xml(self, ind=1, step=2):
        s  = ind * step * ' ' + '<TriggerCounter name="%i%s" type="CTPMON">\n' % (self.multiplicity, self.name)
        s += ind * step * ' ' + '  <TriggerCondition name="%i%s" triggerthreshold="%s" multi="%i"/>\n' % (self.multiplicity, self.name, self.name, self.multiplicity)
        s += ind * step * ' ' + '</TriggerCounter>\n'
        return s

