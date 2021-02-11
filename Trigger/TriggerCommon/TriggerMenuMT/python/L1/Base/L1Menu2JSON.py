# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger("Menu.L1.Base.L1Menu2JSON")


class L1MenuJSONConverter(object):

    def __init__(self, l1menu = None, outputFile = None, bgsOutputFile = None, inputFile = None ):
        self.menu          = l1menu
        self.inputFile     = inputFile
        self.outputFile    = outputFile
        self.bgsOutputFile = bgsOutputFile

    def writeJSON(self,pretty=False):

        if not self.outputFile:
            log.warning("Can't write json file since no name was provided")
            return

        # L1Menu json
        confObj = self.generateJSON()
        with open( self.outputFile, mode="wt" ) as fh:
            import json
            json.dump(confObj, fh, indent = 4 if pretty else None, separators=(',', ': '))
        log.info("Wrote %s", self.outputFile)

        if self.bgsOutputFile is not None:
            confObj = self.generateJsonBunchgroupset()
            with open( self.bgsOutputFile, mode="wt" ) as fh:
                import json
                json.dump(confObj, fh, indent = 4 if pretty else None, separators=(',', ': '))
            log.info("Wrote %s", self.bgsOutputFile)


        return self.outputFile



    def generateJSON(self):
        #        import json
        from collections import OrderedDict as odict
        confObj = odict()

        confObj["filetype"] = "l1menu" 

        confObj["name"] = self.menu.menuName

        # items
        confObj["items"] = self.menu.items.json()

        # bunchgroups
        confObj["bunchGroups"] = self.menu.ctp.bunchGroupSet.json()

        # thresholds
        confObj["thresholds"] = odict()
        confObj["thresholds"]["internal"] = odict()
        confObj["thresholds"]["internal"]["type"] = "internal"
        confObj["thresholds"]["internal"]["names"] = [ "BGRP%i" % bg.internalNumber for bg in self.menu.ctp.bunchGroupSet.bunchGroups] + \
                                                     [ "RNDM%i" % i for i in range(0,len(self.menu.ctp.random.names)) ]
        confObj["thresholds"]["internal"]["randoms"] = odict( [ ("RNDM%i" %i , { "cut" : c }) for i,c in enumerate( self.menu.ctp.random.cuts) ] )

        # run 3 thresholds
        confObj["thresholds"].update( self.menu.thresholds.json() )

        # legacy calo thresholds
        confObj["thresholds"]["legacyCalo"] = self.menu.thresholds.jsonLegacy()

        # topo algorithms
        confObj["topoAlgorithms"] = self.menu.topoAlgos.json()

        # board definition
        confObj["boards"] = self.menu.boards.json()

        # connectors definition
        confObj["connectors"] = self.menu.connectors.json()

        # CTP input cabling definition
        confObj["ctp"] = self.menu.ctp.json()

        return confObj


    def generateJsonBunchgroupset(self):
        from collections import OrderedDict as odict
        confObj = odict()

        confObj["filetype"] = "bunchgroupset" 

        confObj["name"] = self.menu.menuName

        # bunchgroups
        confObj["bunchGroups"] = self.menu.ctp.bunchGroupSet.json()

        return confObj
