# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigMissingETHypo.TrigMissingETHypoConf import TrigMissingETHypoAlgMT, TrigMissingETHypoToolMT

from AthenaCommon.SystemOfUnits import GeV


class MissingETHypoAlgMT(TrigMissingETHypoAlgMT):
    __slots__ = []
    def __init__(self, name, hTools=[], metKey=""):
        super( MissingETHypoAlgMT, self ).__init__( name )

        if len(hTools)!=0: 
            self.HypoTools = hTools 
        if metKey!="": 
            self.METContainerKey = metKey 

        # Monitoring code copied from old hypo config file.
        # Kept here in case it may be useful during development
        # Will be removed/re-implemented soon.
        '''
        from TrigMissingETHypo.TrigMissingETHypoMonitoring import TrigEFMissingETHypoValidationMonitoring, TrigEFMissingETHypoCosmicMonitoring
        validation = TrigEFMissingETHypoValidationMonitoring()
        cosmic = TrigEFMissingETHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("METHypo_Time")
        time.TimerHistLimits = [0, 5]

        self.AthenaMonTools = [ time, validation, cosmic ]
        '''

    def onlineMonitoring(self):
        from TrigMissingETHypo.TrigMissingETHypoMonitoringTool import TrigMissingETHypoMonitoringTool
        self.MonTool = TrigMissingETHypoMonitoringTool()

class MissingETHypoToolMT(TrigMissingETHypoToolMT):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( MissingETHypoToolMT, self ).__init__( name )

        # Configure threshold from trigger name
        if 'alg' in kwargs:
            trigParts = name.split('_')
            alg = kwargs['alg']
            if alg=='cell':
                alg = trigParts[-1]
            idx = trigParts.index(alg) 
            self.metThreshold = int(filter(str.isdigit, trigParts[idx-1]))


            
def TrigMETCellHypoToolFromDict(chainDict):
    """ Configure tool operating on met from cells"""
    # note for future developers, it seems that the chainDict has the information about the type of alg, it would be god to use it
    # not calling the class above as it tries to parse the name back
    # also there seems no property to decide if it is met from cells yet, not setting it therefore
    # possibly there would be only one function if the met source is available in the chainDict and settable tool property
    
    tool = MissingETHypoToolMT( chainDict['chainName'] )
    tool.metThreshold = int(chainDict['chainParts'][0]['threshold'])
    
    return tool
            

def TrigMETCellHypoToolFromName(name, conf):
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName
    
    decoder = DictFromChainName()    
    decodedDict = decoder.analyseShortName(conf, [], "") # no L1 info
    decodedDict['chainName'] = name 
    
    return TrigMETCellHypoToolFromDict( decodedDict )


def TrigMETPufitHypoToolFromName(name, conf):
    return MissingETHypoToolMT(name, alg='pufit')


def TrigMETPufitHypoToolFromName(name, conf):
    return MissingETHypoToolMT(name, alg='pufit')

def TrigMETJetHypoToolFromName(name, conf):
    return MissingETHypoToolMT(name, alg='mht')


if __name__ == "__main__":
    confCell = TrigMETCellHypoToolFromName("HLT_xe65_L1XE50", "HLT_xe65_L1XE50")
    assert confCell, "Cell tool not configured"


    
