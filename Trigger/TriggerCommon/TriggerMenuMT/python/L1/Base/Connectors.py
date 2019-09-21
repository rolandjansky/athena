# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import re
from enum import Enum
from collections import OrderedDict as odict

from AthenaCommon.Logging import logging

from .TopoAlgorithms import AlgCategory

log = logging.getLogger("Menu.L1.Base.Connectors")

class CType(Enum):
    CTPIN = (1, 'ctpin')
    ELEC = (2, 'electrical')
    OPT = (3, 'optical')
    def __init__(self, _, ctype ):
        self.ctype = ctype

    def __str__(self):
        return self.ctype

    @staticmethod
    def from_str(label):
        if label == 'ctpin':
            return CType.CTPIN
        elif label == 'electrical':
            return CType.ELEC
        elif label == 'optical':
            return CType.OPT
        else:
            raise NotImplementedError("Connector of type %s does't exist" % label)


class CFormat(Enum):
    MULT = (1, 'multiplicity')
    TOPO = (2, 'topological')
    def __init__(self, _, cformat ):
        self.cformat = cformat

    @staticmethod
    def from_str(label):
        if label == 'multiplicity':
            return CFormat.MULT
        elif label == 'topological':
            return CFormat.TOPO
        else:
            raise NotImplementedError



class MenuConnectorsCollection(object):

    def __init__(self):
        self.connectors = odict()
        self.inverseLookup = {}

    def addConnector(self, connDef):
        name, cformat, ctype, legacy, boardName = map(connDef.__getitem__,["name", "format", "type", "legacy", "board"])

        if name in self.connectors:
            raise RuntimeError("Connector %s has already been defined" % name)

        log.debug("Adding connector %s, format %s, legacy set to %s, and connType %s", name, cformat, legacy, ctype)
        if CType.from_str(ctype) is CType.ELEC:
            newConnector = ElectricalConnector(name, cformat, legacy)
        else:
            newConnector = Connector(name, cformat, ctype, legacy)
        self.connectors[name] = newConnector

        
        # treat differently depending on the "format", which can be: 'topological' or 'multiplicity'        
        if connDef["format"] == 'multiplicity':
            # multiplicity connectors contain all the triggerlines in a flat "thresholds" list
            startbit = 0
            for thrName in connDef["thresholds"]:
                nbits = connDef["nbitsDefault"]
                if type(thrName)==tuple:
                    (thrName,nbits) = thrName
                if thrName is None:
                    startbit += nbits
                    continue
                tl = TriggerLine( name = thrName, startbit = startbit, nbits = nbits)
                startbit += nbits
                newConnector.addTriggerLine(tl)

        elif connDef["format"] == 'topological':
            # topological connectors when they are electrical
            # connectors contain the triggerlines in up to four
            # algorithm groups, each corresponding to a different (fpga,clock) setting

            currentTopoCategory = AlgCategory.getCategoryFromBoardName(boardName)

            if newConnector.ctype == CType.ELEC:
                for thrG in connDef["algorithmGroups"]:
                    (newConnector.fpga, newConnector.clock) = map(thrG.__getitem__,["fpga","clock"])
                    fpga,clock = map(thrG.__getitem__,["fpga","clock"])
                    for topo in thrG["algorithms"]:
                        bit = topo.outputbits[0] if isinstance(topo.outputbits, tuple) else topo.outputbits
                        for (i, tl) in enumerate(topo.outputlines):
                            # for topoological triggerlines the names have to be prefixed as they are in the item definitions
                            tlname = currentTopoCategory.prefix + tl
                            newConnector.addTriggerLine( TriggerLine( name = tlname, startbit = bit+i, nbits = 1 ), fpga, clock )

        else:
            raise RuntimeError("Property 'format' of connector %s is '%s' but must be either 'multiplicity' or 'topological'")

        if newConnector.ctype == CType.CTPIN:
            try:
                zbThr = connDef["zeroBias"]
                thr = re.match(r"^ZB_\d?(?P<thr>\w*)",zbThr).groupdict()["thr"] # extract the threshold
                if thr not in connDef["thresholds"]:
                    raise RuntimeError("Connector %s contains zero bias threshold %s but not the corresponding threshold %s" % (name, zbThr, thr))
                tl = TriggerLine( name = zbThr, startbit = 30, nbits = 1)
                newConnector.addTriggerLine(tl)
            except KeyError:
                # for connectors without zeroBias threshold
                pass
        


    def json(self):
        confObj = odict()
        for conn in self.connectors.values():
            confObj[conn.name] = conn.json()
        return confObj




class Connector(object):
    __slots__ = [ 'name', 'cformat', 'ctype', 'legacy', 'triggerLines']
    def __init__(self, name, cformat, ctype, legacy):
        """
        @param name name of the connector
        @param cformat can be 'topological' or 'multiplicity'
        @param ctype can be 'ctpin', 'electrical', or 'optical'
        """
        self.name    = name
        self.cformat = CFormat.from_str(cformat)
        self.ctype   = CType.from_str(ctype)
        self.legacy  = legacy
        self.triggerLines = []

    def addTriggerLine(self, tl):
        self.triggerLines.append( tl )

    def json(self):
        confObj = odict()
        confObj["type"] = str(self.ctype)
        confObj["triggerlines"] = [tl.json() for tl in self.triggerLines]
        return confObj


class ElectricalConnector(Connector):
    def __init__(self, name, cformat, legacy):
        """
        @param name name of the connector
        @param cformat can be 'topological' or 'multiplicity'
        """
        super(ElectricalConnector,self).__init__(name = name, cformat = cformat, ctype = 'electrical', legacy = legacy)
        self.triggerLines = { 0 : {0:[],1:[]}, 1 : {0:[],1:[]} }

    def addTriggerLine(self, tl, fpga, clock):
        self.triggerLines[fpga][clock].append( tl )

    def json(self):
        confObj = odict()
        confObj["type"] = str(self.ctype)
        confObj["triggerlines"] = {}
        for fpga in [0,1]:
            fpgas = "fpga%i" % fpga
            confObj["triggerlines"][fpgas] = odict()
            for clock in [0,1]:
                clocks = "clock%i" % clock
                confObj["triggerlines"][fpgas][clocks] = [tl.json() for tl in self.triggerLines[fpga][clock]]
        return confObj


class TriggerLine(object):

    def __init__(self, name, startbit, nbits):
        self.name     = name    
        self.startbit = startbit
        self.nbits    = nbits   

    def json(self):
        confObj = odict()
        confObj["name"]     = self.name
        confObj["startbit"] = self.startbit
        confObj["nbits"]    = self.nbits
        return confObj



