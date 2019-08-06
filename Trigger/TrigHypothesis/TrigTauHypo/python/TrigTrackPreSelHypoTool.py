# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def TrigTauTrackHypoToolFromDict( chainDict ):

    name = chainDict['chainName']

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    currentHypo = TrigTrackPreSelHypoTool(name)
    currentHypo.MonTool = ""

    return currentHypo
