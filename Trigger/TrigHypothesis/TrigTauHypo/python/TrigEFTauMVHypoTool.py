def TrigEFTauMVHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    from TrigTauHypo.TrigTauHypoConf import TrigEFTauMVHypoTool
    currentHypo = TrigEFTauMVHypoTool(name)
    currentHypo.level = 1
    currentHypo.method = 3
    currentHypo.MonTool = ""

    return currentHypo
