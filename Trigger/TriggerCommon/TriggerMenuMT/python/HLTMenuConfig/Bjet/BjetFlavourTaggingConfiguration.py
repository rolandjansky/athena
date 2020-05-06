# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigEDMConfig.TriggerEDMRun3 import recordable
#from AthenaCommon.Constants import DEBUG

def getFlavourTagging( inputJets, inputVertex, inputTracks ):
    algSequence = []
    
    from TrigBjetHypo.TrigBjetHypoConf import TrigBtagFexMT
    bTagFex = TrigBtagFexMT( "TrigBtagFexMT" )
    bTagFex.JetKey = inputJets
    bTagFex.PriVtxKey = inputVertex
    bTagFex.TracksKey = inputTracks
    bTagFex.OutputBTagging = recordable( "HLT_BTagging" )

    from TrigBjetHypo.TrigBtagFexMTConfig import TrigBtagFexMT_OnlineMonitoring
    bTagFex.MonTool = TrigBtagFexMT_OnlineMonitoring()

    algSequence.append( bTagFex )

    return algSequence



