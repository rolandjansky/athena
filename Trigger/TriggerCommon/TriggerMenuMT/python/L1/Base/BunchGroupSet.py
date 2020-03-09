# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags

from .Limits import Limits
from .L1MenuFlags import L1MenuFlags


log = logging.getLogger("Menu.L1.Base.BunchGroupSet")


def setDefaultBunchGroupDefinition(bgs):
    """
    sets default bunchgroups for all menus, needed for simulation.
    """

    bgs.bunchGroups = Limits.NumBunchgroups * [None]

    # BGS and BG names
    bgs.name = TriggerFlags.triggerMenuSetup().partition('_')[0]
    bunchgroupnames = L1MenuFlags.BunchGroupNames()[:Limits.NumBunchgroups]
    bunchgroupnames += ['NotUsed'] * (Limits.NumBunchgroups - len(bunchgroupnames))

    for i,bgname in enumerate(bunchgroupnames):
        bgs.addBunchGroup( bgname, i, [] )

    # these bunchgroups need to be filled for MC simulation
    bgs.bunchGroups[0].bunches = [1]
    bgs.bunchGroups[1].bunches = [1]
    bgs.bunchGroups[7].bunches = [1]



class BunchGroupSet(object):

    class BunchGroup(object):
        def __init__(self, name='', internalNumber=0, partition=0, bunches=[]):
            self.name           = name
            self.internalNumber = internalNumber
            self.menuPartition  = partition
            self.bunches        = bunches

        def json(self):
            confObj = odict()
            confObj["name"]  = self.name
            confObj["id"]    = self.internalNumber
            confObj["bcids"] = self.bunches
            return confObj
        
    def __init__(self, name='', menuPartition=0, bunchGroups = None):
        self.name          = name
        self.menuPartition = menuPartition
        self.bunchGroups   = bunchGroups

    def __len__(self):
        return len(self.bunchGroups)


    @classmethod
    def partitioning(cls):
        first = L1MenuFlags.BunchGroupPartitioning()
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
            log.error('Warning: tried to add bunchgroup %i, which is not between 0 and %i', internalNumber, Limits.NumBunchgroups-1)
            return self

        # check if one already exists with this number
        if self.bunchGroups[internalNumber] is not None:
            log.error('Warning: tried to add bunchgroup %i, but one with that number already exists', internalNumber)
            return self

        partition=0
        for lowestBG in L1MenuFlags.BunchGroupPartitioning():
            if internalNumber >= lowestBG:
                partition += 1

        self.bunchGroups[internalNumber] = BunchGroupSet.BunchGroup(name, internalNumber, partition, bunches)
        return self

    def json(self):
        confObj = odict()
        for bg in self.bunchGroups:
            confObj["BGRP%i" % bg.internalNumber] = bg.json()
        return confObj
