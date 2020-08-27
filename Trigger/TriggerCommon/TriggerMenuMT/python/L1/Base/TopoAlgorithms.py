# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict
from operator import attrgetter
from enum import Enum

from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Base.TopoAlgorithms')

from .TopoAlgos import DecisionAlgo, MultiplicityAlgo, SortingAlgo

class AlgType(Enum):
    SORT = ('sortingAlgorithms')
    DEC = ('decisionAlgorithms') 
    MULT = ('multiplicityAlgorithms')

    def __init__(self, key):
        self.key = key
    
class AlgCategory(Enum):
    TOPO = (1, 'TOPO', 'new topo', 'TopoAlgoDef')
    MUCTPI = (2, 'MUTOPO', 'muctpi topo', 'TopoAlgoDefMuctpi')
    LEGACY = (3, 'R2TOPO', 'legacy topo', 'TopoAlgoDefLegacy')
    MULTI = (4, 'MULTTOPO', 'multiplicity topo', 'TopoAlgoDefMultiplicity')

    def __init__(self, _, key, desc, defFile ):
        self.key = key
        self.prefix = key + '_' if key else ''
        self.desc = desc
        self.defFile = defFile

    def __str__(self):
        return self.desc

    @staticmethod
    def getAllCategories():
        return [ AlgCategory.TOPO, AlgCategory.MUCTPI, AlgCategory.MULTI, AlgCategory.LEGACY ]
            
    @staticmethod
    def getCategoryFromBoardName(boardName):
        if 'muctpi' in boardName.lower():
            currentTopoCategory = AlgCategory.MUCTPI
        elif 'topo' in boardName.lower():
            if 'legacy' in boardName.lower():
                currentTopoCategory = AlgCategory.LEGACY
            else:
                currentTopoCategory = AlgCategory.TOPO
        else:
            raise RuntimeError("Board %s is not a topo board" % boardName )
        return currentTopoCategory


class MenuTopoAlgorithmsCollection(object):

    def __init__(self):
        # all algos that are in menu (new and legacy)
        self.topoAlgos = odict()
        for cat in AlgCategory:
            self.topoAlgos[cat] = odict()


    def addAlgo(self, algo, category):
        if type(category) != AlgCategory:
            raise RuntimeError( "No category is provided when adding topo algo %s to menu" % algo.name)

        if isinstance(algo,DecisionAlgo):
            algType = AlgType.DEC
        elif isinstance(algo, SortingAlgo):
            algType = AlgType.SORT
        elif isinstance(algo, MultiplicityAlgo):
            algType = AlgType.MULT
        else:
            raise RuntimeError("Trying to add topo algorithm %s of unknown type %s to the menu" % (algo.name, type(algo)))

        if algType not in self.topoAlgos[category]:
            self.topoAlgos[category][algType] = odict()

        if algo.name in self.topoAlgos[category][algType]:
            raise RuntimeError("Trying to add topo algorithm %s a second time" % algo.name)

        self.topoAlgos[category][algType][algo.name] = algo


    def json(self):
        def idGenerator(usedIds, start):
            while True:
                while start in usedIds:
                    start += 1
                yield start
                start += 1

        confObj = odict()
        for cat in self.topoAlgos:
            confObj[cat.key] = odict()
            for typ in self.topoAlgos[cat]:
                confObj[cat.key][typ.key] = odict()
                for alg in sorted(self.topoAlgos[cat][typ].values(), key=attrgetter('name')):
                    confObj[cat.key][typ.key][alg.name] = alg.json()

                # set unspecified algoIds to a unique value
                usedAlgIds = set([x["algId"] for x in confObj[cat.key][typ.key].values() if x["algId"]>=0])
                autoId = idGenerator(usedAlgIds, 0)
                for algJsonEntry in confObj[cat.key][typ.key].values():
                    if algJsonEntry["algId"] < 0:
                        algJsonEntry["algId"] = next(autoId)

        return confObj
