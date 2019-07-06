# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger("Menu.L1.Base.L1Menu2JSON")


class L1MenuJSONConverter(object):

    def __init__(self, l1menu = None, outputFile = None , inputFile = None ):
        self.menu          = l1menu
        self.inputFile     = inputFile
        self.outputFile    = outputFile

    def writeJSON(self,pretty=False):

        if not self.outputFile:
            log.warning("Can't write json file since no name was provided")
            return

        confObj = self.generateJSON()

        with open( self.outputFile, mode="wt" ) as fh:
            import json
            json.dump(confObj, fh, indent = 4 if pretty else None)
        log.info("Wrote %s", self.outputFile)
        return self.outputFile



    def generateJSON(self):
        #        import json
        from collections import OrderedDict as odict
        confObj = odict()

        confObj["Name"] = self.menu.menuName

        # items
        confObj["Items"] = odict()
        jItems = confObj["Items"]
        for item in self.menu.items:
            jItems[item.name] = item.json()

        # bunchgroups
        confObj["Bunchgroups"] = self.menu.ctp.bunchGroupSet.json()

        # thresholds
        confObj["Thresholds"] = odict()
        confObj["Thresholds"]["internal"] = odict()
        confObj["Thresholds"]["internal"]["names"] = [ "BGRP%i" % bg.internalNumber for bg in self.menu.ctp.bunchGroupSet.bunchGroups] + \
                                                     [ "RNDM%i" % i for i in range(0,len(self.menu.ctp.random.names)) ]
        confObj["Thresholds"]["internal"]["randoms"] = odict( [ ("RNDM%i" %i , { "cut" : c }) for i,c in enumerate( self.menu.ctp.random.cuts) ] )

        # run 3 thresholds
        confObj["Thresholds"].update( self.menu.thresholds.json() )

        # legacy calo thresholds
        confObj["Thresholds"]["legacyCalo"] = self.menu.thresholds.jsonLegacy()

        # topo algorithms
        confObj["TopoAlgorithms"] = self.menu.topoAlgos.json()

        # board definition
        confObj["BoardDefinition"] = self.menu.boards.json()

        # connectors definition
        confObj["Connectors"] = self.menu.connectors.json()

        # CTP input cabling definition
        confObj["CTP"] = self.menu.ctp.json()

        return confObj




