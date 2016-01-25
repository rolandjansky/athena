# TODO: make this declaration more automatic

topSeq.G4AtlasAlg.ExtraInputs =  [('McEventCollection','GEN_EVENT')]
topSeq.G4AtlasAlg.ExtraOutputs = [('SiHitCollection','SCT_Hits'),('RecoTimingObj','EVNTtoHITS_timings')]

topSeq.StreamHITS.ExtraInputs += topSeq.G4AtlasAlg.ExtraOutputs
# Disable alg filtering - doesn't work in multi-threading
topSeq.StreamHITS.AcceptAlgs = []
algCardinality = jp.ConcurrencyFlags.NumThreads()

if (algCardinality != 1):
    for alg in topSeq:
        name = alg.name()
#        if name in ["SGInputLoader", "StreamHITS"]:
        if name in ["StreamHITS"]:
            print 'Disabling cloning/cardinality for', name
            # Don't clone these algs
            alg.Cardinality = 1
            alg.IsClonable = False
        else:
            alg.Cardinality = algCardinality
            alg.IsClonable = True
