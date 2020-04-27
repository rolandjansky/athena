# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigConfIO.TriggerConfigAccessBase import TriggerConfigAccess, ConfigType

class HLTMenuAccess(TriggerConfigAccess):
    """
    this class provides access to the HLT menu
    the methods are self-explanatory for people with knowledge of the configuration
    """
    def __init__(self, filename = None, dbalias = None, smkey = None ):
        """
        accessor needs to be initialized with either a filename or the dbalias and smkey
        """
        super(HLTMenuAccess,self).__init__( ConfigType.HLTMENU, mainkey = "chains",
                                            filename = filename, dbalias = dbalias, dbkey = smkey )
        self.loader.setQuery([
            "SELECT HTM_DATA FROM {schema}.HLT_MENU WHERE HTM_ID={dbkey}", # for new db schema
            "SELECT HMT.HMT_MENU FROM {schema}.HLT_MASTER_TABLE HMT WHERE HMT.HMT_ID={dbkey}"  # for current db schema
        ])
        self.load()

    def chainNames(self):
        return (x["name"] for x in self)

    def chains(self):
        return iter(self)

    def sequencers(self):
        return self["sequencers"]

    def printSummary(self):
        print("HLT menu %s" % self.name())
        print("Number of chains: %i" % len(self) )
        print("Number of sequencers: %i" % len(self.sequencers()) )



class HLTPrescalesSetAccess(TriggerConfigAccess):
    """
    this class provides access to the HLT prescales set
    the methods are self-explanatory for people with knowledge of the configuration
    """
    def __init__(self, filename = None, dbalias = None, hltpskey = None ):
        """
        accessor needs to be initialized with either a filename or the dbalias and hlpskey
        """
        super(HLTPrescalesSetAccess,self).__init__( ConfigType.HLTPS, mainkey = "prescales",
                                                    filename = filename, dbalias = dbalias, dbkey = hltpskey )
        self.loader.setQuery([
            "SELECT HPS_DATA FROM {schema}.HLT_PRESCALE_SET WHERE HPS_ID={dbkey}" # for current and new db schema
        ])
        self.load()


    def prescales(self):
        return self["prescales"]

    def chainNames(self):
        return iter(self)

    def prescale(self, chainName):
        return self["prescales"][chainName]["prescale"]

    def enabled(self, chainName):
        return self["prescales"][chainName]["enabled"]

    def printSummary(self):
        print("HLT prescales set %s" % self.name())
        print("Number of prescales: %i" % len(self) )
        print("Number of enabled prescales: %i" % sum(x["enabled"] for x in self["prescales"].values()) )



class HLTJobOptionsAccess(TriggerConfigAccess):
    """
    this class provides access to the HLT algorithm configuration
    the methods are self-explanatory for people with knowledge of the configuration
    """
    def __init__(self, filename = None, dbalias = None, smkey = None ):
        """
        accessor needs to be initialized with either a filename or the dbalias and smkey
        """
        super(HLTJobOptionsAccess,self).__init__( ConfigType.HLTJO, mainkey = "properties",
                                                  filename = filename, dbalias = dbalias, dbkey = smkey )
        self.loader.setQuery([
            "SELECT HST_DATA FROM {schema}.HLT_SETUP WHERE HST_ID={dbkey}", # for new db schema
            "SELECT JO.JO_CONTENT FROM {schema}.JO_MASTER_TABLE JO WHERE JO.JO_ID={dbkey}"  # for current db schema
        ])
        self.load()

    def algorithms(self):
        return self["properties"]

    def algorithmNames(self):
        return iter(self)

    def properties(self, algName):
        return self["properties"][algName]
        
    def printSummary(self):
        print("Job options")
        print("Number of algorithms: %i" % len(self) )
        print("Number of properties: %i" % sum(len(alg) for alg in self.algorithms().values()) )
