from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence("Streams")

if hasattr(job, 'StreamHITS'):
    job.StreamHITS.ItemList += ["LArHitContainer#*"]
    job.StreamHITS.ItemList += ["TileHitVector#TileHitVec_Fast"]

else:
    raise AttributeError("StreamHITS not found!")



