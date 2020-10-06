# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__all__ = ['MenuBoardsCollection', 'BoardType']

from enum import Enum
from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger("Menu.L1.Base.Boards") 

class BoardType(Enum):
    NONE = 1
    MUCTPI = 2
    TOPO = 3
    CTPIN = 4
    MERGER = 5
    def __repr__(self):
        return self.name
    def __str__(self):
        return self.name
    @staticmethod
    def fromBoardName(name):
        if 'muctpi' in name.lower():
            btype = BoardType.MUCTPI
        elif 'alfactpin' in name.lower():
            btype = BoardType.MERGER
        elif 'merger' in name.lower():
            btype = BoardType.MERGER
        elif 'topo' in name.lower():            
            btype = BoardType.TOPO
        elif 'ctpin' in name.lower():            
            btype = BoardType.CTPIN
        else:
            raise RuntimeError("No BoardType defined for board %s" % name)
        return btype


class MenuBoardsCollection(object):
    def __init__(self):
        self.boards = {}

    def addBoard(self, boardDef):
        name = boardDef["name"]
        btype = BoardType.fromBoardName(name)
        isLegacy = 'legacy' in boardDef
        self.boards[name] = Board(name, btype, isLegacy)
        if "connectors" in boardDef:
            self.boards[name].addOutputConnectorNames([c["name"] for c in boardDef["connectors"]])
        return self.boards[name]

    def json(self):
        from collections import OrderedDict as odict
        confObj = odict()
        for boardName in sorted(self.boards):
            confObj[boardName] = self.boards[boardName].json()
        return confObj


class Board(object):
    def __init__(self, name, btype, isLegacy = False):
        self.name = name
        self.btype = btype
        self.isLegacy = isLegacy
        self.outputConnectors = []
        self.inputConnectors = []

    def addOutputConnectorNames(self, connName ):
        self.outputConnectors += connName

    def json(self):
        confObj = odict()
        confObj["type"] = str(self.btype)
        if self.isLegacy:
            confObj["legacy"] = self.isLegacy
        confObj["connectors"] = self.outputConnectors
        # inputConnectors only exist for merger boards
        if confObj["type"] == BoardType.MERGER:
            confObj["inputConnectors"] = self.inputConnectors
        return confObj
