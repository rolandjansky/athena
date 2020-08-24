# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigConfIO.TriggerConfigAccessBase import TriggerConfigAccess, ConfigType


class L1MenuAccess(TriggerConfigAccess):
    """
    this class provides access to the L1Menu
    the methods are self-explanatory for people with knowledge of the configuration
    """
    def __init__(self, filename = None, dbalias = None, smkey = None ):
        """
        accessor needs to be initialized with either a filename or the dbalias and smkey
        """
        super(L1MenuAccess,self).__init__( ConfigType.L1MENU, mainkey = "items",
                                           filename = filename, dbalias = dbalias, dbkey = smkey)
        self.loader.setQuery([
            "SELECT L1MT.L1TM_DATA FROM {schema}.SUPER_MASTER_TABLE SMT, {schema}.L1_MENU L1MT WHERE L1MT.L1TM_ID=SMT.SMT_L1_MENU_ID AND SMT.SMT_ID={dbkey}", # for new db schema
            "SELECT L1MT.L1MT_MENU FROM {schema}.SUPER_MASTER_TABLE SMT, {schema}.L1_MASTER_TABLE L1MT WHERE L1MT.L1MT_ID=SMT.SMT_L1_MASTER_TABLE_ID AND SMT.SMT_ID={dbkey}"  # for current db schema
        ])
        self.load()

    def itemNames(self):
        return self._config["items"].keys()

    def itemsWithCtpid(self ):
        return self.items( includeKeys = ['ctpid'] )

    def items(self, includeKeys = [] ):
        if includeKeys:
            """ reduce returned dictionary """
            items = self._config["items"]
            return { x : {k : items[x][k] for k in includeKeys if k in items[x]} for x in items }
        else:
            return self._config["items"]

    def thresholdTypes(self):
        thrTypes = self._config["thresholds"].keys()
        if "legacyCalo" in thrTypes:
            thrTypes.remove("legacyCalo")
            thrTypes += self._config["thresholds"]["legacyCalo"].keys()
        return thrTypes

    def thresholds(self, thresholdType = None):
        if thresholdType:
            if thresholdType == "internal":
                return {}
            if thresholdType in self["thresholds"]:
                return self["thresholds"][thresholdType]["thresholds"]
            if thresholdType in self["thresholds"]["legacyCalo"]:
                return self["thresholds"]["legacyCalo"][thresholdType]["thresholds"]
            raise RuntimeError("Threshold type %s not known in thresholds section of the menu" % thresholdType)
        else:
            thrs = {}
            for thrType in self.thresholdTypes():
                thrs.update( self.thresholds(thrType) )
            return thrs

    def thresholdNames(self, thresholdType = None):
        if thresholdType == "internal":
            return self["thresholds"]["internal"]["names"]
        elif thresholdType is None:
            return self.thresholds().keys() + self.thresholdNames("internal")
        else:
            return self.thresholds(thresholdType).keys()

    def thresholdExtraInfo(self, thresholdType):
        if thresholdType in self["thresholds"]:
            thrDef = self["thresholds"][thresholdType]
        elif thresholdType in self["thresholds"]["legacyCalo"]:
            thrDef = self["thresholds"]["legacyCalo"][thresholdType]
        else:
            raise KeyError("Threshold type %s not known in thresholds section of the menu" % thresholdType)
        return {k:thrDef[k] for k in thrDef if k not in ("thresholds", "type")}

    def topoAlgorithmTypes(self):
        return self["topoAlgorithms"].keys()

    def topoAlgorithms(self, topoAlgoType = None):
        if topoAlgoType:
            return self["topoAlgorithms"][topoAlgoType]
        else:
            d = {}
            for x in self["topoAlgorithms"].values():
                for gr in x.values():
                    d.update(gr)
            return d

    def topoAlgorithmNames(self, topoAlgoType = None):
        allAlgs = self.topoAlgorithms(topoAlgoType)
        return allAlgs.keys()


    def boardNames(self):
        return iter(self["boards"])

    def boards(self):
        return self["boards"]

    def board(self, boardName):
        return self["boards"][boardName]

    def connectorNames(self):
        return iter(self["connectors"])

    def connectors(self):
        return self["connectors"]

    def connector(self, connectorName):
        return self["connectors"][connectorName]

    def ctp(self):
        return self["ctp"]

    def ctpInputs(self, inputType):
        """ inputType should be 'optical', 'electrical' or 'ctpin'
        """
        return self["ctp"]["inputs"][inputType]

    def printSummary(self):
        print("L1 menu %s" % self.name())
        print("Number of items: %i" % len(self) )
        print("Number of threshold types: %i" % len(self.thresholdTypes()) )
        print("Number of thresholds: %i" % len(self.thresholds()) )
        print("Number of topo algorithms: %i" % len(self.topoAlgorithms()))
        print("Number of boards: %i (%i are legacy boards)" % ( len(self.boards()), sum(["legacy" in b for b in self.boards().values()]) ))
        print("Number of connectors: %i (%i are legacy connetors)" % ( len(self.connectors()), sum(["legacy" in c for c in self.connectors().values()]) ))
        print("CTP has %i optical, %i electrical, and %i CTPIN inputs" % ( len(self.ctpInputs("optical")), len(self.ctpInputs("electrical")), 
                                                                           len(set(self.ctpInputs("ctpin")["slot7"].values() + 
                                                                                   self.ctpInputs("ctpin")["slot8"].values() + 
                                                                                   self.ctpInputs("ctpin")["slot9"].values()) - set([""])) ))




class L1PrescalesSetAccess(TriggerConfigAccess):
    """
    this class provides access to the L1 prescales set
    the methods are self-explanatory for people with knowledge of the configuration
    """
    @staticmethod
    def calcPrescaleFromCut(cut):
        """
        turns cut value (which is what the hardware is configured with), into a float prescale value
        """
        return 0xFFFFFF / ( 0x1000000 - cut )

    def __init__(self, filename = None, dbalias = None, l1pskey = None ):
        """
        accessor needs to be initialized with either a filename or the dbalias and l1pskey
        """
        super(L1PrescalesSetAccess,self).__init__( ConfigType.L1PS, mainkey = "cutValues",
                                                   filename = filename, dbalias = dbalias, dbkey = l1pskey )
        self.loader.setQuery([
            "SELECT L1PS_DATA FROM {schema}.L1_PRESCALE_SET WHERE L1PS_ID={dbkey}" # for current and new db schema
        ])
        self.load()


    def itemNames(self):
        return self["cutValues"].keys()
    def cutValues(self):
        return self["cutValues"]
    def cut(self, itemName):
        return self["cutValues"][itemName]["cut"]
    def prescale(self, itemName):
        return L1PrescalesSetAccess.calcPrescaleFromCut( self.cut(itemName) )

    def enabled(self, itemName):
        return self["cutValues"][itemName]["enabled"]

    def printSummary(self):
        print("L1 prescales set %s" % self.name())
        print("Number of prescales: %i" % len(self) )
        print("Number of enabled prescales: %i" % sum(x["enabled"] for x in self["cutValues"].values()) )


class BunchGroupSetAccess(TriggerConfigAccess):
    """
    this class provides access to the L1 bunchgroup set
    the methods are self-explanatory for people with knowledge of the configuration
    """
    def __init__(self, filename = None, dbalias = None, bgskey = None ):
        super(BunchGroupSetAccess,self).__init__( ConfigType.BGS, mainkey = "bunchGroups",
                                                  filename = filename, dbalias = dbalias, dbkey = bgskey )
        self.loader.setQuery([
            "SELECT L1BGS_DATA FROM {schema}.L1_BUNCH_GROUP_SET WHERE L1BGS_ID={dbkey}" # for current and new db schema
        ])
        self.load()

