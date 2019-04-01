
def TrigTauTrackHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    currentHypo = TrigTrackPreSelHypoTool(name)
    currentHypo.MonTool = ""

    return currentHypo
