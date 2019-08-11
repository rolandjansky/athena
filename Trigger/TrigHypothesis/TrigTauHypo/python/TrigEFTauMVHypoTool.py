# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def TrigEFTauMVHypoToolFromDict( chainDict ):

    name = chainDict['chainName']

    from TrigTauHypo.TrigTauHypoConf import TrigEFTauMVHypoTool
    currentHypo = TrigEFTauMVHypoTool(name)
    currentHypo.level = 1
    currentHypo.method = 3
    currentHypo.MonTool = ""

    return currentHypo
