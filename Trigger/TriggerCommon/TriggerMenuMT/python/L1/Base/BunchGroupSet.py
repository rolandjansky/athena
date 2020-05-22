# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict
from itertools import groupby

from AthenaCommon.Logging import logging

from .Limits import Limits
from .L1MenuFlags import L1MenuFlags


log = logging.getLogger("Menu.L1.Base.BunchGroupSet")

def createDefaultBunchGroupSetMC():
    """
    create BunchGroupSet for simulation

    This BGS is independent from the menu, and contains only BG0 (BCRVeto) and BG1 (Paired)
    """
    bgs = BunchGroupSet("MC")
    bgs.addBunchGroup( BunchGroupSet.BunchGroup(name = 'BCRVeto', internalNumber = 0).addRange(0,3539).addRange(3561,3563).normalize() )\
       .addBunchGroup( BunchGroupSet.BunchGroup(name = 'Paired', internalNumber = 1).addTrain(0,3564).normalize() )
    return bgs


def createDefaultBunchGroupSet():
    """
    sets default bunchgroups for all menus, needed for simulation.
    """
    if L1MenuFlags.BunchGroupNames.statusOn: # if flag has been set
        # if menu defines bunchgroup names, then we generate a bunchgroup set from that
        name = L1MenuFlags.MenuSetup().partition('_')[0]
        bgs = BunchGroupSet(name)
        bgs.addBunchGroup( BunchGroupSet.BunchGroup(name = 'BCRVeto', internalNumber = 0).addRange(0,3539).addRange(3561,3563).normalize() )\
           .addBunchGroup( BunchGroupSet.BunchGroup(name = 'Paired', internalNumber = 1).addTrain(0,3564).normalize() )

        bunchgroupnames = L1MenuFlags.BunchGroupNames()[:Limits.NumBunchgroups]
        bunchgroupnames += ['NotUsed'] * (Limits.NumBunchgroups - len(bunchgroupnames))
        for i,bgname in enumerate(bunchgroupnames):
            bgs.bunchGroups[i].name = bgname
    else:
        bgs = createDefaultBunchGroupSetMC()

    return bgs


class BunchGroupSet(object):

    class BunchGroup(object):
        nameUndefBG = "NotUsed"
        def __init__(self, internalNumber, name = nameUndefBG, partition=0, bunches=None):
            # check if internalNumber is within limits
            if internalNumber<0 or internalNumber >= Limits.NumBunchgroups:
                raise RuntimeError('Cannot create bunchgroup with internal number %i (must be between 0 and %i)' % (internalNumber, Limits.NumBunchgroups-1))
            self.name           = name # name of the bunchgroup
            self.internalNumber = internalNumber # number between 0 and 15
            self.menuPartition  = partition # CTP partition that the bunchgroup belongs to
            self.bunches        = bunches if bunches else [] # list of bcids
            self.normalized     = [] # list of trains (filled in normalized())
        def __str__(self):
            return "bunchgroup %s(%i) with %i bunches" % (self.name, self.internalNumber, len(self))
        def __len__(self):
            return len(self.bunches)
        def addBunch(self, bcid):
            self.bunches += [ bcid ]
            return self
        def addRange(self, firstbcid, lastbcid):
            self.bunches += list(range(firstbcid, lastbcid+1))
            return self
        def addTrain(self, firstbcid, length):
            self.bunches += list(range(firstbcid, firstbcid+length))
            return self
        def normalize(self):
            """ turn list of bunches into trains """
            self.normalized = []
            self.bunches = sorted(list(set(self.bunches))) # uniq and sort
            if any(map(lambda bcid : bcid<0 or bcid >= 3564, self.bunches)):
                raise RuntimeError("Found bcid outside range 0..3563 in bunchgroup %s" % self.name)
            for k,g in groupby(enumerate(self.bunches), lambda x : x[1]-x[0]):
                train = list(g)
                self.normalized += [ (train[0][1], len(train)) ]
            return self
        def json(self):
            confObj = odict()
            confObj["name"]  = self.name
            confObj["id"]    = self.internalNumber
            confObj["info"] = "%i bunches, %i groups" % (len(self), len(self.normalized))
            confObj["bcids"] = []
            for first, length in self.normalized:
                confObj["bcids"] += [ odict( [ ("first", first), ("length", length) ] ) ]
            return confObj
        

    def __init__(self, name='', menuPartition=0):
        self.name          = name
        self.menuPartition = menuPartition
        self.bunchGroups   = [ BunchGroupSet.BunchGroup(i) for i in range(0,Limits.NumBunchgroups) ] # initialize all BunchGroups

    def __len__(self):
        return len(self.bunchGroups)


    @classmethod
    def partitioning(cls):
        first = L1MenuFlags.BunchGroupPartitioning()
        last = first[1:] + [ Limits.NumBunchgroups ]
        partitioning = dict( zip([1,2,3],zip(first,last)) )
        return partitioning


    def resize(self, newsize):
        if type(self.bunchGroups) != list:
            self.bunchGroups = []
        self.bunchGroups += newsize * [None]
        self.bunchGroups = self.bunchGroups[:newsize]


    def addBunchGroup(self, bunchGroup):
        # check if one already exists with this number
        doesExist = self.bunchGroups[bunchGroup.internalNumber].name != BunchGroupSet.BunchGroup.nameUndefBG
        if doesExist:
            raise RuntimeError("Adding bunchgroup with internal number %i, which already exists" % bunchGroup.internalNumber)

        partition=0
        for lowestBG in L1MenuFlags.BunchGroupPartitioning():
            if bunchGroup.internalNumber >= lowestBG:
                partition += 1
        bunchGroup.partition = partition
        self.bunchGroups[bunchGroup.internalNumber] = bunchGroup
        return self


    def json(self):
        confObj = odict()
        for bg in self.bunchGroups:
            confObj["BGRP%i" % bg.internalNumber] = bg.json()
        return confObj

    def writeJSON(self, outputFile, destdir="./", pretty=True):
        outputFile = destdir.rstrip('/') + '/' + outputFile
        confObj = odict()
        confObj["name"] = self.name
        confObj["filetype"] = "bunchgroupset"
        confObj["bunchGroups"] = self.json()
        with open( outputFile, mode="wt" ) as fh:
            import json
            json.dump(confObj, fh, indent = 4 if pretty else None, separators=(',', ': '))
        log.info("Wrote %s", outputFile)
        return outputFile
