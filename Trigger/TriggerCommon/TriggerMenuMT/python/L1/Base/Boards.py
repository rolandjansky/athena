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
    MERGER = 4
    def __repr__(self):
        return self.name
    def __str__(self):
        return self.name


class MenuBoardsCollection(object):
    def __init__(self):
        self.boards = {}

    def addBoard(self, boardDef):
        name = boardDef["name"]
        if 'muctpi' in name.lower():
            btype = BoardType.MUCTPI
        elif 'alfactpin' in name.lower():
            btype = BoardType.MERGER
        elif 'merger' in name.lower():
            btype = BoardType.MERGER
        elif 'topo' in name.lower():            
            btype = BoardType.TOPO
        else:
            btype = BoardType.NONE

        isLegacy = 'legacy' in name.lower()

        self.boards[name] = Board(name, btype, isLegacy)

        if "connectors" in boardDef:
            self.boards[name].addOutputConnectorNames([c["name"] for c in boardDef["connectors"]])

        return self.boards[name]

    def json(self):
        boardOrder = ["MuCTPi", "Topo1", "Topo2", "Topo3", "LegacyTopo0", "LegacyTopo1", "LegacyTopoMerger", "AlfaCtpin"]
        from collections import OrderedDict as odict
        confObj = odict()
        for boardName in boardOrder:
            if boardName in self.boards:
                confObj[boardName] = self.boards[boardName].json()
        return confObj


class Board(object):
    def __init__(self, name, btype, isLegacy = False):
        self.name = name
        self.btype = btype
        self.isLegacy = False
        self.outputConnectors = []
        self.inputConnectors = []

    def addOutputConnectorNames(self, connName ):
        self.outputConnectors += [ connName ]

    def json(self):
        confObj = odict()
        confObj["type"] = str(self.btype)
        confObj["legacy"] = self.isLegacy
        confObj["connectors"] = self.outputConnectors
        if confObj["type"] == BoardType.MERGER:
            confObj["inputConnectors"] = self.inputConnectors
        return confObj
