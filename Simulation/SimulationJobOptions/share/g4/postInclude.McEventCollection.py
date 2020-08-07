#########################################################
#########################################################
from AthenaCommon.AlgSequence import AthSequencer
## configure Athena for POOL persistency
streamAlgs = ['StreamHITS']
streamSeq = AthSequencer("AthOutSeq")
for stream in streamAlgs:
    sSoutStream = getattr(streamSeq, stream, None)
    if sSoutStream is not None:
        sSoutStream.ItemList += [ "McEventCollection#*" ]
