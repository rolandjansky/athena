
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.Constants import DEBUG

def getFlavourTagging( inputJets, inputVertex, inputTracks ):
    algSequence = []
    
    from TrigBjetHypo.TrigBjetHypoConf import TrigBtagFexMT
    bTagFex = TrigBtagFexMT( "TrigBtagFexMT" )
    bTagFex.OutputLevel = DEBUG
    bTagFex.JetKey = inputJets
    bTagFex.PriVtxKey = inputVertex
    bTagFex.TracksKey = inputTracks
    bTagFex.OutputBTagging = recordable( "HLT_BTagging" )
    algSequence.append( bTagFex )

    return algSequence



