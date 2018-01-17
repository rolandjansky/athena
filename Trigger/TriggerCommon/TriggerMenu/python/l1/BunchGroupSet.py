# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Limits import Limits
from Lvl1Flags import Lvl1Flags

from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerMenu.l1.BunchGroupSet")

def setDefaultBunchGroupDefinition(bgs):
    """
    sets default bunchgroups for all menus, needed for simulation.
    """
    from TriggerJobOpts.TriggerFlags import TriggerFlags

    bgs.bunchGroups = Limits.NumBunchgroups * [None]

    # BGS and BG names
    bgs.name = TriggerFlags.triggerMenuSetup().partition('_')[0]
    bunchgroupnames = Lvl1Flags.BunchGroupNames()[:Limits.NumBunchgroups]
    bunchgroupnames += ['BG_%i' % i for i in range( len(bunchgroupnames), Limits.NumBunchgroups)]

    for i,bgname in enumerate(bunchgroupnames):
        bgs.addBunchGroup( bgname, i, [] )

    # this bunchgroups need to be filled for MC simulation
    #    if 'MC_' in TriggerFlags.triggerMenuSetup():
    bgs.bunchGroups[0].bunches = [1]
    bgs.bunchGroups[1].bunches = [1]
    bgs.bunchGroups[7].bunches = [1]

class BunchGroupSet:

    class BunchGroup:
        def __init__(self, name='', internalNumber=0, partition=0, bunches=[]):
            self.name           = name
            self.internalNumber = internalNumber
            self.menuPartition  = partition
            self.bunches        = bunches

        def xml(self, ind=1, step=2):
            #s  = ind * step * ' ' + '<BunchGroup menuPartition="%i" internalNumber="%i" name="%s"' % (self.menuPartition, self.internalNumber, self.name)
            s  = ind * step * ' ' + '<BunchGroup internalNumber="%i" name="%s"' % (self.internalNumber, self.name)
            if not self.bunches:
                s += '/>\n'
            else:
                s += '>\n'
                for b in self.bunches:
                    s += (ind+1) * step * ' ' + '<Bunch bunchNumber="%i"/>\n' % b
                s += ind * step * ' ' + '</BunchGroup>\n'
            return s
        

    def __init__(self, name='', menuPartition=0, bunchGroups = None):
        self.name          = name
        self.menuPartition = menuPartition
        self.bunchGroups   = bunchGroups

    def __len__(self):
        return len(self.bunchGroups)


    @classmethod
    def partitioning(cls):
        from Lvl1Flags import Lvl1Flags
        first = Lvl1Flags.BunchGroupPartitioning()
        last = first[1:] + [ Limits.NumBunchgroups ]
        partitioning = dict( zip([1,2,3],zip(first,last)) )
        return partitioning

    def setDefaultBunchGroupDefinition(self):
        setDefaultBunchGroupDefinition(self)


    def resize(self, newsize):
        if type(self.bunchGroups) != list:
            self.bunchGroups = []
        self.bunchGroups += newsize * [None]
        self.bunchGroups = self.bunchGroups[:newsize]


    def addBunchGroup(self, name, internalNumber, bunches):
        # check if internalNumber is within limits
        if internalNumber<0 or internalNumber >= Limits.NumBunchgroups:
            log.error('Warning: tried to add bunchgroup %i, which is not between 0 and %i' % (internalNumber, Limits.NumBunchgroups-1))
            return self

        # check if one already exists with this number
        if self.bunchGroups[internalNumber] != None:
            log.error('Warning: tried to add bunchgroup %i, but one with that number already exists' % internalNumber)
            return self

        partition=0
        from Lvl1Flags import Lvl1Flags
        for lowestBG in Lvl1Flags.BunchGroupPartitioning():
            if internalNumber >= lowestBG:
                partition += 1

        self.bunchGroups[internalNumber] = BunchGroupSet.BunchGroup(name, internalNumber, partition, bunches)
        return self



    def xml(self, ind=1, step=2):
        s  = ind * step * ' ' + '<BunchGroupSet name="%s" menuPartition="%i">\n' % (self.name, self.menuPartition)
        for bg in self.bunchGroups:
            if bg:
                s += bg.xml(ind+1,step)
        s += ind * step * ' ' + '</BunchGroupSet>\n'
        return s
