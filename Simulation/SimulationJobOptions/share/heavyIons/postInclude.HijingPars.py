#########################################################
#
# SimulationJobOptions/postOptions.HijingPars.py
# Andrzej Olszewski
#
# Mods for Hijing event parameters.
#
# This job option should be added via the postInclude
# command line argument. I.e. before theApp.initialize()
# is called.
#
#########################################################
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()

from AthenaCommon.DetFlags import DetFlags
if DetFlags.pileup.any_on():
    index = 0
    for alg in topSequence:
        index += 1 #count position
        if 'PileUpToolsAlg' in alg.name():
            from AthenaCommon.CfgGetter import getPrivateTool
            alg.PileUpTools+=[getPrivateTool("MergeHijingParsTool")]
            break
        if 'MergeMcEventCollection' == alg.name():
            from MCTruthSimAlgs.MCTruthSimAlgsConf import MergeHijingPars
            mergeHijingPars = MergeHijingPars()
            topSequence.insert(index, mergeHijingPars)
            break
else:
    try:
        from TruthIO.TruthIOConf import PrintHijingPars
    except:
        from TruthExamples.TruthExamplesConf import PrintHijingPars
    topSequence += PrintHijingPars()


## configure Athena for POOL persistency
streamAlgs = ['StreamHITS','StreamRDO','StreamESD','StreamAOD']
streamSeq = AthSequencer("AthOutSeq")
for stream in streamAlgs:
    sSoutStream = getattr(streamSeq, stream, None)
    tSoutStream = getattr(topSequence, stream, None)
    if tSoutStream is not None:
        tSoutStream.ItemList += [ "HijingEventParams#Hijing_event_params" ]
    else:
        if sSoutStream is not None:
            sSoutStream.ItemList += [ "HijingEventParams#Hijing_event_params" ]
