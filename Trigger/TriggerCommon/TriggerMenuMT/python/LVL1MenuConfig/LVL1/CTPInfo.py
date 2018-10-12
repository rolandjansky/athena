# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from BunchGroupSet import BunchGroupSet
from Lvl1MenuUtil import oldStyle

class CTPInfo:

    class Random:
        def __init__(self, names, cuts):
            self.names = names
            self.cuts  = cuts

        def xml(self, ind=1, step=2):
            if self.cuts[3] != 1:
                raise RuntimeError("Random generator 3 has to run at 40MHz, so Random.cut3 must be set to 1")
            
            a = []
            for idx, (name,cut) in enumerate( zip(self.names,self.cuts) ):
                a += ['name%i="%s" cut%i="%i"' % (idx, name, idx, cut)]
            return ind * step * ' ' + '<Random %s/>\n' % ' '.join(a)


    class PrescaledClock:
        def __init__(self, name = '', clock1 = 0, clock2 = 0):
            self.name = name
            self.clock1 = clock1
            self.clock2 = clock2
        def xml(self, ind=1, step=2):
            return ind * step * ' ' + '<PrescaledClock clock1="%i" clock2="%i" name="%s"/>\n' % (self.clock1, self.clock2, self.name)


    # Members of CTPInfo
    def __init__(self):
        self.random         = CTPInfo.Random( names = ['Random0', 'Random1', 'Random2', 'Random3'], cuts = [1, 1, 1, 1] )
        self.prescaledClock = CTPInfo.PrescaledClock('psc01', 10, 100)
        self.bunchGroupSet  = BunchGroupSet()

    def setBunchGroupSetName(self, name):
        self.bunchGroupSet.name = name
        return self.bunchGroupSet

    def addBunchGroup(self, name, internalNumber, bunches):
        self.bunchGroupSet.addBunchGroup(name, internalNumber, bunches)

    def xml(self, ind=1, step=2):
        s = ''
        if oldStyle(): # just to make sure the triggertool can still read old files for a while
            s += ind * step * ' ' + '<Random name="rand01" rate1="0" rate2="0" seed1="1" seed2="1"/>\n'
            s += ind * step * ' ' + '<Deadtime complex1_level="7" complex1_rate="415" complex2_level="7" complex2_rate="415" name="Physics" simple="5" version="1"/>\n'
        else:            
            s += self.random.xml(ind,step)
        s += self.bunchGroupSet.xml(ind,step)
        s += self.prescaledClock.xml(ind,step)
        return s
