
if not 'vp1InputFile' in dir(): vp1InputFiles = []

# customize the algorithm
from VP1AlgsBatch.VP1AlgsBatchConf import VP1BatchOnLatestEvent
VP1BatchOnLatestEvent.DestinationDirectory="atlasathome_test" 
VP1BatchOnLatestEvent.UseRandomVP1ConfigFile=True


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += VP1BatchOnLatestEvent()
topSequence.TimeOut=0
