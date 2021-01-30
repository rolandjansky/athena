## configure Athena for POOL persistency
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
streamSeq = AlgSequence("Streams")
sSoutStream = getattr(streamSeq, 'StreamHITS', None)
tSoutStream = None #getattr(topSequence, 'StreamHITS', None)
StreamHITS = None
if tSoutStream is not None:
    StreamHITS = tSoutStream
else:
    if sSoutStream is not None:
        StreamHITS = sSoutStream

if StreamHITS is not None:
    StreamHITS.ItemList += ['McEventCollection#NewTruthEvent']

#StreamHITS.TakeItemsFromInput=False;
print StreamHITS
