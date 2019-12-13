
#from AthenaCommon.Constants import DEBUG

def getFlavourTagging( inputJets, inputVertex, inputTracks ):
    algSequence = []
    
    from TrigBjetHypo.TrigBjetHypoConf import TrigBtagFexMT
    bTagFex = TrigBtagFexMT( "TrigBtagFexMT" )
    bTagFex.JetKey = inputJets
    bTagFex.PriVtxKey = inputVertex
    bTagFex.TracksKey = inputTracks
    algSequence.append( bTagFex )

    return algSequence



